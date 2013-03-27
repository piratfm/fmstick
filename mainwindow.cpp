/*
 * most processing functions are here
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

#include <cstdlib>
#include <iostream>
#include <QtCore/QString>
#include <QtGui/QApplication>
#include <QMessageBox>
#include <QStatusBar>
#include <QProgressBar>
#include <QIcon>

#include "mainwindow.h"
#include "querier.h"
#include "ui_mainwindow.h"

#include "fmtx.h"
#include "logging.h"

#define REVISION   "$Rev: 268 $"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int i;
    QLineEdit *curr_msg = NULL;

    liveEditingEnabled=false;
    is_normal = true;
    moving = false;
    acceptClose = false;
    deviceStatus = false;
    toolBarIsVisible = false;
    listener=NULL;
    fmtxCmdSpecial = FMTX_SPECIAL_FRONTEND; ///< At the startup point we want to work with frontend
    fmtxEEPROMInfoClean();

    erroricon = QIcon(":/fmstick/images/Gnome-Network-Wireless-NoTxError-64.png");
    warningicon = QIcon(":/fmstick/images/Gnome-Network-Wireless-NoTxWarning-64.png");
    notxicon = QIcon(":/fmstick/images/Gnome-Network-Wireless-NoTx-64.png");
    txicon = QIcon(":/fmstick/images/fmstick.png");
    nodeviceicon = QIcon(":/fmstick/images/Gnome-Network-Wireless-NoDevice-64.png");

    CurrentVersion = REVISION;
    CurrentVersion.replace(QRegExp("^\\$Rev: (\\d+) \\$$"), "\\1");
    CurrentVersion =  QString(VERSION) + "." + CurrentVersion;

    ui->setupUi(this);

    ui->lineEdit_sw_ver->setText(CurrentVersion);
    ui->checkBox_sw_runAtStartup->setChecked(GetRunAtStartup());
    audioLimiter_prev_div = 0;
    SetAudioCompLimByPreset(4);

    /* actions */
    connect( ui->actionOpen, SIGNAL(triggered()),
             this, SLOT(ReadFrontendParams()) );
    connect( ui->actionSave, SIGNAL(triggered()),
             this, SLOT(SaveFrontendParams()) );

    connect( ui->actionReset_to_Default, SIGNAL(triggered()),
             this, SLOT(resetFmTxSettings()) );
    connect( ui->actionRead_from_Device, SIGNAL(triggered()),
             this, SLOT(GetFrontendParams()) );
    connect( ui->actionWrite_to_Device, SIGNAL(triggered()),
             this, SLOT(SetFrontendParams()) );
    connect( ui->actionRead_from_EEPROM, SIGNAL(triggered()),
             this, SLOT(ShowEEPROMParams()) );
    connect( ui->actionWrite_to_EEPROM, SIGNAL(triggered()),
             this, SLOT(SaveEEPROMParams()) );

    connect( ui->actionSkin_changeMode, SIGNAL(triggered()),
             this, SLOT(toggleWindowMode()) );
    connect( ui->actionToggle_ToolBar, SIGNAL(triggered()),
             this, SLOT(toggleToolBar()));
    connect( ui->actionChange_RadioText, SIGNAL(triggered()),
             this, SLOT(DisplayRadioTextDialog()) );

    connect( ui->actionExit, SIGNAL(triggered()),
             this, SLOT(quit()) );
    connect( ui->actionToggle_transmission, SIGNAL(triggered()),
             this, SLOT(StationButtonClicked()) );
    connect( ui->actionConnect_to_device, SIGNAL(triggered()),
             this, SLOT(SetConnectionState()));
    connect(ui->actionAbout, SIGNAL(triggered()),
            this, SLOT(ShowAboutApp()));

    /* audio compressor/limiter */
    connect( ui->comboBox_audioPreset, SIGNAL(currentIndexChanged(int)),
             this, SLOT(SetAudioCompLimByPreset(int)) );
    connect( ui->checkBox_audio_enablecompressor, SIGNAL( stateChanged(int) ),
             this, SLOT( toggleCompressor(int) ) );
    connect( ui->spinBox_audiocompressorThreshold, SIGNAL( valueChanged(int) ),
            this, SLOT( SetAudioCompressorThreshold(int) ) );
    connect( ui->spinBox_audioCompressorGain, SIGNAL(valueChanged(int)),
             this, SLOT(SetAudioCompressorGain(int)));
    connect( ui->comboBox_audioCompressorReleasetime, SIGNAL(currentIndexChanged(int)),
             this, SLOT(SetAudioCompressorReleaseTime(int)));
    connect( ui->doubleSpinBox_audioCompressorAttackTime, SIGNAL(valueChanged(double)),
             this, SLOT(SetAudioCompressorAttackTime(double)));
    connect( ui->checkBox_audioLimiterEnable , SIGNAL( stateChanged(int) ),
             this, SLOT( toggleLimiter(int) ) );
    connect( ui->doubleSpinBox_audio_lim_release_time, SIGNAL(valueChanged(double)),
             this, SLOT(SetAudioLimiterReleaseTime(double)));
    connect( ui->comboBox_audio_input_level, SIGNAL(currentIndexChanged(int)),
             this, SLOT(SetAudioStandAlone(int)));
    connect( ui->spinBox_audio_input_amplitude, SIGNAL(valueChanged(int)),
             this, SLOT(SetAudioStandAlone()));
    connect( ui->comboBox_audio_input_mute, SIGNAL(currentIndexChanged(int)),
             this, SLOT(SetAudioStandAloneMute(int)));


    /* audio signal quality measurement */
    connect( ui->checkBox_asq_overmoddetect, SIGNAL( stateChanged(int) ),
             this, SLOT(SetAudioASQFlags()));
    connect( ui->checkBox_asq_detectLow , SIGNAL( stateChanged(int) ),
             this, SLOT( toggleASQLow(int) ) );
    connect( ui->spinBox_asq_detectLowThreshold, SIGNAL(valueChanged(int)),
             this, SLOT(SetAudioASQLevelLow(int)));
    connect( ui->spinBox_asq_detectLowDuration, SIGNAL(valueChanged(int)),
             this, SLOT(SetAudioASQDurationLow(int)));
    connect( ui->checkBox_asq_detectHigh , SIGNAL( stateChanged(int) ),
             this, SLOT( toggleASQHigh(int) ) );
    connect( ui->spinBox_asq_detectHighThreshold, SIGNAL(valueChanged(int)),
             this, SLOT(SetAudioASQLevelHigh(int)));
    connect( ui->spinBox_asq_detectHighDuration, SIGNAL(valueChanged(int)),
             this, SLOT(SetAudioASQDurationHigh(int)));

    /* transmitter configuration */
    //checkBox_transmitter_pilot: spinBox_transmitter_pilot_devi, spinBox_transmitter_pilot_freq, checkBox_transmitter_lmr
    //checkBox_transmitter_rds: spinBox_transmitter_rds_devi
    //checkBox_transmitter_enabled: doubleSpinBox_transmitter_freq spinBox_transmitter_power ==> txcap
    // \==> checkBox_transmitter_antcap: doubleSpinBox_transmitter_antcap_val
    connect( ui->checkBox_transmitter_pilot , SIGNAL( stateChanged(int) ),
             this, SLOT( toggleTransmitPilot(int) ) );
    connect( ui->checkBox_transmitter_rds , SIGNAL( stateChanged(int) ),
             this, SLOT( toggleTransmitRds(int) ) );
    connect( ui->checkBox_transmitter_lmr , SIGNAL( stateChanged(int) ),
             this, SLOT( SetTransmitComponentFlags() ) );

    connect( ui->checkBox_transmitter_enabled , SIGNAL( stateChanged(int) ),
             this, SLOT( toggleTransmitEnabled(int) ) );
    connect( ui->checkBox_transmitter_antcap , SIGNAL( stateChanged(int) ),
             this, SLOT( toggleTransmitAntCap(int) ) );
    connect( ui->comboBox_transmitter_preemph, SIGNAL(currentIndexChanged(int)),
             this, SLOT(SetTransmitPreemphasis(int)));
    connect( ui->spinBox_transmitter_audioDev, SIGNAL(valueChanged(int)),
             this, SLOT(SetTransmitAudioDeviation(int)));

    connect( ui->spinBox_transmitter_pilot_devi, SIGNAL(valueChanged(int)),
             this, SLOT( SetTransmitPilotDeviation(int) ) );
    connect( ui->spinBox_transmitter_pilot_freq, SIGNAL(valueChanged(int)),
             this, SLOT( SetTransmitPilotFrequency(int) ) );
    connect( ui->spinBox_transmitter_rds_devi, SIGNAL(valueChanged(int)),
             this, SLOT( SetTransmitRDSDeviation(int) ) );
    connect( ui->doubleSpinBox_transmitter_freq, SIGNAL(valueChanged(double)),
             this, SLOT( SetTransmitFrequency(double) ) );
    connect( ui->spinBox_transmitter_power, SIGNAL(valueChanged(int)),
             this, SLOT( SetTransmitPower() ) );
    connect( ui->doubleSpinBox_transmitter_antcap_val, SIGNAL(valueChanged(double)),
             this, SLOT( SetTransmitPower() ) );

    /* RDS Configuration */
    //checkBox_rds_useaf: doubleSpinBox_rds_af
    //checkBox_rds_fifo: spinBox_rds_fifo_sz
    // \==> checkBox_rds_time: spinBox_rds_time_int
    connect( ui->lineEdit_rds_progid, SIGNAL( editingFinished()),
             this, SLOT( SetRDSPI() ));
    connect( ui->checkBox_rds_useaf , SIGNAL( stateChanged(int) ),
             this, SLOT( toggleRDSaf(int) ) );
    connect( ui->doubleSpinBox_rds_af, SIGNAL(valueChanged(double)),
             this, SLOT( SetRDSAF(double) ));
    connect( ui->checkBox_rds_fifo , SIGNAL( stateChanged(int) ),
             this, SLOT( toggleRDSfifo(int) ) );
    connect( ui->spinBox_rds_fifo_sz , SIGNAL(valueChanged(int)),
             this, SLOT( SetRDSFIFO(int) ) );
    connect( ui->spinBox_rds_ps_msgs , SIGNAL( valueChanged(int) ),
             this, SLOT( toggleRDSPSNum(int) ) );
    connect( ui->spinBox_rds_ps_repeats , SIGNAL( valueChanged(int) ),
             this, SLOT( SetRDSPSRepeats(int) ) );
    connect( ui->comboBox_rds_ps_percentage, SIGNAL(currentIndexChanged(int)),
             this, SLOT(SetRDSPSPercentage(int)));

    /* this boths PS msg editors must not re-block each other */
    connect( ui->lineEdit_station_id, SIGNAL(textChanged(QString)), this, SLOT( toggleRDSPSMsg(QString)) );
    for(i=0; i<12; i++){
        curr_msg = GetRDSLineEditById(i);
        //if(curr_msg) connect( curr_msg, SIGNAL(editingFinished()), this, SLOT(toggleStationId()) );
        //if(curr_msg) connect( curr_msg, SIGNAL(textEdited(QString)), this, SLOT(toggleStationId()) );
        if(curr_msg) connect( curr_msg, SIGNAL(textChanged(QString)), this, SLOT(toggleStationId()) );
    }

    /* RadioText */
    connect( ui->plainTextEdit_rds_rt, SIGNAL(textChanged()),
             this, SLOT(toggleRDSRT()));

    /* misc flags */
    connect( ui->checkBox_rds_art_head, SIGNAL(stateChanged(int)),
             this, SLOT(SetRDSPsMiscFlags()) );
    connect( ui->checkBox_rds_compr_code, SIGNAL(stateChanged(int)),
             this, SLOT(SetRDSPsMiscFlags()) );
    connect( ui->checkBox_rds_dynPT, SIGNAL(stateChanged(int)),
             this, SLOT(SetRDSPsMiscFlags()) );
    connect( ui->checkBox_rds_forceb, SIGNAL(stateChanged(int)),
             this, SLOT(SetRDSPsMiscFlags()) );
    connect( ui->checkBox_rds_is_stereo, SIGNAL(stateChanged(int)),
             this, SLOT(SetRDSPsMiscFlags()) );
    connect( ui->checkBox_rds_ta, SIGNAL(stateChanged(int)),
             this, SLOT(SetRDSPsMiscFlags()) );
    connect( ui->checkBox_rds_ta_tp, SIGNAL(stateChanged(int)),
             this, SLOT(SetRDSPsMiscFlags()) );
    connect( ui->comboBox_rds_pty, SIGNAL(currentIndexChanged(int)),
             this, SLOT(SetRDSPsMiscFlags()) );


    /* StationId -> PS msg editor */
    //connect( ui->lineEdit_station_id, SIGNAL(textEdited(QString)),
    //         this, SLOT( toggleRDSPSMsg(QString)) );

    /* BigButton actions */
    connect( ui->commandLinkButton_station, SIGNAL(clicked()),
             this, SLOT(StationButtonClicked()) );
    connect( ui->commandLinkButton_station, SIGNAL(customContextMenuRequested(const QPoint &)),
             this, SLOT(StationButtonContextMenu(const QPoint &)) );

    /* hardware section */
    connect(ui->pushButton_fe_powerdown, SIGNAL(clicked()),
            this, SLOT(SetFrontendPowerDown()));
    connect(ui->pushButton_fe_powerup, SIGNAL(clicked()),
            this, SLOT(SetFrontendPowerUp()));
    connect(ui->pushButton_fe_reset, SIGNAL(clicked()),
            this, SLOT(SetFrontendReset()));
    connect( ui->pushButton_open_firmware, SIGNAL(clicked()),
             this, SLOT(UpdateFirmware()));
    connect( ui->checkBox_hw_run_txAtPowerUp , SIGNAL( toggled(bool) ),
             this, SLOT( SetRunEepromCfg(bool) ) );

    /* software configuration */
    connect( ui->checkBox_rds_time , SIGNAL( stateChanged(int) ),
             this, SLOT( toggleRDStime(int) ) );
    connect( ui->spinBox_rds_time_int , SIGNAL( valueChanged(int) ),
             this, SLOT( SetRDSTime(int) ) );
    connect( ui->checkBox_sw_runAtStartup, SIGNAL(stateChanged(int)),
             this, SLOT(SetRunAtStartup(int)) );
    connect( ui->checkBox_sw_live_edit, SIGNAL(toggled(bool)),
             this, SLOT(toggleLiveEditing(bool)));
    connect( ui->pushButton_sw_visitWebsite, SIGNAL(clicked()),
             this, SLOT(goToHomePage()));
    connect( ui->checkBox_sw_remote, SIGNAL(stateChanged(int)),
             this, SLOT(updateHTTPListener()));
    connect( ui->spinBox_sw_remote_port, SIGNAL(valueChanged(int)),
             this, SLOT(updateHTTPListener()));
    connect( ui->lineEdit_sw_remote_login, SIGNAL(editingFinished()),
             this, SLOT(updateHTTPListenerAccess()));
    connect( ui->lineEdit_sw_remote_pwd, SIGNAL(editingFinished()),
             this, SLOT(updateHTTPListenerAccess()));
//    connect( ui->textBrowser_sw_remote_uri, SIGNAL( highlighted( const QString&) ),
//             this, SLOT( openBrowser(const QString&)) );
    connect( ui->pushButton_sw_send_custom_rds, SIGNAL(clicked()),
             this, SLOT(SetRDSCustomGroup()));

#ifdef QT_NO_DEBUG
    loglevel=LOG_INFO;
#else
    loglevel=LOG_DUMP;
#endif

    querier = new Querier();
    /* actions from querier */
    connect( querier, SIGNAL(setASQOvermod(bool)), ui->label_audio_overmodulated, SLOT(setEnabled(bool)) );
    connect( querier, SIGNAL(setASQHighLevel(bool)),
             ui->label_audio_asq_triggered_high, SLOT(setEnabled(bool)) );
    connect( querier, SIGNAL(setASQLowLevel(bool)), ui->label_audio_asq_triggered_low, SLOT(setEnabled(bool)) );
    connect( querier, SIGNAL(setASQLevel(int)), this, SLOT(setAudioMeasuredLvl(int)) );
    connect( querier, SIGNAL(setTransmitterAntCap(double)),
             this, SLOT(setAntCapNoEvent(double)) );
    connect( querier, SIGNAL(setTransmitterTuned(int)), this, SLOT( SetTransmitIcon(int) ) );
    connect( querier, SIGNAL(setFrontendFound(bool)), this, SLOT(RedrawEverything(bool)) );

    connect( querier, SIGNAL(setRDSRTMessage(QString)), this, SLOT(updateRDSRTview(QString)) );
    connect( querier, SIGNAL(setRDSRTPlusMarks(QList<int> &, int)), this, SLOT(updateRDSRTPlusMark(QList<int> &, int)) );

    /* RadioText Plus (we need querier initializer 1st) */
    connect( ui->checkBox_rds_title_file, SIGNAL(stateChanged(int)),
             this, SLOT(UpdateRDSRTFileEnabled(int)));
    connect( ui->pushButton_open_rds_title, SIGNAL(clicked()),
             this, SLOT(UpdateRDSRTFile()));
    connect( ui->checkBox_rds_title_rtplus, SIGNAL(stateChanged(int)),
             this, SLOT(UpdateRDSRTPlusEnabled()));
    connect( ui->lineEdit_rds_content1_reg, SIGNAL( editingFinished()),
             this, SLOT(UpdateRDSRTPlusEnabled()));
    connect( ui->lineEdit_rds_content2_reg, SIGNAL( editingFinished()),
             this, SLOT(UpdateRDSRTPlusEnabled()));
    connect( ui->comboBox_rds_content1_type, SIGNAL( currentIndexChanged(int)),
             this, SLOT(UpdateRDSRTPlusEnabled()));
    connect( ui->comboBox_rds_content2_type, SIGNAL( currentIndexChanged(int)),
             this, SLOT(UpdateRDSRTPlusEnabled()));
    connect( ui->lineEdit_rds_content3_reg, SIGNAL( editingFinished()),
             this, SLOT(UpdateRDSRTPlusEnabled()));
    connect( ui->lineEdit_rds_content4_reg, SIGNAL( editingFinished()),
             this, SLOT(UpdateRDSRTPlusEnabled()));
    connect( ui->comboBox_rds_content3_type, SIGNAL( currentIndexChanged(int)),
             this, SLOT(UpdateRDSRTPlusEnabled()));
    connect( ui->comboBox_rds_content4_type, SIGNAL( currentIndexChanged(int)),
             this, SLOT(UpdateRDSRTPlusEnabled()));
    connect( ui->checkBox_rds_dis_item, SIGNAL(stateChanged(int)),
             this, SLOT(UpdateRDSRTPlusEnabled()));
    //connect( ui->actionSelect_Language, SIGNAL(triggered(bool)), this, SLOT(showLanguageMenu(bool)) );
}

void MainWindow::readFromConfig()
{
    //switchLanguage(NULL);

    /* read saved state */
    readSettings(&settings, false);

    toggleASQLow(ui->checkBox_asq_detectLow->isChecked());
    toggleASQHigh(ui->checkBox_asq_detectHigh->isChecked());

    toggleTransmitPilot(ui->checkBox_transmitter_pilot->isChecked());
    toggleTransmitRds(ui->checkBox_transmitter_rds->isChecked());
    toggleTransmitEnabled(ui->checkBox_transmitter_enabled->isChecked());

    toggleRDSaf(ui->checkBox_rds_useaf->isChecked());
    toggleRDSfifo(ui->checkBox_rds_fifo->isChecked());
    toggleRDSPSVisibility(ui->spinBox_rds_ps_msgs->value());
    UpdateRDSRTFileEnabled(ui->checkBox_rds_title_file->isChecked());
    UpdateRDSRTPlusEnabled();
    //updateHTTPListener();

    createLanguageMenu();
    ui->actionSelect_Language->setMenu(languageMenu);

    setupTrayIcon();
    SetWindowMode(!ui->checkBox_mini_mode->isChecked());
    SetAudioCompLimPresetByParams();

    /* actions for querier */
    if(ui->checkBox_rds_time->isChecked()) {
        querier->rdsInterval = ui->spinBox_rds_time_int->value() * 1000;
    }

    /* try to connect */
    toggleConnection(false, false);
    if(ui->checkBox_sw_autoconnect->isChecked())
        querier->timer.start();
    else {
        QStatusBar *status = statusBar();
        status->showMessage(tr("Default config editing mode..."));
        status->show();
    }

    this->adjustSize();
    this->resize(1,1);
    this->updateGeometry();

    qDebug() << __FUNCTION__;
}






MainWindow::~MainWindow()
{
    delete ui;
//    if(querier->isRunning())
//        querier->terminate();
    delete querier;

    trayIcon->setVisible(false);
    trayIcon->hide();
    delete trayIcon;
}



void MainWindow::SetConnectionState()
{
    qDebug() << __FUNCTION__;
    /* force querier to disconnect */
    liveEditingEnabled = false;
    querier->device_lost = true;
    querier->still_not_exists = false;
    toggleConnection(!deviceStatus, true);

    if(deviceStatus)
        querier->timer.setInterval(0);
}



void MainWindow::RedrawEverything(bool bFound)
{
    int ret, transmitting;
    char cpuid[32];
    char rev[32];
    QStatusBar *status = statusBar();
    qDebug() << __FUNCTION__;
    fmtxEEPROMInfoClean();

    liveEditingEnabled=false;
    querier->is_ready=false;
    deviceStatus=bFound;

    toggleConnection(bFound, false);
    if(!bFound) {
        querier->is_ready=ui->checkBox_sw_autoconnect->isChecked();
        if(!ui->checkBox_sw_autoconnect->isChecked())
            querier->timer.stop();
        qDebug() << "frontend flagged as" << bFound << " returning.";
        if(!is_normal || !this->isVisible())
            trayIcon->showMessage(tr("FmStick disconnected"),
                        tr("Device disconnected!"),
                        QSystemTrayIcon::Information, 5 * 1000);
        return;
    }

    status->showMessage(tr("(Re)Reading frontend info..."), 10000);
    status->show();

    cpuid[0] = '\0';
    rev[0] = '\0';
    querier->HidAccessMutex.lock();
    ret = fmtxIoAppIdString(cpuid, rev);
    querier->HidAccessMutex.unlock();
    if(ret!=FMTX_MODE_OK) {
        qDebug() << "can't get id string";
        goto redraw_err;
    }
    qDebug() << "got CPU ID: "<<QString(cpuid)<<" rev.:"<<QString(rev);
    ui->lineEdit_hw_id_cpuid->setText(QString(cpuid));
    ui->lineEdit_hw_id_cpufwid->setText(QString(rev));


    status->showMessage(tr("Reading EEPROM..."), 10000);
    status->show();
    qDebug() << "Reading EEPROM...";
    querier->HidAccessMutex.lock();
    fmtxCmdSpecial = FMTX_SPECIAL_EEPROM;   ///< now work with eeprom instead of frontend
    ret=fmtxEEPROMReadConfig();
    fmtxCmdSpecial = FMTX_SPECIAL_FRONTEND;
    querier->HidAccessMutex.unlock();

    ui->checkBox_hw_run_txAtPowerUp->blockSignals(true);
    ui->checkBox_hw_run_txAtPowerUp->setChecked(fmtxEEPROMGetStartupFlag());
    ui->checkBox_hw_run_txAtPowerUp->blockSignals(false);

    if(ret==FMTX_MODE_POWER_DOWN) {
        qDebug() << "EEPROM is empty yet!";
    } else if(ret!=FMTX_MODE_OK) {
        qDebug() << "Can't read EEPROM";
        goto redraw_err;
    }
    qDebug() << "EEPROM Readed..";


    transmitting = 0;
    querier->HidAccessMutex.lock();
    ret = fmtxIoAppFeGetMode(&transmitting);
    querier->HidAccessMutex.unlock();
    qDebug() << "Frontend mode: " << ret << "transmitting: " << transmitting;
    if(ret == FMTX_MODE_POWER_DOWN){
        qDebug() << "Frontend powered down!";
        ret = SetFrontendPowerUp();
    }

    if(ret!=FMTX_MODE_POWER_UP && ret!=FMTX_MODE_TRANSMITTING) {
        qDebug() << "Frontend not found..";
        SetTransmitIcon(ICON_WARN);
        goto redraw_err;
    }

    if(transmitting) {
        qDebug() << "Already transmitting!";
        SetTransmitIcon(ICON_TX);
    }

    /* needed to detect if hardware supports rds */
    querier->HidAccessMutex.lock();
    DisplayFirmwareInfo();
    querier->HidAccessMutex.unlock();
    if(fmtxCmdStatus!=FMTX_MODE_OK)
        goto redraw_err;

    if(ui->checkBox_rewriteCfg->isChecked()) {
        qDebug() << "Forcing configuration from software, instead of hardware!";
        status->showMessage("Writing running config...", 10000);
        status->show();
        if(SetFrontendParams()!=FMTX_MODE_OK)
            goto redraw_err;
        //hack for digital audio
        if(! SetDigitalAudioHack())
                goto redraw_err;
    }

    status->showMessage(tr("Reading running config..."), 10000);
    status->show();
    querier->HidAccessMutex.lock();
    ret=GetFrontendParams();
    querier->HidAccessMutex.unlock();

    if(ret!=FMTX_MODE_OK)
        goto redraw_err;

    liveEditingEnabled=ui->checkBox_sw_live_edit->isChecked();
    statusBar()->hide();
    this->adjustSize();
    this->resize(1,1);
    this->updateGeometry();
    querier->is_ready=true;

    qDebug() << "Redraw everything done! transmitting: " << transmitting;
    if(!is_normal || !this->isVisible())
        trayIcon->showMessage(tr("FmStick connected"),
                    tr("Device found and connected!%1")
                              .arg( transmitting ? tr("\nAnd transmitter is powered on!") : ""),
                    QSystemTrayIcon::Information, 5 * 1000);
    return;

redraw_err:
    if(!is_normal || !this->isVisible())
        trayIcon->showMessage(tr("FmStick not connected"), tr("Can't connect to frontend!"),
                          QSystemTrayIcon::Critical, 5 * 1000);
    status->showMessage(tr("Can't connect to frontend!"), 10000);
    status->show();
    qDebug() << "Error while redraw everything";
    querier->is_ready=true;
    SetTransmitIcon(ICON_ERR);
}


void MainWindow::DisplayFirmwareInfo(void)
{
    uint8_t buff[16];
    QString digistr;
    QChar fw_major, fw_minor;

    qDebug() << "Reading Firmware info...";

    if((fmtxCmdStatus = fmtxIoAppCommand(GET_REV, buff, 0)) !=FMTX_MODE_OK) goto display_firmware_err;

    digistr = QString::number(buff[1]).rightJustified(2, '0', false);
    ui->lineEdit_hw_id_txpart->setText(QString("SI47").append(digistr));

    fw_major = QChar(buff[2]);
    fw_minor = QChar(buff[3]);
    ui->lineEdit_hw_id_txfwrev->setText(QString(fw_major) + "." + QString(fw_minor));

    ui->lineEdit_hw_id_txpatchid->setText(QString::number(buff[4]) + "." + QString::number(buff[5]));

    fw_major = QChar(buff[6]);
    fw_minor = QChar(buff[7]);
    ui->lineEdit_hw_id_txcomprev->setText(QString(fw_major) + "." + QString(fw_minor));

    ui->lineEdit_hw_id_txchiprev->setText(QString(QChar(buff[8])));


    qDebug() << "Chip identified: SI47" << digistr << "-" << QString(QChar(buff[8])) << QString(QChar(buff[2])) << QString(QChar(buff[3])) << "-GM(R)";

    logwrite(LOG_INFO, "CHIP: si47%d, fw=%c.%c, patch_id=0x%02x.0x%02x, "
                       "chip_fw=%c.%c, chip_rev=%c",
             buff[1],
             buff[2],buff[3],
             buff[4],buff[5],
             buff[6],buff[7],
             buff[8]);
    if ((buff[1] == 11)||(buff[1] == 13)){
        if (buff[2] < 2){
            qDebug() << "Chip supports RDS, but firmware: " << QString(QChar(buff[8])) << "." << QString(QChar(buff[2])) << "is NOT, please change your chip to more modern one...";
        } else {
            qDebug() << "Chip supports RDS";
        }
    }

    qDebug() << "Firmware info readed";
    return;
display_firmware_err:
    qDebug() << "Error while reading firmware info";
}
