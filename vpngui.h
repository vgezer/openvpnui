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


#ifndef VPNGUI
#define VPNGUI


#include <QDialog>

class ConfigParser;

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QTabWidget;
class QAction;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QMenuBar;
class QPushButton;
class QTextEdit;
class QCheckBox;
class QComboBox;
class QSpinBox;
QT_END_NAMESPACE

class VPNGui : public QDialog
{
    Q_OBJECT

public:
    explicit VPNGui(ConfigParser *_configParser, QWidget *parent = 0);
    void createMenu(ConfigParser *_configParser);

public slots:
    void exit();
    void showAboutDlg();

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void reject();

private:
    QTabWidget *tabWidget;

    enum { NumGridRows = 3, NumButtons = 4 };

    QMenuBar *menuBar;
    QGroupBox *horizontalGroupBox;
    QDialogButtonBox *buttonBox;


    QMenu *fileMenu;
    QAction *newConfigAction;
    QAction *openConfigAction;
    QAction *createDefaultConfigAction;
    QAction *saveCreatedConfigAction;
    QAction *exitAction;

    QMenu *configMenu;
    QAction *updateConfigAction;

    QMenu *helpMenu;
    QAction *aboutAction;
};

class QuickSettingsTab : public QWidget
{
    Q_OBJECT

public:
    explicit QuickSettingsTab(ConfigParser *_configParser, QWidget *parent = 0);
    QString readContents();

public slots:
    void setConfig();
    void updateValues();
    void addCertKey();

private:
    void createQuickOptions();
    QGroupBox *m_pQuickSettingsLayout;

    QLabel *m_pProfileLabel;
    QLineEdit *m_pProfileEdit;
    ConfigParser *m_pConfigParser;

    // general options

    QLabel *m_pRemoteHostLabel;
    QLineEdit *m_pRemoteHostEdit;

    QLabel *m_pRemotePortLabel;
    QSpinBox *m_pRemotePortSpinbox;

    QLabel *m_pProtocolLabel;
    QComboBox *m_pProtocolComboBox;

    QLabel *m_pResolveTryLabel;
    QLineEdit *m_pResolveTryEdit;

    QLabel *m_pLoadServerCertLabel;
    QPushButton *m_pLoadServerCertBtn;

    QLabel *m_pLoadUserCertLabel;
    QPushButton *m_pLoadUserCertBtn;

    QLabel *m_pLoadUserKeyLabel;
    QPushButton *m_pLoadUserKeyBtn;


};

class GeneralSettingsTab : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralSettingsTab(ConfigParser *_configParser, QWidget *parent = 0);


public slots:
    void updateValues();
    void setConfig(int);
    void setConfig();

private:
    void createGeneralOptions();
    // general options
    ConfigParser *m_pConfigParser;
    QGroupBox *m_pGeneralSettingsLayout;
    QCheckBox *m_pCompLzoBox;
    QCheckBox *m_pFloatBox;
    QCheckBox *m_pPersistKeyBox;
    QCheckBox *m_pPersistTunBox;
    QCheckBox *m_pNoBindBox;
    QCheckBox *m_pAuthNoCacheBox;
    QCheckBox *m_pAuthUserBox;
    QCheckBox *m_pNoCacheBox;
    QCheckBox *m_pRedirectGWBox;

    // downgrade rights
    QLabel *m_pUserLabel;
    QLineEdit *m_pUserEdit;
    QLabel *m_pGroupLabel;
    QLineEdit *m_pGroupEdit;

    // Other settings

    QLabel *m_pVerbosityLevelLabel;
    QSpinBox *m_pVerbosityLevelSpinBox;
    QLabel *m_pNsCertTypeLabel;
    QComboBox *m_pNsCertTypeComboBox;
    QCheckBox *m_pMuteReplayWarningsCheckBox;
    QCheckBox *m_pRemoteRandomCheckBox;

    QLabel *m_pRouteDelayLabel;
    QLineEdit *m_pRouteDelayEdit;

    QLabel *m_pMuteLabel;
    QLineEdit *m_pMuteEdit;

    QLabel *m_pDeviceLabel;
    QComboBox *m_pDeviceComboBox;
    QLabel *m_pDeviceMTULabel;
    QLineEdit *m_pDeviceMTUEdit;
};

class ManualEditTab : public QWidget
{
    Q_OBJECT

public:
    explicit ManualEditTab(ConfigParser *_configParser, QWidget *parent = 0);
    void setConfigEdit(QString _value);

public slots:
    void updateValues();
    void refreshFileContents();

private:
    void createManualEditOptions();
    ConfigParser *m_pConfigParser;
    QGroupBox *m_pManualSettingsLayout;

    QLabel *m_pConfigEditLabel;
    QTextEdit * m_pConfigEdit;
    QLabel *m_pApplyHint;
    QPushButton *m_pApplyConfigButton;
};

#endif // VPNGUI_H
