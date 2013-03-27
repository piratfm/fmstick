
#include "mainwindow.h"
#include "querier.h"
#include "ui_mainwindow.h"

#include "fmtx.h"
#include "logging.h"


/******************************************************************
 * Transmitter editor
 ******************************************************************/
void MainWindow::DisplayTransmitterEditor(void)
{
    uint16_t transmitterFlags = 0;
    uint8_t power=0;
    double freq=0.0;
    double rfcap=0;
    int fmtxStatus=0;

    qDebug() << "Reading Transmitter info...";

    transmitterFlags=fmtxTransmitterGetComponentFlags();
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_transmitter_err;

    ui->comboBox_transmitter_preemph->setCurrentIndex(fmtxTransmitterGetPreemphasisId());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_transmitter_err;

    ui->spinBox_transmitter_audioDev->setValue(fmtxTransmitterGetAudioDeviation());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_transmitter_err;

    ui->checkBox_transmitter_pilot->setChecked(transmitterFlags & TX_COMPONENT_ENABLE_PILOT_MASK);
    toggleTransmitPilot(transmitterFlags & TX_COMPONENT_ENABLE_PILOT_MASK);

    ui->spinBox_transmitter_pilot_devi->setValue(fmtxTransmitterGetPilotDeviation());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_transmitter_err;

    ui->spinBox_transmitter_pilot_freq->setValue(fmtxTransmitterGetPilotFrequency());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_transmitter_err;

    ui->checkBox_transmitter_lmr->setChecked(transmitterFlags & TX_COMPONENT_ENABLE_LMR_MASK);

    ui->checkBox_transmitter_rds->setChecked(transmitterFlags & TX_COMPONENT_ENABLE_RDS_MASK);
    toggleTransmitRds(transmitterFlags & TX_COMPONENT_ENABLE_RDS_MASK);

    //spinBox_transmitter_rds_devi
    ui->spinBox_transmitter_rds_devi->setValue(fmtxTransmitterGetRDSDeviation());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_transmitter_err;

    fmtxStatus = fmtxTransmitterGetTuneStatus(&freq, &power, &rfcap);
    qDebug() << "Transmitter configuration("<< fmtxCmdStatus <<"): " <<
                "freq:" << freq << ", power:" << power << ", rfcap:" << rfcap << ", tuneStatus:" << fmtxStatus;
    if(fmtxCmdStatus!=FMTX_MODE_OK && fmtxCmdStatus!=FMTX_MODE_TRANSMITTING) goto display_transmitter_err;

    ui->checkBox_transmitter_enabled->setChecked(power>0);
    toggleTransmitEnabled(power>0);
    SetTransmitIcon(power>0 ? ICON_TX : ICON_NOTX);

    ui->doubleSpinBox_transmitter_freq->setValue(freq);

    ui->spinBox_transmitter_power->setValue(power);

    //TODO: we dont know who setted this value, user or ic
    //ui->checkBox_transmitter_antcap->setChecked(false);
    //toggleTransmitAntCap(0);
    //doubleSpinBox_transmitter_antcap_val
    ui->doubleSpinBox_transmitter_antcap_val->setValue(rfcap);

    qDebug() << "Transmitter info readed";
    return;

display_transmitter_err:
    qDebug() << "Error while reading transmitter info";
}

void MainWindow::ConfigureTransmitterEditor(void)
{
    int ret;

    qDebug() << "Setting Transmitter info...";

    SetTransmitPreemphasis(ui->comboBox_transmitter_preemph->currentIndex());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    SetTransmitAudioDeviation(ui->spinBox_transmitter_audioDev->value());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    SetTransmitPilotDeviation(ui->spinBox_transmitter_pilot_devi->value());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    SetTransmitPilotFrequency(ui->spinBox_transmitter_pilot_freq->value());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    SetTransmitRDSDeviation(ui->spinBox_transmitter_rds_devi->value());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    SetTransmitFrequency(ui->doubleSpinBox_transmitter_freq->value());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    querier->HidAccessMutex.lock();
    if(ui->checkBox_transmitter_enabled->isChecked()) {
        if(ui->checkBox_transmitter_antcap->isChecked())
            ret=fmtxTransmitterSetTunePower(ui->spinBox_transmitter_power->value(), ui->doubleSpinBox_transmitter_antcap_val->value());
        else
            ret=fmtxTransmitterSetTunePower(ui->spinBox_transmitter_power->value(), 0.0);
        SetTransmitIcon(ICON_TX);
    } else {
        ret=fmtxTransmitterSetTunePower(0, 0.0);
        SetTransmitIcon(ICON_NOTX);
    }
    querier->HidAccessMutex.unlock();
    if(ret != FMTX_MODE_OK) {
        qDebug() << "Can't enable transmission";
        SetTransmitIcon(ICON_WARN);
        goto configure_transmitter_err;
    }

    SetTransmitComponentFlags();
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    qDebug() << "Transmitter info sended";
    return;

configure_transmitter_err:
    qDebug() << "Error while sending transmitter info";
}



/******************************************************************
 * Transmitter slots
 ******************************************************************/
void MainWindow::setAntCapNoEvent(double dVal)
{
    ui->doubleSpinBox_transmitter_antcap_val->blockSignals(true);
    ui->doubleSpinBox_transmitter_antcap_val->setValue(dVal);
    ui->doubleSpinBox_transmitter_antcap_val->blockSignals(false);
}


void MainWindow::SetTransmitPreemphasis(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetPreemphasisId(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter preemphasis flag";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetTransmitAudioDeviation(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetAudioDeviation(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter audio deviation flag";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetTransmitComponentFlags()
{
    uint8_t ComponentFlags = 0;
    if(!liveEditingEnabled)
        return;
    if(ui->checkBox_transmitter_lmr->isChecked())
        ComponentFlags |= TX_COMPONENT_ENABLE_LMR_MASK;
    if(ui->checkBox_transmitter_pilot->isChecked())
        ComponentFlags |= TX_COMPONENT_ENABLE_PILOT_MASK;
    if(ui->checkBox_transmitter_rds->isChecked())
        ComponentFlags |= TX_COMPONENT_ENABLE_RDS_MASK;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetComponentFlags(ComponentFlags) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter components flag";
        SetTransmitIcon(ICON_WARN);
    }

    querier->HidAccessMutex.unlock();
}

void MainWindow::SetTransmitPilotDeviation(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetPilotDeviation(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter pilot deviation flag";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetTransmitPilotFrequency(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetPilotFrequency(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter pilot frequency flag";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetTransmitRDSDeviation(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetRDSDeviation(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter audio deviation flag";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetTransmitFrequency(double dVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetTuneFreq(dVal) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter audio deviation flag";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

/* sets power and antcap at the same time */
int MainWindow::SetTransmitPower()
{
    uint8_t ret;
    ui->label_transmitter_level->setText(QString("%1")
                                         .arg(ui->spinBox_transmitter_power->value()));
    if(!liveEditingEnabled)
        return FMTX_MODE_NONE;
    querier->HidAccessMutex.lock();
    if(ui->checkBox_transmitter_antcap->isChecked()) {
        ret=fmtxTransmitterSetTunePower(ui->spinBox_transmitter_power->value(), ui->doubleSpinBox_transmitter_antcap_val->value());
    } else {
        ret=fmtxTransmitterSetTunePower(ui->spinBox_transmitter_power->value(), 0.0);
    }
    if(ret != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter power and capacitor";
        SetTransmitIcon(ICON_WARN);
    } else
    querier->HidAccessMutex.unlock();

    querier->timer.setInterval(2000);
    return ret;
}

void MainWindow::SetTransmitIcon( int iVal )
{
    qDebug() << "SetTransmitIcon" << iVal;
    switch(iVal){
    case ICON_NOTX:
        ui->commandLinkButton_station->setIcon(notxicon);
        trayIcon->setIcon(notxicon);
        ui->actionToggle_transmission->setIcon(notxicon);
        ui->actionToggle_transmission->setText(tr("Enable TX?"));
        break;
    case ICON_TX:
        ui->commandLinkButton_station->setIcon(txicon);
        trayIcon->setIcon(txicon);
        ui->actionToggle_transmission->setIcon(txicon);
        ui->actionToggle_transmission->setText(tr("Disable TX?"));
        break;
    case ICON_WARN:
        ui->commandLinkButton_station->setIcon(warningicon);
        trayIcon->setIcon(warningicon);
        break;
    case ICON_ERR:
        ui->commandLinkButton_station->setIcon(erroricon);
        trayIcon->setIcon(erroricon);
        break;
    case ICON_NODEV:
        ui->commandLinkButton_station->setIcon(nodeviceicon);
        trayIcon->setIcon(nodeviceicon);
        break;
    }
}

void MainWindow::SetTransmitEnabled( bool bEnabled )
{
    uint8_t ret;
    if(!liveEditingEnabled)
        return;

    qDebug() << QString("Transmission: %1").arg(bEnabled?"enabled":"disabled");

    if(bEnabled) {
        ret = SetTransmitPower();
        SetTransmitIcon(ICON_TX);
    } else {
        querier->HidAccessMutex.lock();
        ret=fmtxTransmitterSetTunePower(0, 0.0);
        querier->HidAccessMutex.unlock();
        querier->timer.setInterval(2000);
        SetTransmitIcon(ICON_NOTX);
    }

    if(ret != FMTX_MODE_OK){
        qDebug() << "Can't enable transmission";
        SetTransmitIcon(ICON_WARN);
    }
}

void MainWindow::toggleTransmitEnabled(int value)
{
    ui->frame_transmitter_tx->setVisible(value);
    ui->verticalSlider_main_power->setEnabled(value);
    SetTransmitEnabled( value );
}

void MainWindow::toggleTransmitAntCap(int value)
{
    ui->frame_transmitter_antcap->setEnabled(value);
    SetTransmitPower();
}

void MainWindow::toggleTransmitRds(int value)
{
    ui->frame_rds_deviation->setVisible(value);
    ui->widget_rds->setEnabled(value);
    SetTransmitComponentFlags();
}

void MainWindow::toggleTransmitPilot(int value)
{
    ui->frame_transmitter_pilot->setVisible(value);
    SetTransmitComponentFlags();
}
