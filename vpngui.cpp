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


#include <QtWidgets>
#include <QFileDialog>

#include "vpngui.h"
#include "defines.h"
#include "configparser.h"

VPNGui::VPNGui(ConfigParser *_configParser, QWidget *parent)
    : QDialog(parent)
{
    tabWidget = new QTabWidget;
    tabWidget->addTab(new QuickSettingsTab(_configParser), tr("Basic"));
    tabWidget->addTab(new GeneralSettingsTab(_configParser), tr("General"));
    tabWidget->addTab(new ManualEditTab(_configParser), tr("Manual Configuration Editor"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    createMenu(_configParser);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->setMenuBar(menuBar);
    setLayout(mainLayout);

    setWindowTitle(APPNAME);
}

void VPNGui::keyPressEvent(QKeyEvent *e) {
    if(e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
    else {/* minimize */}
}

QuickSettingsTab::QuickSettingsTab(ConfigParser *_configParser, QWidget *parent)
    : QWidget(parent)
{
    m_pConfigParser = _configParser;
    createQuickOptions();
    connect(_configParser, SIGNAL(configFileOpened()), this, SLOT(updateValues()));
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(m_pQuickSettingsLayout);
    setLayout(layout);
}

GeneralSettingsTab::GeneralSettingsTab(ConfigParser *_configParser, QWidget *parent)
    : QWidget(parent)
{
    m_pConfigParser = _configParser;
    createGeneralOptions();
    connect(_configParser, SIGNAL(configFileOpened()), this, SLOT(updateValues()));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_pGeneralSettingsLayout);
    setLayout(layout);
}

ManualEditTab::ManualEditTab(ConfigParser *_configParser, QWidget *parent)
    : QWidget(parent)
{
    m_pConfigParser = _configParser;
    createManualEditOptions();
    connect(_configParser, SIGNAL(configFileOpened()), this, SLOT(updateValues()));
    connect(_configParser, SIGNAL(paramChanged()), this, SLOT(updateValues()));
    connect(m_pConfigEdit, SIGNAL(textChanged()), this, SLOT(refreshFileContents()));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_pManualSettingsLayout);
    setLayout(layout);
}

void VPNGui::createMenu(ConfigParser *_configParser)
{
    menuBar = new QMenuBar;

    fileMenu = new QMenu(tr("&File"), this);
    newConfigAction = fileMenu->addAction(tr("&Create blank configuration"));
    openConfigAction = fileMenu->addAction(tr("&Open configuration..."));
    saveCreatedConfigAction = fileMenu->addAction(tr("&Save configuration..."));
    exitAction = fileMenu->addAction(tr("E&xit"));

    configMenu = new QMenu(tr("&Configuration"), this);
    createDefaultConfigAction = configMenu->addAction(tr("&Create default configuration"));
    updateConfigAction = configMenu->addAction(tr("&Update modified configuration"));

    helpMenu = new QMenu(tr("&Help"), this);
    aboutAction = helpMenu->addAction(tr("&About %1").arg(APPNAME));

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(configMenu);
    menuBar->addMenu(helpMenu);

    connect(newConfigAction, SIGNAL(triggered()), _configParser, SLOT(cleanConfig()));
    connect(openConfigAction, SIGNAL(triggered()), _configParser, SLOT(readConfig()));
    connect(saveCreatedConfigAction, SIGNAL(triggered()), _configParser, SLOT(saveConfig()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(exit()));

    connect(createDefaultConfigAction, SIGNAL(triggered()), _configParser,
            SLOT(createDefaultConfig()));
    connect(updateConfigAction, SIGNAL(triggered()), _configParser, SLOT(updateManual()));

    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAboutDlg()));
}

void VPNGui::showAboutDlg() {
    QMessageBox aboutMsg(QMessageBox::NoIcon,
                         tr("About %1 %2").arg(APPNAME).arg(VERSION),
                         tr("Copyright %1 - %2<br />"
                            "%3 is an open source configuration"
                            " creator UI for OpenVPN. The source code is"
                            " available at: <a href=\"%4\">%4</a>").arg(QDate::currentDate().year()).arg("<a href=\"mailto:volkangezer@gmail.com?subject=OpenVPN UI\">Volkan Gezer</a>").arg(APPNAME).arg(GITHUBLINK));

    aboutMsg.setTextFormat(Qt::RichText);
    aboutMsg.exec();

}

void VPNGui::exit() {

    QMessageBox confirmationMsg(
                QMessageBox::Question,
                tr("Confirmation"),
                tr("Are you sure you want to quit?"),
                QMessageBox::Yes | QMessageBox::No);

    confirmationMsg.setButtonText(QMessageBox::Yes, tr("Yes"));
    confirmationMsg.setButtonText(QMessageBox::No, tr("No"));

    if (confirmationMsg.exec() == QMessageBox::Yes) {
        this->accept();
    }
}

void VPNGui::reject() {
    exit();
}

void QuickSettingsTab::createQuickOptions()
{
    m_pQuickSettingsLayout = new QGroupBox(tr("Simple Settings"));
    QFormLayout *layout = new QFormLayout;

    m_pProfileLabel = new QLabel(tr("Profile name:"));
    m_pProfileEdit = new QLineEdit;


    m_pRemoteHostLabel = new QLabel(tr("Remote server:"));
    m_pRemoteHostEdit = new QLineEdit;
    connect(m_pRemoteHostEdit, SIGNAL(editingFinished()), this, SLOT(setConfig()));

    m_pRemotePortLabel = new QLabel(tr("Remote port:"));
    m_pRemotePortSpinbox = new QSpinBox;
    m_pRemotePortSpinbox->setRange(0, 65535);
    m_pRemotePortSpinbox->setValue(1194);
    connect(m_pRemotePortSpinbox, SIGNAL(editingFinished()), this, SLOT(setConfig()));

    m_pProtocolLabel = new QLabel(tr("Server protocol:"));
    m_pProtocolComboBox = new QComboBox;
    m_pProtocolComboBox->addItem("UDP");
    m_pProtocolComboBox->addItem("TCP");
    connect(m_pProtocolComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setConfig()));

    m_pResolveTryLabel = new QLabel(tr("Resolve retry:"));
    m_pResolveTryEdit = new QLineEdit;
    connect(m_pResolveTryEdit, SIGNAL(editingFinished()), this, SLOT(setConfig()));

    m_pLoadServerCertLabel = new QLabel(tr("Load server certificate:"));
    m_pLoadServerCertBtn = new QPushButton(tr("Browse..."));
    connect(m_pLoadServerCertBtn, SIGNAL(released()), this, SLOT(addCertKey()));

    m_pLoadUserCertLabel = new QLabel(tr("Load user certificate:"));
    m_pLoadUserCertBtn = new QPushButton(tr("Browse..."));
    connect(m_pLoadUserCertBtn, SIGNAL(released()), this, SLOT(addCertKey()));

    m_pLoadUserKeyLabel = new QLabel(tr("Load user private key:"));
    m_pLoadUserKeyBtn = new QPushButton(tr("Browse..."));
    connect(m_pLoadUserKeyBtn, SIGNAL(released()), this, SLOT(addCertKey()));


    layout->addRow(m_pProfileLabel, m_pProfileEdit);
    layout->addRow(m_pRemoteHostLabel, m_pRemoteHostEdit);
    layout->addRow(m_pRemotePortLabel, m_pRemotePortSpinbox);
    layout->addRow(m_pProtocolLabel, m_pProtocolComboBox);
    layout->addRow(m_pResolveTryLabel, m_pResolveTryEdit);
    layout->addRow(m_pLoadServerCertLabel, m_pLoadServerCertBtn);
    layout->addRow(m_pLoadUserCertLabel, m_pLoadUserCertBtn);
    layout->addRow(m_pLoadUserKeyLabel, m_pLoadUserKeyBtn);

    m_pQuickSettingsLayout->setLayout(layout);
}

QString QuickSettingsTab::readContents() {
    QFile *file = new QFile;
    QTextStream *in;
    QString fileName = QFileDialog::getOpenFileName(this,
        "Select file to load...", "", "All Files (*.*)");
    file->setFileName(fileName);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
            return "N/A";
    in = new QTextStream(file);
    return in->readAll();
}


void QuickSettingsTab::addCertKey() {
    QObject* option = sender();
    QString value = readContents();

    if(option == m_pLoadServerCertBtn) {
        m_pConfigParser->addTags("ca", value);
        if(!value.contains("N/A")) m_pLoadServerCertBtn->setText(tr("ADDED"));

    }
    if(option == m_pLoadUserCertBtn) {

        m_pConfigParser->addTags("cert", value);
        if(!value.contains("N/A")) m_pLoadUserCertBtn->setText(tr("ADDED"));

    }
    if(option == m_pLoadUserKeyBtn) {
        m_pConfigParser->addTags("key", value);
        if(!value.contains("N/A")) m_pLoadUserKeyBtn->setText(tr("ADDED"));
    }
}

void QuickSettingsTab::updateValues() {
    int index = m_pProtocolComboBox->findText(m_pConfigParser->getConfigValue("proto").toUpper());
    if (index != -1) { // -1 for not found
       m_pProtocolComboBox->setCurrentIndex(index);
    }
    QString remoteVal = m_pConfigParser->getConfigValue("remote");
    int portIndex = remoteVal.indexOf(" ");
    m_pRemoteHostEdit->setText(remoteVal.left(portIndex));
    m_pRemotePortSpinbox->setValue(remoteVal.mid(portIndex + 1,
                                                 remoteVal.length() - portIndex).toInt());

    m_pResolveTryEdit->setText(m_pConfigParser->getConfigValue("resolv-retry"));

    if(m_pConfigParser->isCaKeyActive("ca")) {
        m_pLoadServerCertBtn->setText(tr("ADDED"));
    }
    else {
        m_pLoadServerCertBtn->setText(tr("Browse..."));
    }
    if(m_pConfigParser->isCaKeyActive("cert")) {
        m_pLoadUserCertBtn->setText(tr("ADDED"));
    }
    else {
        m_pLoadUserCertBtn->setText(tr("Browse..."));
    }
    if(m_pConfigParser->isCaKeyActive("key")) {
        m_pLoadUserKeyBtn->setText(tr("ADDED"));
    }
    else {
        m_pLoadUserKeyBtn->setText(tr("Browse..."));
    }


}

void QuickSettingsTab::setConfig() {

    m_pConfigParser->addLine("proto " + m_pProtocolComboBox->currentText().toLower());
    if(!m_pRemoteHostEdit->text().isEmpty()) {
        m_pConfigParser->addLine("remote " + m_pRemoteHostEdit->text() + " " + m_pRemotePortSpinbox->text());
    }
    else {
        m_pConfigParser->removeLine("remote "); // space necessary to prevent conflict with "remote-"
    }
    if(!m_pResolveTryEdit->text().isEmpty()) {
        m_pConfigParser->addLine("resolv-retry " + m_pResolveTryEdit->text());
    }
    else {
        m_pConfigParser->removeLine("resolv-retry");
    }

}

void GeneralSettingsTab::createGeneralOptions()
{
    m_pGeneralSettingsLayout = new QGroupBox(tr("General Options"));
    QGridLayout *layout = new QGridLayout;

    m_pFloatBox = new QCheckBox(tr("Float"));
    connect(m_pFloatBox, SIGNAL(stateChanged(int)), this, SLOT(setConfig(int)));

    m_pCompLzoBox = new QCheckBox(tr("Compression LZO"));
    connect(m_pCompLzoBox, SIGNAL(stateChanged(int)), this, SLOT(setConfig(int)));

    m_pNoBindBox = new QCheckBox(tr("No bind"));
    connect(m_pNoBindBox, SIGNAL(stateChanged(int)), this, SLOT(setConfig(int)));

    m_pPersistKeyBox = new QCheckBox(tr("Persist key"));
    connect(m_pPersistKeyBox, SIGNAL(stateChanged(int)), this, SLOT(setConfig(int)));

    m_pPersistTunBox = new QCheckBox(tr("Persist tun"));
    connect(m_pPersistTunBox, SIGNAL(stateChanged(int)), this, SLOT(setConfig(int)));

    m_pAuthNoCacheBox = new QCheckBox(tr("Auth no-cache"));
    connect(m_pAuthNoCacheBox, SIGNAL(stateChanged(int)), this, SLOT(setConfig(int)));

    m_pAuthUserBox = new QCheckBox(tr("Auth user/pass"));
    connect(m_pAuthUserBox, SIGNAL(stateChanged(int)), this, SLOT(setConfig(int)));

    m_pRedirectGWBox = new QCheckBox(tr("Redirect gateway"));
    connect(m_pRedirectGWBox, SIGNAL(stateChanged(int)), this, SLOT(setConfig(int)));

    layout->addWidget(m_pFloatBox, 0, 0);
    layout->addWidget(m_pCompLzoBox, 0, 1);
    layout->addWidget(m_pNoBindBox, 0, 2);
    layout->addWidget(m_pPersistKeyBox, 0, 3);
    layout->addWidget(m_pPersistTunBox, 1, 0);
    layout->addWidget(m_pAuthNoCacheBox, 1, 1);
    layout->addWidget(m_pAuthUserBox, 1, 2);
    layout->addWidget(m_pRedirectGWBox, 1, 3);
    // Device Settings

    m_pDeviceLabel = new QLabel(tr("Device:"));
    m_pDeviceComboBox = new QComboBox;
    m_pDeviceComboBox->addItem("tap");
    m_pDeviceComboBox->addItem("tun");
    m_pDeviceComboBox->setCurrentIndex(1);
    connect(m_pDeviceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setConfig()));

    m_pDeviceMTULabel = new QLabel(tr("Device MTU:"));
    m_pDeviceMTUEdit = new QLineEdit;
    connect(m_pDeviceMTUEdit, SIGNAL(editingFinished()), this, SLOT(setConfig()));

    QGroupBox *m_pDeviceSettingsGroup = new QGroupBox(tr("Device Settings"));
    QFormLayout *deviceFormLayout = new QFormLayout;
    deviceFormLayout->addRow(m_pDeviceLabel, m_pDeviceComboBox);
    deviceFormLayout->addRow(m_pDeviceMTULabel, m_pDeviceMTUEdit);
    m_pDeviceSettingsGroup->setLayout(deviceFormLayout);
    layout->addWidget(m_pDeviceSettingsGroup, 2, 0, 1, 4);

    // User downgrade

    QGroupBox *pUserDowngradeGroup = new QGroupBox(tr("Downgrade user privileges to:"));
    QFormLayout *pUserFormLayout = new QFormLayout;

    m_pUserLabel = new QLabel(tr("User:"));
    m_pUserEdit = new QLineEdit();
    connect(m_pUserEdit, SIGNAL(editingFinished()), this, SLOT(setConfig()));

    m_pGroupLabel = new QLabel(tr("Group:"));
    m_pGroupEdit = new QLineEdit();
    connect(m_pGroupEdit, SIGNAL(editingFinished()), this, SLOT(setConfig()));

    pUserFormLayout->addRow(m_pUserLabel, m_pUserEdit);
    pUserFormLayout->addRow(m_pGroupLabel, m_pGroupEdit);
    pUserDowngradeGroup->setLayout(pUserFormLayout);
    layout->addWidget(pUserDowngradeGroup, 3, 0, 1, 4);

    // Other

    QGroupBox *pOtherGroup = new QGroupBox(tr("Other settings:"));
    QFormLayout *pOtherFormLayout = new QFormLayout;

    m_pRouteDelayLabel = new QLabel(tr("Route delay:"));
    m_pRouteDelayEdit = new QLineEdit();
    connect(m_pRouteDelayEdit, SIGNAL(editingFinished()), this, SLOT(setConfig()));

    m_pVerbosityLevelLabel = new QLabel(tr("Log verbosity:"));
    m_pVerbosityLevelSpinBox = new QSpinBox();
    m_pVerbosityLevelSpinBox->setRange(0, 11);
    m_pVerbosityLevelSpinBox->setValue(3);
    connect(m_pVerbosityLevelSpinBox, SIGNAL(editingFinished()), this, SLOT(setConfig()));


    m_pMuteLabel = new QLabel(tr("Mute log:"));
    m_pMuteEdit = new QLineEdit();
    connect(m_pMuteEdit, SIGNAL(editingFinished()), this, SLOT(setConfig()));

    m_pMuteReplayWarningsCheckBox = new QCheckBox(tr("Mute replay warnings"));
    connect(m_pMuteReplayWarningsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setConfig()));

    m_pRemoteRandomCheckBox = new QCheckBox(tr("Connect servers randomly"));
    connect(m_pRemoteRandomCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setConfig()));

    m_pNsCertTypeLabel = new QLabel(tr("Certificate Type:"));
    m_pNsCertTypeComboBox = new QComboBox();
    m_pNsCertTypeComboBox->addItem(tr("server"));
    m_pNsCertTypeComboBox->addItem(tr("client"));
    connect(m_pNsCertTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setConfig()));


    pOtherFormLayout->addRow(m_pRouteDelayLabel, m_pRouteDelayEdit);
    pOtherFormLayout->addRow(m_pVerbosityLevelLabel, m_pVerbosityLevelSpinBox);
    pOtherFormLayout->addRow(m_pMuteLabel, m_pMuteEdit);
    pOtherFormLayout->addRow(m_pMuteReplayWarningsCheckBox, m_pRemoteRandomCheckBox);
    pOtherFormLayout->addRow(m_pNsCertTypeLabel, m_pNsCertTypeComboBox);

    pOtherGroup->setLayout(pOtherFormLayout);
    layout->addWidget(pOtherGroup, 4, 0, 1, 4);

    m_pGeneralSettingsLayout->setLayout(layout);
}

void GeneralSettingsTab::setConfig() {
    m_pConfigParser->addLine("dev " + m_pDeviceComboBox->currentText().toLower());

    if(!m_pDeviceMTUEdit->text().isEmpty()) {
        m_pConfigParser->addLine("tun-mtu " + m_pDeviceMTUEdit->text());
    }
    else {
        m_pConfigParser->removeLine("tun-mtu");
    }
    if(!m_pUserEdit->text().isEmpty()) {
        m_pConfigParser->addLine("user " + m_pUserEdit->text());
    }
    else {
        m_pConfigParser->removeLine("user");
    }
    if(!m_pGroupEdit->text().isEmpty()) {
        m_pConfigParser->addLine("group " + m_pGroupEdit->text());
    }
    else {
        m_pConfigParser->removeLine("group");
    }

    if(!m_pRouteDelayEdit->text().isEmpty()) {
        m_pConfigParser->addLine("route-delay " + m_pRouteDelayEdit->text());
    }
    else {
        m_pConfigParser->removeLine("route-delay");
    }
    if(!m_pVerbosityLevelSpinBox->text().isEmpty()) {
        m_pConfigParser->addLine("verb " + m_pVerbosityLevelSpinBox->text());
    }
    else {
        m_pConfigParser->removeLine("verb");
    }
    if(!m_pMuteEdit->text().isEmpty()) {
        m_pConfigParser->addLine("mute " + m_pMuteEdit->text());
    }
    else {
        m_pConfigParser->removeLine("mute");
    }
    if(m_pMuteReplayWarningsCheckBox->checkState() == Qt::Checked) {
        m_pConfigParser->addLine("mute-replay-warnings");
    }
    else {
        m_pConfigParser->removeLine("mute-replay-warnings");
    }
    if(m_pRemoteRandomCheckBox->checkState() == Qt::Checked) {
        m_pConfigParser->addLine("remote-random");
    }
    else {
        m_pConfigParser->removeLine("remote-random");
    }
    m_pConfigParser->addLine("ns-cert-type " + m_pNsCertTypeComboBox->currentText().toLower());

}

void GeneralSettingsTab::setConfig(int _state) {
    QObject* option = sender();

    if(option == m_pFloatBox) {
            if(_state) {
                m_pConfigParser->addLine("float");
            }
            else {
                m_pConfigParser->removeLine("float");
            }
    }
    if(option == m_pCompLzoBox) {
            if(_state) {
                m_pConfigParser->addLine("comp-lzo");
            }
            else {
                m_pConfigParser->removeLine("comp-lzo");
            }
    }
    if(option == m_pNoBindBox) {
            if(_state) {
                m_pConfigParser->addLine("nobind");
            }
            else {
                m_pConfigParser->removeLine("nobind");
            }
    }
    if(option == m_pPersistKeyBox) {
            if(_state) {
                m_pConfigParser->addLine("persist-key");
            }
            else {
                m_pConfigParser->removeLine("persist-key");
            }
    }
    if(option == m_pPersistTunBox) {
            if(_state) {
                m_pConfigParser->addLine("persist-tun");
            }
            else {
                m_pConfigParser->removeLine("persist-tun");
            }
    }
    if(option == m_pAuthNoCacheBox) {
            if(_state) {
                m_pConfigParser->addLine("auth-nocache");
            }
            else {
                m_pConfigParser->removeLine("auth-nocache");
            }
    }
    if(option == m_pAuthUserBox) {
            if(_state) {
                m_pConfigParser->addLine("auth-user-pass");
            }
            else {
                m_pConfigParser->removeLine("auth-user-pass");
            }
    }
    if(option == m_pRedirectGWBox) {
            if(_state) {
                m_pConfigParser->addLine("redirect-gateway");
            }
            else {
                m_pConfigParser->removeLine("redirect-gateway");
            }
    }
    if(option == m_pMuteReplayWarningsCheckBox) {
            if(_state) {
                m_pConfigParser->addLine("mute-replay-warnings");
            }
            else {
                m_pConfigParser->removeLine("mute-replay-warnings");
            }
    }
    if(option == m_pRemoteRandomCheckBox) {
            if(_state) {
                m_pConfigParser->addLine("remote-random");
            }
            else {
                m_pConfigParser->removeLine("remote-random");
            }
    }

}

void GeneralSettingsTab::updateValues() {

    m_pFloatBox->setChecked(m_pConfigParser->isConfigActive("float"));
    m_pCompLzoBox->setChecked(m_pConfigParser->isConfigActive("comp-lzo"));
    m_pNoBindBox->setChecked(m_pConfigParser->isConfigActive("nobind"));
    m_pPersistKeyBox->setChecked(m_pConfigParser->isConfigActive("persist-key"));
    m_pPersistTunBox->setChecked(m_pConfigParser->isConfigActive("persist-tun"));
    m_pAuthNoCacheBox->setChecked(m_pConfigParser->isConfigActive("auth-nocache"));
    m_pAuthUserBox->setChecked(m_pConfigParser->isConfigActive("auth-user-pass"));
    m_pRedirectGWBox->setChecked(m_pConfigParser->isConfigActive("redirect-gateway"));

    int index = m_pDeviceComboBox->findText(m_pConfigParser->getConfigValue("dev"));
    if (index != -1) { // -1 for not found
       m_pDeviceComboBox->setCurrentIndex(index);
    }

    m_pDeviceMTUEdit->setText(m_pConfigParser->getConfigValue("tun-mtu"));

    m_pUserEdit->setText(m_pConfigParser->getConfigValue("user"));
    m_pGroupEdit->setText(m_pConfigParser->getConfigValue("group"));


    m_pRouteDelayEdit->setText(m_pConfigParser->getConfigValue("route-delay"));

    m_pVerbosityLevelSpinBox->setValue(m_pConfigParser->getConfigValue("verb").toInt());

    m_pMuteEdit->setText(m_pConfigParser->getConfigValue("mute"));

    m_pMuteReplayWarningsCheckBox->setChecked(m_pConfigParser->isConfigActive("mute-replay-warnings"));

    m_pRemoteRandomCheckBox->setChecked(m_pConfigParser->isConfigActive("remote-random"));

    index = m_pNsCertTypeComboBox->findText(m_pConfigParser->getConfigValue("ns-cert-type"));
    if (index != -1) { // -1 for not found
       m_pNsCertTypeComboBox->setCurrentIndex(index);
    }

}

void ManualEditTab::createManualEditOptions() {
    m_pManualSettingsLayout = new QGroupBox(tr("Manual Configuration"));
    QFormLayout *manualEdit = new QFormLayout;

    m_pConfigEditLabel = new QLabel(tr("Loaded config file:"));
    m_pConfigEdit = new QTextEdit;

    m_pApplyConfigButton = new QPushButton(tr("Apply Config"));
    connect(m_pApplyConfigButton, SIGNAL(released()), m_pConfigParser, SLOT(updateManual()));

    m_pApplyHint = new QLabel(tr("If you changed configuration manually, apply using the following button:"));


    manualEdit->addRow(m_pConfigEditLabel, m_pConfigEdit);
    manualEdit->addRow(m_pApplyHint);
    manualEdit->addRow(m_pApplyConfigButton);

    m_pManualSettingsLayout->setLayout(manualEdit);
}

void ManualEditTab::setConfigEdit(QString _value) {
    m_pConfigEdit->setText(_value);
}


void ManualEditTab::refreshFileContents() {
    m_pConfigParser->setFileContents(m_pConfigEdit->toPlainText());
}


void ManualEditTab::updateValues() {
    setConfigEdit(m_pConfigParser->getFileContents());
}
