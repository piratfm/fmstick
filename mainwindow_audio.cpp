
#include "mainwindow.h"
#include "querier.h"
#include "ui_mainwindow.h"

#include "fmtx.h"
#include "logging.h"




/******************************************************************
 * Audio editor
 ******************************************************************/
void MainWindow::DisplayAudioEditor(void)
{
    uint8_t audioFlags;
    int16_t audio2bFlags = 0;

    qDebug() << "Reading Audio info...";
    audioFlags=fmtxAudioGetAcompFlags();
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;

    ui->checkBox_audio_enablecompressor->setChecked(audioFlags & TX_ACOMP_ENABLE_ACEN_MASK);
    toggleCompressor(audioFlags & TX_ACOMP_ENABLE_ACEN_MASK);

    ui->comboBox_audioCompressorReleasetime->setCurrentIndex(fmtxAudioGetAcompReleaseTimeId());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;

    ui->doubleSpinBox_audioCompressorAttackTime->setValue(fmtxAudioGetAcompAttackTime());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;
    //(0.5 * (tmpprop+1)

    ui->spinBox_audioCompressorGain->setValue(fmtxAudioGetAcompGain());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;

    ui->spinBox_audiocompressorThreshold->setValue(fmtxAudioGetAcompThreshold());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;

    ui->checkBox_audioLimiterEnable->setChecked(audioFlags & TX_ACOMP_ENABLE_LIMITEN_MASK);
    toggleLimiter(audioFlags & TX_ACOMP_ENABLE_LIMITEN_MASK);

    ui->doubleSpinBox_audio_lim_release_time->setValue(fmtxAudioGetLimiterReleaseTime());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;
    //512/float
    //change audio preset combobox
    SetAudioCompLimPresetByParams();

    audio2bFlags = fmtxAudioGetAsqIntFlags();
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;

    ui->checkBox_asq_overmoddetect->setChecked(audio2bFlags & TX_ASQ_INT_SELECT_OVERMODIEN_MASK);

    ui->checkBox_asq_detectHigh->setChecked(audio2bFlags & TX_ASQ_INT_SELECT_IALHIEN_MASK);
    toggleASQHigh(audio2bFlags & TX_ASQ_INT_SELECT_IALHIEN_MASK);

    ui->spinBox_asq_detectHighDuration->setValue(fmtxAudioGetAsqDurationHigh());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;

    ui->spinBox_asq_detectHighThreshold->setValue(fmtxAudioGetAsqLevelHigh());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;

    ui->checkBox_asq_detectLow->setChecked(audio2bFlags & TX_ASQ_INT_SELECT_IALLIEN_MASK);
    toggleASQLow(audio2bFlags & TX_ASQ_INT_SELECT_IALLIEN_MASK);

    ui->spinBox_asq_detectLowDuration->setValue(fmtxAudioGetAsqDurationLow());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;

    ui->spinBox_asq_detectLowThreshold->setValue(fmtxAudioGetAsqLevelLow());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;

    audio2bFlags = fmtxAudioGetInputLevels();
    ui->spinBox_audio_input_amplitude->setValue(audio2bFlags & TX_LINE_INPUT_LEVEL_LILEVEL_MASK);
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;
    ui->comboBox_audio_input_mute->setCurrentIndex((audio2bFlags & TX_LINE_INPUT_LEVEL_LIATTEN_MASK) >> TX_LINE_INPUT_LEVEL_LIATTEN_SHFT);
    ui->comboBox_audio_input_mute->setCurrentIndex(fmtxAudioGetInputMute());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_audio_err;

    qDebug() << "Audio info readed";
    return;

display_audio_err:
    qDebug() << "Error while reading audio info";
}

void MainWindow::ConfigureAudioEditor(void)
{

    qDebug() << "Sending Audio info...";
    SetAudioCompLimFlags();

    if(ui->checkBox_audio_enablecompressor->isChecked()) {
        SetAudioCompressorThreshold(ui->spinBox_audiocompressorThreshold->value());
        if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_audio_err;

        SetAudioCompressorGain(ui->spinBox_audioCompressorGain->value());
        if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_audio_err;

        SetAudioCompressorReleaseTime(ui->comboBox_audioCompressorReleasetime->currentIndex());
        if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_audio_err;

        SetAudioCompressorAttackTime(ui->doubleSpinBox_audioCompressorAttackTime->value());
        if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_audio_err;
    }

    if(ui->checkBox_audio_enablecompressor->isChecked()) {
        SetAudioLimiterReleaseTime(ui->doubleSpinBox_audio_lim_release_time->value());
        if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_audio_err;
    }

    SetAudioASQFlags();
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_audio_err;

    if(ui->checkBox_asq_detectLow->isChecked()) {
        SetAudioASQLevelLow(ui->spinBox_asq_detectLowThreshold->value());
        if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_audio_err;

        SetAudioASQDurationLow(ui->spinBox_asq_detectLowDuration->value());
        if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_audio_err;
    }

    if(ui->checkBox_asq_detectHigh->isChecked()) {
        SetAudioASQLevelHigh(ui->spinBox_asq_detectHighThreshold->value());
        if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_audio_err;

        SetAudioASQDurationHigh(ui->spinBox_asq_detectHighDuration->value());
        if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_audio_err;
    }

    qDebug() << "Audio info sended";
    return;

configure_audio_err:
    qDebug() << "Error while sending audio info";
}




/******************************************************************
 * Audio slots
 ******************************************************************/
void MainWindow::setAudioMeasuredLvl(int iVal)
{
    ui->label_asq_level->setText(QString("%1").arg(iVal));
    if(iVal < -70)
        iVal = -70;
    if(iVal > 0)
        iVal = 0;
    ui->progressBar_audio_asq_measured->setValue(iVal);
}


void MainWindow::SetAudioCompLimPresetByParams()
{
    if(ui->checkBox_audio_enablecompressor->isChecked() == false &&
       ui->checkBox_audioLimiterEnable->isChecked() == false) {
        //comp/lim: is disabled
        ui->comboBox_audioPreset->blockSignals(true);
        ui->comboBox_audioPreset->setCurrentIndex(0);
        ui->comboBox_audioPreset->blockSignals(false);
        ui->frame_comp_lim->setVisible(false);
        qDebug() << "AudioPreset: got disabled";
        return;
    } else if(ui->checkBox_audio_enablecompressor->isChecked() == false ||
       ui->checkBox_audioLimiterEnable->isChecked() == false) {
        //comp/lim: something - is disabled, custom
        ui->comboBox_audioPreset->blockSignals(true);
        ui->comboBox_audioPreset->setCurrentIndex(4);
        ui->comboBox_audioPreset->blockSignals(false);
        ui->frame_comp_lim->setVisible(true);
        qDebug() << "AudioPreset: got custom";
        return;
    }

    //comp/lim: both checked, now check values
    if( ui->spinBox_audiocompressorThreshold->value() == -40 &&
            ui->spinBox_audioCompressorGain->value() == 15 &&
            ui->comboBox_audioCompressorReleasetime->currentIndex() == 0/*100*/ &&
            ui->doubleSpinBox_audioCompressorAttackTime->value() == 5.0 &&
            ui->doubleSpinBox_audio_lim_release_time->value() == 5.0196) {
        //comp/lim: minimal
        ui->comboBox_audioPreset->blockSignals(true);
        ui->comboBox_audioPreset->setCurrentIndex(1);
        ui->comboBox_audioPreset->blockSignals(false);
        ui->frame_comp_lim->setVisible(false);
        qDebug() << "AudioPreset: got minimal";
        return;
    }

    if( ui->spinBox_audiocompressorThreshold->value() == -20 &&
            ui->spinBox_audioCompressorGain->value() == 10 &&
            ui->comboBox_audioCompressorReleasetime->currentIndex() == 2/*350*/ &&
            ui->doubleSpinBox_audioCompressorAttackTime->value() == 2.0 &&
            ui->doubleSpinBox_audio_lim_release_time->value() == 10.0392) {
        //comp/lim: normal
        ui->comboBox_audioPreset->blockSignals(true);
        ui->comboBox_audioPreset->setCurrentIndex(2);
        ui->comboBox_audioPreset->blockSignals(false);
        ui->frame_comp_lim->setVisible(false);
        qDebug() << "AudioPreset: got normal";
        return;
    }

    if( ui->spinBox_audiocompressorThreshold->value() == -15 &&
            ui->spinBox_audioCompressorGain->value() == 5 &&
            ui->comboBox_audioCompressorReleasetime->currentIndex() == 4/*1000*/ &&
            ui->doubleSpinBox_audioCompressorAttackTime->value() == 0.5 &&
            ui->doubleSpinBox_audio_lim_release_time->value() == 51.2) {
        //comp/lim: normal
        ui->comboBox_audioPreset->blockSignals(true);
        ui->comboBox_audioPreset->setCurrentIndex(3);
        ui->comboBox_audioPreset->blockSignals(false);
        ui->frame_comp_lim->setVisible(false);
        qDebug() << "AudioPreset: got normal";
        return;
    }

    //comp/lim: unknown, set custom
    ui->comboBox_audioPreset->blockSignals(true);
    ui->comboBox_audioPreset->setCurrentIndex(4);
    ui->comboBox_audioPreset->blockSignals(false);
    ui->frame_comp_lim->setVisible(true);
    qDebug() << "AudioPreset: got unknown:"
             << " CT:" << ui->spinBox_audiocompressorThreshold->value()
             << " CG:" << ui->spinBox_audioCompressorGain->value()
             << " RT:" << ui->comboBox_audioCompressorReleasetime->currentIndex()
             << " AT:" << ui->doubleSpinBox_audioCompressorAttackTime->value()
             << " LR:" << ui->doubleSpinBox_audio_lim_release_time->value();

    return;

}

void MainWindow::SetAudioCompLimByPreset(int iVal)
{
    /*0 - dis, 1 - minimal, 2 - normal, 3 - agressive, 4 - custom */
    if(iVal==4) {
        ui->frame_comp_lim->setVisible(true);
    } else {
        ui->frame_comp_lim->setVisible(false);
    }

    switch(iVal){
    case 0:
        ui->checkBox_audio_enablecompressor->setChecked(false);
        ui->checkBox_audioLimiterEnable->setChecked(false);
        break;
    case 1:
        ui->checkBox_audio_enablecompressor->setChecked(true);
        ui->spinBox_audiocompressorThreshold->setValue(-40);
        ui->spinBox_audioCompressorGain->setValue(15);
        ui->comboBox_audioCompressorReleasetime->setCurrentIndex(0/*100*/);
        ui->doubleSpinBox_audioCompressorAttackTime->setValue(5.0);
        ui->checkBox_audioLimiterEnable->setChecked(true);
        ui->doubleSpinBox_audio_lim_release_time->setValue(5.0196);
        break;
    case 2:
        ui->checkBox_audio_enablecompressor->setChecked(true);
        ui->spinBox_audiocompressorThreshold->setValue(-20);
        ui->spinBox_audioCompressorGain->setValue(10);
        ui->comboBox_audioCompressorReleasetime->setCurrentIndex(2/*350*/);
        ui->doubleSpinBox_audioCompressorAttackTime->setValue(2.0);
        ui->checkBox_audioLimiterEnable->setChecked(true);
        ui->doubleSpinBox_audio_lim_release_time->setValue(10.0392);
        break;
    case 3:
        ui->checkBox_audio_enablecompressor->setChecked(true);
        ui->spinBox_audiocompressorThreshold->setValue(-15);
        ui->spinBox_audioCompressorGain->setValue(5);
        ui->comboBox_audioCompressorReleasetime->setCurrentIndex(4/*1000*/);
        ui->doubleSpinBox_audioCompressorAttackTime->setValue(0.5);
        ui->checkBox_audioLimiterEnable->setChecked(true);
        ui->doubleSpinBox_audio_lim_release_time->setValue(51.2);
        break;
    }
}

bool MainWindow::SetAudioCompLimFlags()
{
    uint8_t CompLimFlags = 0;
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    if(ui->checkBox_audio_enablecompressor->isChecked())
        CompLimFlags |= TX_ACOMP_ENABLE_ACEN_MASK;
    if(ui->checkBox_audioLimiterEnable->isChecked())
        CompLimFlags |= TX_ACOMP_ENABLE_LIMITEN_MASK;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAcompFlags(CompLimFlags) != FMTX_MODE_OK){
        qDebug() << "Can't set compressor/limiter flag";
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

void MainWindow::toggleCompressor(int value)
{
    ui->frame_audio_acomp->setVisible(value);
    SetAudioCompLimFlags();
}

bool MainWindow::SetAudioCompressorThreshold(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAcompThreshold(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set audio compressor threshold";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioCompressorGain(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAcompGain(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set audio compressor gain";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioCompressorReleaseTime(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAcompReleaseTimeId(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set audio compressor release time";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioCompressorAttackTime(double dVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAcompAttackTime(dVal) != FMTX_MODE_OK){
        qDebug() << "Can't set audio compressor attack time";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

void MainWindow::toggleLimiter(int value)
{
    ui->frame_audio_alim->setVisible(value);
    SetAudioCompLimFlags();
}

bool MainWindow::SetAudioLimiterReleaseTime(double dVal)
{
    bool ret = true;
    int new_div;

    //prevent division by zero
    if(dVal < 0.256)
        dVal = 0.256;

    new_div = 512.0/dVal;
    //take care about 5th digitsa after point that cutted
    if((audioLimiter_prev_div == new_div) &&
            (512.0/audioLimiter_prev_div - dVal > 0.0001))
        new_div++;

    if(new_div < 5)
        new_div=5;
    if(new_div > 2000)
        new_div=2000;

    audioLimiter_prev_div = new_div;
    qDebug() << __FUNCTION__ << "set_div: " << audioLimiter_prev_div
             << " val:" << 512.0/audioLimiter_prev_div;
    ui->doubleSpinBox_audio_lim_release_time->blockSignals(true);
    ui->doubleSpinBox_audio_lim_release_time->setValue(512.0/audioLimiter_prev_div);
    ui->doubleSpinBox_audio_lim_release_time->blockSignals(false);

    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetLimiterReleaseTime(512.0/audioLimiter_prev_div) != FMTX_MODE_OK){
        qDebug() << "Can't set audio limiter release time";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioASQFlags()
{
    bool ret = true;
    uint8_t ASQFlags = 0;
    if(!liveEditingEnabled)
        return false;
    if(ui->checkBox_asq_overmoddetect->isChecked())
        ASQFlags |= TX_ASQ_INT_SELECT_OVERMODIEN_MASK;
    if(ui->checkBox_asq_detectLow->isChecked())
        ASQFlags |= TX_ASQ_INT_SELECT_IALLIEN_MASK;
    if(ui->checkBox_asq_detectHigh->isChecked())
        ASQFlags |= TX_ASQ_INT_SELECT_IALHIEN_MASK;
    querier->HidAccessMutex.lock();
    if(fmtxAudiosetAsqIntFlags(ASQFlags) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ flags";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioASQLevelLow(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAsqLevelLow(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ Low Level Threshold";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioASQDurationLow(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAsqDurationLow(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ Low Duration";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioASQLevelHigh(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAsqLevelHigh(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ High Level Threshold";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioASQDurationHigh(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAsqDurationHigh(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ High Duration";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

void MainWindow::toggleASQLow(int value)
{
    ui->frame_audio_asq_low->setVisible(value);
    SetAudioASQFlags();
}

void MainWindow::toggleASQHigh(int value)
{
    ui->frame_audio_asq_high->setVisible(value);
    SetAudioASQFlags();
}

bool MainWindow::SetDigitalAudioHack()
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxIoAppEnableAudio() != FMTX_MODE_OK){
        qDebug() << "Can't set Digital audio hack";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioStandAlone(int iVal)
{
    int iVal1 = ui->comboBox_audio_input_level->currentIndex();
    switch(iVal){
    case 0:
        ui->spinBox_audio_input_amplitude->setValue(190);
        break;
    case 1:
        ui->spinBox_audio_input_amplitude->setValue(301);
        break;
    case 2:
        ui->spinBox_audio_input_amplitude->setValue(416);
        break;
    case 3:
        ui->spinBox_audio_input_amplitude->setValue(636);
        break;
    default:
        break;
    }

    int iVal2 = ui->spinBox_audio_input_amplitude->value();
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetInputLevels(iVal1, iVal2) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ High Duration";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}


bool MainWindow::SetAudioStandAloneMute(int val)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetInputMute(val) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ High Duration";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}
