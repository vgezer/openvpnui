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

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <QDialog>
#include <QMap>

QT_BEGIN_NAMESPACE

class QFile;
QT_END_NAMESPACE
class ConfigParser : public QDialog
{
    Q_OBJECT
public:
    ConfigParser();
    QString getFileContents() const;
    void setFileContents(const QString _newValue);
    QString getDefaultConfigValue(const QString _configKey);
    QString getConfigValue(const QString _configKey);
    bool isConfigActive(const QString _configKey);
    bool isCaKeyActive(const QString _tag);
    void setCaKeyFlag(QString _tag, bool _active);
    void setFileName(const QString _fileName);
    void addLine(QString _line);
    void removeLine(QString _line);
    void addTags(const QString _tag, const QString _content);
    void removeTags(const QString _tag);

public slots:
    void readConfig(bool _fromFile);
    void readConfig();
    void updateManual();
    void createDefaultConfig();
    void cleanConfig();
    void saveConfig();

signals:
   bool configFileOpened();
   void paramChanged();

private:
    QMap<QString, QString> availablePairsDefault{{"dev", "tun"}, {"proto", "udp"},
                                                 {"remote", "my-server-1 1194"},
                                                 {"resolv-retry", "infinite"},
                                                 {"user" , "nobody"},
                                                 {"group", "nogroup"},
                                                 {"ns-cert-type", "server"},
                                                 {"verb" ,"3"}};

    QStringList *availableNoValueDefault = new QStringList{"client", "nobind", "persist-key", "persist-tun", "comp-lzo"};

    QMap<QString, QString> availablePairs{{"dev", "tun"}, {"dev-node", "MyTap"},
                                          {"proto", "udp"}, {"remote", "my-server-1 1194"},
                                          {"http-proxy","[proxy server] [proxy port #]"},
                                          {"resolv-retry", "infinite"}, {"user", "nobody"},
                                          {"group", "nogroup"}, {"ns-cert-type", "server"},
                                          {"tls-auth","ta.key 1"}, {"cipher","x"}, {"verb","3"},
                                          {"mute","20"}, {"tun-mtu","1500"},
                                          {"route-delay","0"}};

    QStringList *availableNoValue = new QStringList{"client", "remote-random",
            "nobind", "persist-key", "persist-tun",
            "mute-replay-warnings", "comp-lzo", "redirect-gateway",
            "auth-user-pass", "auth-nocache", "float"};

    QMap<QString, QString> configFilePairs;
    QStringList *configFileNoValue = new QStringList();
    QMap<QString, bool> configFileCaKey{{"ca", false}, {"cert", false}, {"key", false}};



    QFile *file;
    QString fileName;
    QString fileContents;
    QString processLine(const QString _line);
    void updateFields();
};

#endif // CONFIGPARSER_H
