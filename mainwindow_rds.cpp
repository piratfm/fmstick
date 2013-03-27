
#include "mainwindow.h"
#include "querier.h"
#include "ui_mainwindow.h"

#include "fmtx.h"
#include "logging.h"

/******************************************************************
 * RDS helpers
 ******************************************************************/
QLineEdit *MainWindow::GetRDSLineEditById(int id)
{
    switch(id){
    case 0:
        return ui->lineEdit_rds_ps_1;
        break;
    case 1:
        return ui->lineEdit_rds_ps_2;
        break;
    case 2:
        return ui->lineEdit_rds_ps_3;
        break;
    case 3:
        return ui->lineEdit_rds_ps_4;
        break;
    case 4:
        return ui->lineEdit_rds_ps_5;
        break;
    case 5:
        return ui->lineEdit_rds_ps_6;
        break;
    case 6:
        return ui->lineEdit_rds_ps_7;
        break;
    case 7:
        return ui->lineEdit_rds_ps_8;
        break;
    case 8:
        return ui->lineEdit_rds_ps_9;
        break;
    case 9:
        return ui->lineEdit_rds_ps_10;
        break;
    case 10:
        return ui->lineEdit_rds_ps_11;
        break;
    case 11:
        return ui->lineEdit_rds_ps_12;
        break;
    }
    return NULL;
}

void MainWindow::DelayedSendPS()
{
    int i;
    for(i=0; i<ui->spinBox_rds_ps_msgs->value(); i++){
        if(SetRDSPSMessage(i) != FMTX_MODE_OK)
            return;
    }
}


void MainWindow::toggleStationId()
{
    qDebug() << __FUNCTION__;
    int i;
    QString NewId, txt;
    QLineEdit *curr_msg = NULL;
    for(i=0; i<ui->spinBox_rds_ps_msgs->value(); i++){
        curr_msg = GetRDSLineEditById(i);
        if(curr_msg) {
            if(i+1 == ui->spinBox_rds_ps_msgs->value()) {
                txt.clear();
                txt.append(curr_msg->text());
            } else {
                txt.fill(' ', 8);
                txt.replace(0, curr_msg->text().length(), curr_msg->text());
            }
            NewId.append(txt);
        }
    }

    while(!NewId.isEmpty() && NewId.endsWith(' '))
        NewId.chop(1);

    qDebug() << "PS Msg: " << NewId;
    ui->lineEdit_station_id->blockSignals(true);
    ui->lineEdit_station_id->setText(NewId);
    ui->lineEdit_station_id->blockSignals(false);
    DelayedSendPS();
}

void MainWindow::toggleRDSPSMsg(QString msg)
{
    qDebug() << __FUNCTION__;
    int i, len = msg.length() / 8;
    QLineEdit *curr_msg;
    if (msg.length() % 8)
        len++;
    for(i=0; i<len; i++){
        QString src = msg.mid(8*i, 8);
        curr_msg = GetRDSLineEditById(i);
        if(curr_msg) {
            curr_msg->blockSignals(true);
            curr_msg->setText(src);
            curr_msg->blockSignals(false);
        }
    }
    if(!msg.length()) ui->lineEdit_rds_ps_1->clear();
    if(!len) len=1;
    ui->spinBox_rds_ps_msgs->setValue(len);
    //ui->frame_rds_ps_1->setVisible(len > 0);
    ui->frame_rds_ps_2->setVisible(len > 1);
    ui->frame_rds_ps_3->setVisible(len > 2);
    ui->frame_rds_ps_4->setVisible(len > 3);
    ui->frame_rds_ps_5->setVisible(len > 4);
    ui->frame_rds_ps_6->setVisible(len > 5);
    ui->frame_rds_ps_7->setVisible(len > 6);
    ui->frame_rds_ps_8->setVisible(len > 7);
    ui->frame_rds_ps_9->setVisible(len > 8);
    ui->frame_rds_ps_10->setVisible(len > 9);
    ui->frame_rds_ps_11->setVisible(len > 10);
    ui->frame_rds_ps_12->setVisible(len > 11);

    DelayedSendPS();
}



/******************************************************************
 * RDS editor
 ******************************************************************/
void MainWindow::DisplayRDSEditor(void)
{
    qDebug() << __FUNCTION__;
    uint16_t rdsFlags = 0;
    double rdsAFFreq = 0;
    uint8_t fifo_sz;
    int ps_msgs, i;
    char psmsgbuff[9];
    char rtmsgbuff[65];
    QString msgPart = "";

    qDebug() << "Reading RDS info...";

    //lineEdit_rds_progid
    ui->lineEdit_rds_progid->setText(QString("%1").arg(fmtxRDSGetPI(), 4, 16).toUpper());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_rds_err;

    rdsFlags = fmtxRDSGetPsMiscFlags();
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_rds_err;

    ui->checkBox_rds_dynPT->setChecked(rdsFlags & TX_RDS_PS_MISC_RDSD3_MASK);
    ui->checkBox_rds_compr_code->setChecked(rdsFlags & TX_RDS_PS_MISC_RDSD2_MASK);
    ui->checkBox_rds_art_head->setChecked(rdsFlags & TX_RDS_PS_MISC_RDSD1_MASK);
    ui->checkBox_rds_is_stereo->setChecked(rdsFlags & TX_RDS_PS_MISC_RDSD0_MASK);
    ui->checkBox_rds_forceb->setChecked(rdsFlags & TX_RDS_PS_MISC_FORCEB_MASK);
    ui->radioButton_rds_music->setChecked(rdsFlags & TX_RDS_PS_MISC_RDSMS_MASK);
    ui->radioButton_rds_speech->setChecked(!(rdsFlags & TX_RDS_PS_MISC_RDSMS_MASK));
    ui->comboBox_rds_pty->setCurrentIndex((rdsFlags & TX_RDS_PS_MISC_RDSPTY_MASK) >> TX_RDS_PS_MISC_RDSPTY_SHFT);

    //checkBox_rds_useaf
    ui->checkBox_rds_useaf->setChecked(fmtxRDSGetPsAFStatus(&rdsAFFreq));
    toggleRDSaf(ui->checkBox_rds_useaf->isChecked());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_rds_err;

    //doubleSpinBox_rds_af
    ui->doubleSpinBox_rds_af->setValue(rdsAFFreq);

    fifo_sz = fmtxRDSGetFifoSize();
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_rds_err;

    ui->checkBox_rds_fifo->setChecked(fifo_sz > 0);
    toggleRDSfifo(fifo_sz > 0);
    ui->spinBox_rds_fifo_sz->setValue(fifo_sz);

    //it's software dependant!
    //checkBox_rds_time
    //spinBox_rds_time_int

    //comboBox_rds_ps_percentage
    ui->comboBox_rds_ps_percentage->setCurrentIndex(fmtxRDSGetPsMixId());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_rds_err;

    //spinBox_rds_ps_repeats
    ui->spinBox_rds_ps_repeats->setValue(fmtxRDSGetPsRepeatCount());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_rds_err;

    ps_msgs = fmtxRDSGetPsMessageCount();
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_rds_err;

    /* No Chance to read this from running frontend */
    if(!ui->checkBox_rewriteCfg->isChecked() || fmtxCmdSpecial == FMTX_SPECIAL_EEPROM) {
        for(i=0; i<ps_msgs; i++)
        {
            fmtxRDSGetPsMessageById(i, psmsgbuff);
            if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_rds_err;

            msgPart = QString(psmsgbuff);
            while(!msgPart.isEmpty() && msgPart.endsWith(' '))
                msgPart.chop(1);

            qDebug() << "PS Msg " << i << ": " << msgPart;
            GetRDSLineEditById(i)->setText(msgPart);
        }
    }

    fmtxRDSGetRtMessage(rtmsgbuff);
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto display_rds_err;
    rtmsgbuff[64] = 0;
    ui->plainTextEdit_rds_rt->setPlainText(QString(rtmsgbuff));

    ui->checkBox_rds_ta->setChecked(rdsFlags & TX_RDS_PS_MISC_RDSTA_MASK);
    ui->checkBox_rds_ta_tp->setChecked(rdsFlags & TX_RDS_PS_MISC_RDSTP_MASK);

    toggleRDSPSVisibility(ps_msgs);
    ui->spinBox_rds_ps_msgs->setValue(ps_msgs);

    qDebug() << "RDS info readed";
    return;

display_rds_err:
    qDebug() << "Error while reading RDS info";
}

void MainWindow::ConfigureRDSEditor(void)
{
    int i;
    QString str;
    qDebug() << "Writing RDS info...";

    SetRDSPsMiscFlags();
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_rds_err;

    SetRDSPI();
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_rds_err;

    SetRDSAF(ui->doubleSpinBox_rds_af->value());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_rds_err;

    if(ui->checkBox_rds_fifo->isChecked())
        SetRDSFIFO(ui->spinBox_rds_fifo_sz->value());
    else
        SetRDSFIFO(0);
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_rds_err;

    SetRDSPSNum(ui->spinBox_rds_ps_msgs->value());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_rds_err;

    for(i=0; i<ui->spinBox_rds_ps_msgs->value(); i++)
        if(SetRDSPSMessage(i) != FMTX_MODE_OK)
            goto configure_rds_err;

    SetRDSPSRepeats(ui->spinBox_rds_ps_repeats->value());
    SetRDSPSPercentage(ui->comboBox_rds_ps_percentage->currentIndex());

    str = ui->plainTextEdit_rds_rt->toPlainText();
    SetRDSRT(&str);

    qDebug() << "RDS info sended";
    return;

configure_rds_err:
    qDebug() << "Error while writing RDS info";
}



/******************************************************************
 * RDS slots
 ******************************************************************/
void MainWindow::SetRDSPI()
{
    bool ok=false;
    uint16_t u16Val;
    if(!liveEditingEnabled)
        return;
    u16Val=ui->lineEdit_rds_progid->text().toUShort(&ok, 16);
    if(!ok) {
        qDebug() << "Can't set RDS PI: \"" << ui->lineEdit_rds_progid->text() << "\" - bad value";
    } else {
        querier->HidAccessMutex.lock();
        if(fmtxRDSSetPI(u16Val) != FMTX_MODE_OK){
            qDebug() << "Can't set RDS AF";
            SetTransmitIcon(ICON_WARN);
        }
        querier->HidAccessMutex.unlock();
    }
}

void MainWindow::SetRDSPsMiscFlags()
{
    uint16_t MiscFlags = 0;
    if(!liveEditingEnabled)
        return;
    if(ui->checkBox_rds_dynPT->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSD3_MASK;
    if(ui->checkBox_rds_compr_code->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSD2_MASK;
    if(ui->checkBox_rds_art_head->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSD1_MASK;
    if(ui->checkBox_rds_is_stereo->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSD0_MASK;
    if(ui->checkBox_rds_forceb->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_FORCEB_MASK;
    if(ui->radioButton_rds_music->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSMS_MASK;

    MiscFlags |= TX_RDS_PS_MISC_RDSPTY_MASK & (ui->comboBox_rds_pty->currentIndex() << TX_RDS_PS_MISC_RDSPTY_SHFT);

    if(ui->checkBox_rds_ta->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSTA_MASK;
    if(ui->checkBox_rds_ta_tp->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSTP_MASK;
    querier->HidAccessMutex.lock();
    if(fmtxRDSSetPsMiscFlags(MiscFlags) != FMTX_MODE_OK){
        qDebug() << "Can't set RDS Misc flag";
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetRDSAF(double dVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxRDSSetPsAF(dVal, ui->checkBox_rds_useaf->isChecked()) != FMTX_MODE_OK){
        qDebug() << "Can't set RDS AF";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::toggleRDSaf(int value)
{
    ui->frame_rds_af->setVisible(value);
    SetRDSAF(ui->doubleSpinBox_rds_af->value());
}

void MainWindow::SetRDSFIFO(int iVal)
{
    if(!liveEditingEnabled)
        return;
    if(!ui->checkBox_rds_fifo->isChecked())
        iVal=0;
    querier->HidAccessMutex.lock();
    if(fmtxRDSSetFifoSize(iVal)!= FMTX_MODE_OK){
        qDebug() << "Can't set RDS FIFO";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::toggleRDSfifo(int value)
{
    ui->frame_rds_fifo->setVisible(value);
    ui->frame_rds_time->setEnabled(value);
    ui->groupBox_rds_sendmsg->setEnabled(value);
    ui->checkBox_rds_time->setEnabled(value);
    SetRDSFIFO(ui->spinBox_rds_fifo_sz->value());
}

void MainWindow::SetRDSTime(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->rdsInterval = iVal * 1000;
}

void MainWindow::toggleRDStime(int value)
{
    ui->frame_rds_time->setVisible(value);
    if(value) {
        querier->rdsInterval = ui->spinBox_rds_time_int->value() * 1000;
    } else {
        querier->rdsInterval=0;
    }
}

void MainWindow::SetRDSPSNum(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxRDSSetPsMessageCount(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set RDS PS Number";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::toggleRDSPSVisibility(int value)
{
    //ui->frame_rds_ps_1->setVisible(value > 0);
    ui->frame_rds_ps_2->setVisible(value > 1);
    ui->frame_rds_ps_3->setVisible(value > 2);
    ui->frame_rds_ps_4->setVisible(value > 3);
    ui->frame_rds_ps_5->setVisible(value > 4);
    ui->frame_rds_ps_6->setVisible(value > 5);
    ui->frame_rds_ps_7->setVisible(value > 6);
    ui->frame_rds_ps_8->setVisible(value > 7);
    ui->frame_rds_ps_9->setVisible(value > 8);
    ui->frame_rds_ps_10->setVisible(value > 9);
    ui->frame_rds_ps_11->setVisible(value > 10);
    ui->frame_rds_ps_12->setVisible(value > 11);
}

void MainWindow::toggleRDSPSNum(int value)
{
    toggleRDSPSVisibility(value);
    toggleStationId();
    SetRDSPSNum(value);
}

void MainWindow::SetRDSPSPercentage(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxRDSSetPsMixId(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set RDS PS Percentage";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetRDSPSRepeats(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxRDSSetPsRepeatCount(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set RDS PS Repeats";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

int MainWindow::SetRDSPSMessage(int id)
{
    QLineEdit *msg=NULL;
    int ret;
    if(!liveEditingEnabled)
        return FMTX_MODE_NONE;
    msg=GetRDSLineEditById(id);
    querier->HidAccessMutex.lock();
    ret = fmtxRDSSetPsMessageById(id, (char *) msg->text().toStdString().c_str());
    if(ret != FMTX_MODE_OK){
        qDebug() << "Can't set RDS PS Message";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
    rtplus_text="";
    return ret;
}

void MainWindow::toggleRDSRT()
{
    QString text = ui->plainTextEdit_rds_rt->toPlainText();
    SetRDSRT(&text);
}



void MainWindow::updateRDSRTPlusMark(QList<int> &itemsList, int retry_id)
{
    QString color_tags[] = {
        "<font style=\"background-color: #CCFFCC\">",
        "<font style=\"background-color: #CCCCFF\">",
        "<font style=\"background-color: #FFCCCC\">",
        "<font style=\"background-color: #FFFFCC\">"
    };


    if(itemsList.size() != 6)
        return;

    qDebug() << "colorize: " <<
                itemsList.at(0) << " => " << itemsList.at(1) << "[" << itemsList.at(2) << "], " <<
                itemsList.at(3) << " => " << itemsList.at(4) << "[" << itemsList.at(5) << "]";

    ui->plainTextEdit_rds_rt->blockSignals(true);
    rtplus_text = ui->plainTextEdit_rds_rt->toPlainText();
    qDebug() << "colorize: input:" << rtplus_text;

    int additional_offset=0;

    if(itemsList.at(1) < itemsList.at(4)) {
        if(itemsList.at(0)) {
            rtplus_text.insert(itemsList.at(1), color_tags[retry_id*2]);
            additional_offset+=color_tags[retry_id*2].length();
            qDebug() << "colorize: [" << additional_offset << "]" << rtplus_text;
            rtplus_text.insert(itemsList.at(1) + additional_offset + itemsList.at(2) + 1, "</font>");
            additional_offset+=7;
            qDebug() << "colorize: [" << additional_offset << "]" << rtplus_text;
        }

        if(itemsList.at(3)) {
            rtplus_text.insert(itemsList.at(4)+additional_offset, color_tags[retry_id*2+1]);
            additional_offset+=color_tags[retry_id*2+1].length();
            qDebug() << "colorize: [" << additional_offset << "]" << rtplus_text;
            rtplus_text.insert(itemsList.at(4)+additional_offset + itemsList.at(5) + 1, "</font>");
            qDebug() << "colorize: [" << additional_offset << "]" << rtplus_text;
        }
    } else {
        if(itemsList.at(3)) {
            rtplus_text.insert(itemsList.at(4), color_tags[retry_id*2+1]);
            additional_offset+=color_tags[retry_id*2+1].length();
            qDebug() << "colorize: [" << additional_offset << "]" << rtplus_text;
            rtplus_text.insert(itemsList.at(4) + additional_offset + itemsList.at(5) + 1, "</font>");
            additional_offset+=7;
            qDebug() << "colorize: [" << additional_offset << "]" << rtplus_text;
        }

        if(itemsList.at(0)) {
            rtplus_text.insert(itemsList.at(1)+additional_offset, color_tags[retry_id*2]);
            additional_offset+=color_tags[retry_id*2].length();
            qDebug() << "colorize: [" << additional_offset << "]" << rtplus_text;
            rtplus_text.insert(itemsList.at(1)+additional_offset + itemsList.at(2) + 1, "</font>");
            qDebug() << "colorize: [" << additional_offset << "]" << rtplus_text;
        }
    }
    qDebug() << "colorize: final: retry[" << retry_id << "]" << rtplus_text;
    ui->plainTextEdit_rds_rt->clear();
    ui->plainTextEdit_rds_rt->appendHtml(rtplus_text);

    ui->plainTextEdit_rds_rt->blockSignals(false);
}

void MainWindow::updateRDSRTview(QString text)
{
    ui->plainTextEdit_rds_rt->blockSignals(true);
    ui->plainTextEdit_rds_rt->setPlainText(text);
    ui->plainTextEdit_rds_rt->blockSignals(false);
}

int MainWindow::SetRDSRT(QString *text_in)
{
    QString text("");
    int ret=0;

    text.append(text_in);
    while(text.length() > 0 && text.endsWith(" "))
        text.chop(1);

    if(!liveEditingEnabled) {
        //return FMTX_MODE_NONE;
    } else {
        querier->HidAccessMutex.lock();
        ret = fmtxRDSSetRtMessage((char *) text.toStdString().c_str());
        if(ret != FMTX_MODE_OK){
            qDebug() << "Can't set RDS RT Message";
            SetTransmitIcon(ICON_WARN);
        }
        querier->HidAccessMutex.unlock();
    }

    /* check if we need RT+ */
    if(ui->checkBox_rds_title_rtplus->isChecked()) {
        QList<int> itemsList;
        int retry_id=0;
        /* song title in the 1st place due it is longer than artist name (in mot cases) */
        querier->parse_rtplus_items(text, querier->rdsRTPlusContent1RegExp, querier->rdsRTPlusContent1Type, itemsList);
        querier->parse_rtplus_items(text, querier->rdsRTPlusContent2RegExp, querier->rdsRTPlusContent2Type, itemsList);
        if(itemsList.size() == 0) {
            querier->parse_rtplus_items(text, querier->rdsRTPlusContent3RegExp, querier->rdsRTPlusContent3Type, itemsList);
            querier->parse_rtplus_items(text, querier->rdsRTPlusContent4RegExp, querier->rdsRTPlusContent4Type, itemsList);
            retry_id++;
        }

        if(itemsList.size()!=0 || !querier->rdsRTPlusDisItem) {
            if(itemsList.size() == 0) itemsList << 0;
            while(itemsList.size() % 6)
                itemsList << 0;
            if(!liveEditingEnabled) {
                updateRDSRTPlusMark(itemsList, retry_id);
            } else {
                querier->HidAccessMutex.lock();
                ret = fmtxRDSSetRtPlusInfo(itemsList.at(0), itemsList.at(1), itemsList.at(2),
                                         itemsList.at(3), itemsList.at(4), itemsList.at(5));
                querier->HidAccessMutex.unlock();
                if(ret != FMTX_MODE_OK){
                    qDebug() << "Can't set RDS RT Message";
                    SetTransmitIcon(ICON_WARN);
                } else {
                    updateRDSRTPlusMark(itemsList, retry_id);
                }
            }
        }
    }
    return ret;
}

void MainWindow::UpdateRDSRTPlusEnabled()
{
    bool enabled = ui->checkBox_rds_title_rtplus->isChecked();
    ui->frame_rds_title_rtplus->setVisible(enabled);
    if(enabled) {
        querier->rdsRTPlusContent1RegExp.setPattern(ui->lineEdit_rds_content1_reg->text());
        querier->rdsRTPlusContent2RegExp.setPattern(ui->lineEdit_rds_content2_reg->text());
        querier->rdsRTPlusContent3RegExp.setPattern(ui->lineEdit_rds_content3_reg->text());
        querier->rdsRTPlusContent4RegExp.setPattern(ui->lineEdit_rds_content4_reg->text());
        querier->rdsRTPlusContent1Type = ui->comboBox_rds_content1_type->currentIndex();
        querier->rdsRTPlusContent2Type = ui->comboBox_rds_content2_type->currentIndex();
        querier->rdsRTPlusContent3Type = ui->comboBox_rds_content3_type->currentIndex();
        querier->rdsRTPlusContent4Type = ui->comboBox_rds_content4_type->currentIndex();
        querier->rdsRTPlusDisItem = ui->checkBox_rds_dis_item->isChecked();
    } else {
        querier->rdsRTPlusContent1RegExp.setPattern("");
        querier->rdsRTPlusContent2RegExp.setPattern("");
        querier->rdsRTPlusContent3RegExp.setPattern("");
        querier->rdsRTPlusContent4RegExp.setPattern("");
        querier->rdsRTPlusContent1Type = RTPLUS_DUMMY_CLASS;
        querier->rdsRTPlusContent2Type = RTPLUS_DUMMY_CLASS;
        querier->rdsRTPlusContent3Type = RTPLUS_DUMMY_CLASS;
        querier->rdsRTPlusContent4Type = RTPLUS_DUMMY_CLASS;
    }
}

void MainWindow::UpdateRDSRTFileEnabled(int enabled)
{
    ui->frame_rds_title_file->setVisible(enabled);
    if(enabled)
        querier->rdsRTFile = ui->lineEdit_rds_title_file->text();
    else
        querier->rdsRTFile = "";
}

/*
Send customized RDS message. Block A setted based on RDS PI-code,\n
other blocks: B,C and D - are avaliable to modify, also you can send\n
message to circular buffer instead of fifo to send it periodically.\n
To send message only once (using fifo buffer), as prefix write "F:",\n
for example: "F:301600004BD7". To send message periodically in\n
circular buffer, write "C:" as prefix, for example: "C:B00824A02005".\n
To Flush circular buffer write "R" symbol before "C:", for example:\n
"RC:B00824A02005" - flushes circular buffer and write first group there\n
Note circular buffe may be cleaned only when re-sending RadioText message.
F:301600004BD7 - send to fifo
C:B00824A02005 - send to circular
RC:B00824A02005 - flush circular and send to circular newer values.
*/
void MainWindow::SetRDSCustomGroup(QString *text)
{
    if(!text)
        text= new QString(ui->lineEdit_rds_custom->text());
    qDebug() << "[MAIN]" << " setting custom group: " << *text;
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();

    if(text->startsWith('R')) {
        if(fmtxRDSSendCustomGroup(TX_RDS_BUFF_IN_MTBUFF, 0, 0, 0, 0, 0, 0) != FMTX_MODE_OK){
            qDebug() << "Can't set RDS Clear Message";
            SetTransmitIcon(ICON_WARN);
            fmtxCmdStatus=FMTX_MODE_NONE;
            return;
        }
        text->remove(0, 1);
    }

    uint8_t flag = TX_RDS_BUFF_IN_LDBUFF;
    if(text->startsWith('F')) {
        flag = flag | TX_RDS_BUFF_IN_FIFO;
    }
    text->remove(0, 2);
    uint8_t B0 = text->mid(0,2).toInt(0, 16);
    uint8_t B1 = text->mid(2,2).toInt(0, 16);
    uint8_t C0 = text->mid(4,2).toInt(0, 16);
    uint8_t C1 = text->mid(6,2).toInt(0, 16);
    uint8_t D0 = text->mid(8,2).toInt(0, 16);
    uint8_t D1 = text->mid(10,2).toInt(0, 16);

    if(fmtxRDSSendCustomGroup(flag, B0, B1, C0, C1, D0, D1) != FMTX_MODE_OK){
        qDebug() << "Can't set RDS Custom Message";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}
