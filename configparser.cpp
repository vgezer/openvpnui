/*  Copyright 2016 Volkan Gezer

//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at

//  http://www.apache.org/licenses/LICENSE-2.0

//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License. */


#include "defines.h"
#include "configparser.h"
#include "vpngui.h"
#include <QFile>
#include <QMap>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

ConfigParser::ConfigParser() {}

void ConfigParser::readConfig() {
    readConfig(true);
}

void ConfigParser::cleanConfig() {

    fileContents.clear();
    updateManual();
}

void ConfigParser::saveConfig() {

    if(!isCaKeyActive("ca") || !isCaKeyActive("cert") || !isCaKeyActive("key")) {
        QMessageBox confirmationMsg(
                    QMessageBox::Warning,
                    tr("Confirmation"),
                    tr("At least one of required certificates is missing. The configuration might"
                       " not work. Continue?"),
                    QMessageBox::Yes | QMessageBox::No);

        confirmationMsg.setButtonText(QMessageBox::Yes, tr("Yes"));
        confirmationMsg.setButtonText(QMessageBox::No, tr("No"));

        if (confirmationMsg.exec() == QMessageBox::No) {
            return;
        }
    }
    QFile *file = new QFile;
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save OpenVPN Configuration", "", "Open VPN Configuration (*.ovpn)");
    file->setFileName(fileName);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
            return;
    QTextStream out(file);

    QString wholeConfig = "# Config created by OpenVPN UI #\n"
                          "# https://github.com/vgezer/openvpnui #\n" +
                          fileContents;

    out << wholeConfig;

}

void ConfigParser::createDefaultConfig() {
    fileContents = "# Config created by OpenVPN UI #\n"
                   "# https://github.com/vgezer/openvpnui #\n"
                   "client\ndev tun\nproto udp\nremote example.org 1194\n"
                   "resolv-retry infinite\nuser nobody\ngroup nogroup\n"
                   "ns-cert-type server\ncomp-lzo\nnobind\npersist-key\n"
                   "persist-tun\nverb 3\n";
                    // do not forget new line at the end
    emit paramChanged();
    updateManual();
}

void ConfigParser::updateManual() {
    readConfig(false);
}

void ConfigParser::readConfig(bool _fromFile) {

    QFile *file = new QFile;
    QTextStream *in;
    configFileNoValue->clear();
    configFilePairs.clear();
    configFileCaKey.clear();

    if(_fromFile) {
        QString fileName = QFileDialog::getOpenFileName(this,
            "Select OpenVPN Configuration", "", "Open VPN Configuration (*.ovpn)");
        file->setFileName(fileName);
        if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
                return;
        in = new QTextStream(file);
    }
    else {
        in = new QTextStream(new QString(fileContents));
        fileContents.clear();
    }

        while (!in->atEnd()) {

            QString line = in->readLine();
            if(line.size() > 0) {
                if(line.at(0) != '#' && line.at(0) != ';') {

                    if(line.contains("<ca>") || line.contains("<cert>") || line.contains("<key>")) {
                        configFileCaKey.insert("ca", line.contains("<ca>"));
                        configFileCaKey.insert("cert", line.contains("<cert>"));
                        configFileCaKey.insert("key", line.contains("<key>"));

                    }

                    //qDebug() << line;
                    QMap<QString, QString>::const_iterator i = availablePairs.constBegin();
                    while (i != availablePairs.constEnd()) {

                        if(line.contains(i.key() + " ")) {
                            configFilePairs.insert(i.key(), processLine(line));
                            //qDebug() << "appending: " << i.key() << line;
                        }
                        ++i;
                    }
                    for(QStringList::iterator config = availableNoValue->begin(); config != availableNoValue->end(); ++config) {
                        if(line.contains(*config) && !configFileNoValue->contains(*config)) {

                            configFileNoValue->append(*config);
                            //qDebug() << "appending (single): " << *config;
                        }
                    }
                    fileContents += line + "\n";
                }
            }

        }
        updateFields();
}

void ConfigParser::updateFields() {
    emit configFileOpened();
}

void ConfigParser::removeLine(const QString _line) {
    if(this->isConfigActive(_line)) {

        configFileNoValue->removeOne(_line);
        configFilePairs.remove(_line.left(_line.indexOf(" ")));
        QString configKey = _line.left(_line.indexOf(" "));
        fileContents.remove(fileContents.indexOf(configKey), fileContents.indexOf("\n", fileContents.indexOf(configKey)) -
                            fileContents.indexOf(configKey) + 1);

        emit paramChanged();
    }
}

void ConfigParser::addLine(const QString _line) {
    if(this->isConfigActive(_line)) {
        removeLine(_line);
    }
    //qDebug() << "config is not active adding" + _line;
    if(_line.indexOf(" ") > 0) {
        configFilePairs.insert(_line.left(_line.indexOf(" ")), processLine(_line));
    }
    else {
        configFileNoValue->append(_line);
    }
    fileContents.append(_line + "\n");
    emit paramChanged();
}

void ConfigParser::addTags(const QString _tag, const QString _content) {

    if(!_content.contains("N/A")) {
        QString toAdd = "\n<" + _tag + ">" + _content + "</" + _tag + ">";
        removeTags(_tag);
        addLine(toAdd);
        configFileCaKey.insert(_tag, true);

    }
}

void ConfigParser::removeTags(const QString _tag) {

    int tagStart = fileContents.indexOf("<" + _tag + ">");
    int tagFinish = fileContents.indexOf("</" + _tag + ">") + _tag.length() + 2;
    // removal for tags is a bit different
    fileContents.remove(fileContents.indexOf("<" + _tag + ">"), tagFinish - tagStart + 3);
    configFileCaKey.insert(_tag, false);
}

QString ConfigParser::processLine(const QString _line) {
    return _line.mid(_line.indexOf(" ")+1, _line.length());

}


QString ConfigParser::getConfigValue(const QString _configKey) {
    return configFilePairs[_configKey];
}

QString ConfigParser::getDefaultConfigValue(const QString _configKey) {
    return availablePairs[_configKey];
}

bool ConfigParser::isConfigActive(const QString _configKey) {
    bool first = configFileNoValue->contains(_configKey);
    bool second = configFilePairs.contains(_configKey.left(_configKey.indexOf(" ")));
    return first || second;
}

bool ConfigParser::isCaKeyActive(const QString _tag) {
    return configFileCaKey.value(_tag);
}

void ConfigParser::setCaKeyFlag(QString _tag, bool _active) {
    configFileCaKey.insert(_tag, _active);
}


void ConfigParser::setFileContents(const QString _newValue) {
    fileContents = _newValue;
}

QString ConfigParser::getFileContents() const {
    return fileContents;
}

