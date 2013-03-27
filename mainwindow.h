/*
 * the main window header
 * $Id$
 * (part of FmStick).
 *
 * (C) 2011 Serge <piratfm@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QMouseEvent>
#include <QtNetwork>

#include <commanddefs.h>
#include <propertydefs.h>
#include "querier.h"
#include "httplistener.h"

namespace Ui {
    class MainWindow;
}

enum {
    ICON_NOTX,
    ICON_TX,
    ICON_WARN,
    ICON_ERR,
    ICON_NODEV
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void HideIfNeeded();
    void readFromConfig();

    Ui::MainWindow *ui;

    QAction *quitAction, *trayNewRtAction;
    QMenu *trayIconMenu, *languageMenu;
    QActionGroup *languageActionGroup;
    QSystemTrayIcon *trayIcon;
    QDialog* rt_popup;
    QPlainTextEdit *rt_editor;
    QString http_admin_login, http_admin_pwd;
    bool deviceStatus;
    QString rtplus_text;
    QString CurrentVersion;


    void DisplayAudioEditor(void);
    void ConfigureAudioEditor(void);
    void DisplayTransmitterEditor(void);
    void ConfigureTransmitterEditor(void);
    void DisplayRDSEditor(void);
    void ConfigureRDSEditor(void);
    void DisplayFirmwareInfo(void);

    ~MainWindow();

public slots:
    bool GetRunAtStartup();
    void SetRunAtStartup(int iVal);
    void GotMsgFromAnotherInstance(const QString&);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void toggleToolBar();
    void ShowAboutApp();
    void goToHomePage();
    void quit();
    void resetFmTxSettings();
    void toggleWindowMode();
    void setAudioMeasuredLvl(int iVal);
    void DisplayRadioTextDialog();
    void SaveRadioTextDialog();
    void switchLanguage(QAction *action);

    /* software section */
    void toggleLiveEditing(bool bVal);
    void SetConnectionState();
    void toggleConnection(bool bVal, bool toggleQuerier);
    void RedrawEverything(bool);

    /* hardware section */
    void SetFrontendPowerDown(void);
    int SetFrontendPowerUp(void);
    void SetFrontendReset(void);
    void SetRunEepromCfg(bool bVal);
    void UpdateFirmware();

    /**************************************
     * Slots for Enabled/Disabled logic
     **************************************/
    /* audio section */
    void toggleCompressor(int value);
    void toggleLimiter(int value);
    void toggleASQLow(int value);
    void toggleASQHigh(int value);

    /* transmitter section */
    void toggleTransmitRds(int value);
    void toggleTransmitPilot(int value);
    void toggleTransmitEnabled(int value);
    void toggleTransmitAntCap(int value);
    void setAntCapNoEvent(double dVal);

    /* RDS Section */
    void toggleRDSaf(int value);
    void toggleRDSfifo(int value);
    void toggleRDStime(int value);
    void toggleRDSPSNum(int value);
    void toggleRDSPSMsg(QString msg);
    void toggleStationId();
    void toggleRDSRT();
    void UpdateRDSRTFileEnabled(int enabled);
    void UpdateRDSRTFile();
    void UpdateRDSRTPlusEnabled();
    void updateRDSRTPlusMark(QList<int> &itemsList, int retry_id);
    void updateRDSRTview(QString text);
    void SetRDSCustomGroup(QString *text=NULL);


    /**************************************
     * Slots for Live Editing logic
     **************************************/
    /* when live editing enabled */
    void SetTransmitComponentFlags();
    void SetTransmitPreemphasis(int iVal);
    void SetTransmitAudioDeviation(int iVal);
    void SetTransmitPilotDeviation(int iVal);
    void SetTransmitPilotFrequency(int iVal);
    void SetTransmitRDSDeviation(int iVal);
    void SetTransmitEnabled( bool bEnabled );
    void SetTransmitIcon( int iVal );
    int  SetTransmitPower();
    void SetTransmitFrequency(double dVal);

    void SetRDSPI();
    void SetRDSPsMiscFlags();
    void SetRDSAF(double dVal);
    void SetRDSFIFO(int iVal);
    void SetRDSTime(int iVal);
    void SetRDSPSPercentage(int iVal);
    void SetRDSPSRepeats(int iVal);
    void SetRDSPSNum(int iVal);
    void DelayedSendPS();
    int SetRDSPSMessage(int id);
    int SetRDSRT(QString *text_in);

    void SetAudioCompLimByPreset(int iVal);
    bool SetAudioCompressorThreshold(int iVal);
    bool SetAudioCompressorGain(int iVal);
    bool SetAudioCompressorReleaseTime(int iVal);
    bool SetAudioCompressorAttackTime(double dVal);
    bool SetAudioLimiterReleaseTime(double dVal);
    bool SetAudioASQFlags();
    bool SetAudioASQLevelLow(int iVal);
    bool SetAudioASQDurationLow(int iVal);
    bool SetAudioASQLevelHigh(int iVal);
    bool SetAudioASQDurationHigh(int iVal);
    bool SetAudioStandAlone(int iVal=-1);
    bool SetAudioStandAloneMute(int val);

    /* actions */
    void StationButtonClicked();
    void StationButtonContextMenu(const QPoint &);
    /* device manipulations */
    int  GetFrontendParams(void);
    int  SetFrontendParams(void);
    /* file manipulations */
    void SaveFrontendParams();
    void ReadFrontendParams();
    /* EEPROM manipulations */
    void SaveEEPROMParams();
    void ShowEEPROMParams();
    /* digital audio hack (needed after fmtx changed)*/
    bool SetDigitalAudioHack();

    /* software events */
    bool checkHTTPListenerPort();
    void updateHTTPListener();
    void updateHTTPListenerAccess();
    bool updateItemFromHTTPListener(QObject *obj, QString *value);
    bool openBrowser(const QString& rUrl, QWidget* parent=NULL);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void hideEvent(QHideEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    int liveEditingEnabled;
    int audioLimiter_prev_div;
    bool toolBarIsVisible;

    Querier  *querier; // this thread for sending rds timestamps and querying interrupts
    HttpListener* listener;
    QSettings settings;
    QString UsedLanguage;
    QTranslator qtTranslator;
    QTranslator appTranslator;

    QIcon erroricon;
    QIcon warningicon;
    QIcon txicon;
    QIcon notxicon;
    QIcon nodeviceicon;

    QLineEdit *GetRDSLineEditById(int id);
    bool SetAudioCompLimFlags();
    void SetAudioCompLimPresetByParams();

    bool is_normal; /* mini mode or normal window mode*/
    bool acceptClose;
    void SetWindowMode(bool normal);
    QPoint offset;
    bool moving;

    QDir directoryOf(const QString &subdir);
    void createLanguageMenu();
    void retranslateUi();
    void readSettings(QSettings *reader, bool txOnly);
    void writeSettings(QSettings *writer, bool txOnly);
    void setupTrayIcon();
    void toggleRDSPSVisibility(int value);
};

#endif // MAINWINDOW_H
