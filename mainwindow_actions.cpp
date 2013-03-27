#include "mainwindow.h"
#include "querier.h"
#include "ui_mainwindow.h"

#include "fmtx.h"
#include "logging.h"


/******************************************************************
 * Various Mainwindow actions
 ******************************************************************/
void MainWindow::retranslateUi()
{
    int backup1, backup2, backup3, backup4, backup5, backup6, backup7, backup8, backup9, backup10, backup11;
    //Worakaround for: QTBUG-3863
    backup1 = ui->comboBox_audioCompressorReleasetime->currentIndex();
    backup2 = ui->comboBox_audioPreset->currentIndex();
    backup3 = ui->comboBox_rds_ps_percentage->currentIndex();
    backup4 = ui->comboBox_rds_pty->currentIndex();
    backup5 = ui->comboBox_rds_content1_type->currentIndex();
    backup6 = ui->comboBox_rds_content2_type->currentIndex();
    backup7 = ui->comboBox_rds_content3_type->currentIndex();
    backup8 = ui->comboBox_rds_content4_type->currentIndex();
    backup9 = ui->comboBox_transmitter_preemph->currentIndex();
    backup10 = ui->comboBox_audio_input_level->currentIndex();
    backup11 = ui->comboBox_audio_input_mute->currentIndex();

    ui->retranslateUi(this);

    ui->comboBox_audioCompressorReleasetime->setCurrentIndex(backup1);
    ui->comboBox_audioCompressorReleasetime->setCurrentIndex(backup1);
    ui->comboBox_audioPreset->setCurrentIndex(backup2);
    ui->comboBox_rds_ps_percentage->setCurrentIndex(backup3);
    ui->comboBox_rds_pty->setCurrentIndex(backup4);
    ui->comboBox_rds_content1_type->setCurrentIndex(backup5);
    ui->comboBox_rds_content2_type->setCurrentIndex(backup6);
    ui->comboBox_rds_content3_type->setCurrentIndex(backup7);
    ui->comboBox_rds_content4_type->setCurrentIndex(backup8);
    ui->comboBox_transmitter_preemph->setCurrentIndex(backup9);
    ui->comboBox_audio_input_level->setCurrentIndex(backup10);
    ui->comboBox_audio_input_mute->setCurrentIndex(backup11);

    //menu->setTitle(tr("File"));
    //fileAction->setText(tr("First action"));
}

void MainWindow::toggleLiveEditing(bool bVal)
{
    if(deviceStatus)
        liveEditingEnabled = bVal;
}


void MainWindow::toggleConnection(bool bVal, bool toggleQuerier)
{
    qDebug() << __FUNCTION__ << ": " << bVal;
    /* actions */
    ui->actionRead_from_Device->setEnabled(bVal);
    ui->actionRead_from_EEPROM->setEnabled(bVal);
    ui->actionWrite_to_Device->setEnabled(bVal);
    ui->actionWrite_to_EEPROM->setEnabled(bVal);
    ui->actionToggle_transmission->setEnabled(bVal);
    /* hardware buttons */
    ui->checkBox_hw_run_txAtPowerUp->setEnabled(bVal);
    ui->pushButton_fe_powerdown->setEnabled(bVal);
    ui->pushButton_fe_powerup->setEnabled(bVal);
    ui->pushButton_fe_reset->setEnabled(bVal);
    ui->pushButton_open_firmware->setEnabled(bVal);

    if(bVal) {
        if(ui->checkBox_rds_fifo->isChecked()) {
            ui->groupBox_rds_sendmsg->setEnabled(bVal);
        }
        if(toggleQuerier)
            querier->timer.start();
        SetTransmitIcon(ICON_WARN);
        ui->actionConnect_to_device->setText("Disconnect?");
        ui->actionConnect_to_device->setIcon(QIcon(":/fmstick/images/connect_32.png"));
    } else {
        ui->groupBox_rds_sendmsg->setEnabled(bVal);
        if(toggleQuerier) {
            querier->timer.stop();
        }
        SetTransmitIcon(ICON_NODEV);
        ui->actionConnect_to_device->setIcon(QIcon(":/fmstick/images/disconnect_32.png"));
        ui->actionConnect_to_device->setText("Connect?");
    }
    deviceStatus = bVal;
}


void MainWindow::SetFrontendPowerDown() {
    qDebug() << __FUNCTION__;
    if(fmtxIoAppFeDown() != FMTX_MODE_POWER_DOWN) {
        qDebug() << "error while powering down";
    }
    SetTransmitIcon(ICON_WARN);
    querier->is_ready=false;
    liveEditingEnabled=false;
    ui->actionRead_from_Device->setEnabled(false);
    ui->actionWrite_to_Device->setEnabled(false);
    fmtxCmdStatus=FMTX_MODE_NONE;
}

int MainWindow::SetFrontendPowerUp() {
    int ret;
    qDebug() << __FUNCTION__;
    querier->HidAccessMutex.lock();
    ret = fmtxIoAppFeUp();
    querier->HidAccessMutex.unlock();
    switch(ret){
    case FMTX_MODE_POWER_UP:
        qDebug() << "Frontend powered up!";
        SetTransmitIcon(ICON_NOTX);
        fmtxCmdStatus=FMTX_MODE_OK;
        break;
    case FMTX_MODE_TRANSMITTING:
        qDebug() << "Frontend already powered up!";
        SetTransmitIcon(ICON_TX);
        fmtxCmdStatus=FMTX_MODE_OK;
        break;
    default:
        qDebug() << "Error while powering up";
        fmtxCmdStatus=FMTX_MODE_NONE;
        break;
    }

    if(ret == FMTX_MODE_POWER_UP || ret == FMTX_MODE_TRANSMITTING) {
        querier->is_ready=true;
        liveEditingEnabled=ui->checkBox_sw_live_edit->isChecked();
        ui->actionRead_from_Device->setEnabled(true);
        ui->actionWrite_to_Device->setEnabled(true);
    }

    return ret;
}

void MainWindow::SetFrontendReset() {
    qDebug() << __FUNCTION__;
    if(fmtxIoAppFeReset() != FMTX_MODE_POWER_DOWN)
        qDebug() << "Error while resetting frontend";
    querier->is_ready=false;
    liveEditingEnabled=false;
    SetTransmitIcon(ICON_WARN);
    ui->actionRead_from_Device->setEnabled(false);
    ui->actionWrite_to_Device->setEnabled(false);
    fmtxCmdStatus=FMTX_MODE_NONE;
}

void MainWindow::StationButtonClicked()
{
    qDebug() << __FUNCTION__;
    bool is_checked = ui->checkBox_transmitter_enabled->isChecked();
    ui->checkBox_transmitter_enabled->setChecked(!is_checked);
}


int MainWindow::GetFrontendParams()
{
    qDebug() << __FUNCTION__;
    liveEditingEnabled=false;

    DisplayAudioEditor();
    if(fmtxCmdStatus!=FMTX_MODE_OK)
        goto frontend_params_err;

    DisplayTransmitterEditor();
    if(fmtxCmdStatus!=FMTX_MODE_OK)
        goto frontend_params_err;

    DisplayRDSEditor();
    if(fmtxCmdStatus!=FMTX_MODE_OK)
        goto frontend_params_err;
    liveEditingEnabled=ui->checkBox_sw_live_edit->isChecked();
    /* run querier, so we can read interrupts and send rds timestamps */
    return fmtxCmdStatus;

frontend_params_err:
    liveEditingEnabled=false;
    qDebug() << "Can't get frontend parameters";
    SetTransmitIcon(ICON_ERR);
    return fmtxCmdStatus;
}

int MainWindow::SetFrontendParams()
{
    qDebug() << __FUNCTION__;
    liveEditingEnabled=true;

    ConfigureAudioEditor();
    if(fmtxCmdStatus!=FMTX_MODE_OK)
        goto frontend_params_err;

    ConfigureTransmitterEditor();
    if(fmtxCmdStatus!=FMTX_MODE_OK)
        goto frontend_params_err;

    ConfigureRDSEditor();
    if(fmtxCmdStatus!=FMTX_MODE_OK)
        goto frontend_params_err;

    if(!SetDigitalAudioHack())
        goto frontend_params_err;

    /* run querier, so we can read interrupts and send rds timestamps */
    qDebug() << "Frontend parameters setted correctly.";
    liveEditingEnabled=ui->checkBox_sw_live_edit->isChecked();
    return fmtxCmdStatus;

frontend_params_err:
    liveEditingEnabled=false;
    qDebug() << "Can't set frontend parameters";
    SetTransmitIcon(ICON_ERR);
    return fmtxCmdStatus;
}
