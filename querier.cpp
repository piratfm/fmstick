/*
 * querier is here, it's periodically query device for data.
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

#include "querier.h"
#include "mainwindow.h"
#include "fmtx.h"
#include "logging.h"

Querier::Querier(QObject *parent) :
    QObject(parent)
{
    numRuns=0;
    rdsInterval=0;//disabled
    rdsToWait=0;//disabled
    device_lost=true;
    still_not_exists=false;
    is_ready=false;
    last_freq = 0;
    last_power = 0;
    last_rfcap = 0;
    rdsRTFile = "";
    rdsRTPlusContent1RegExp = QRegExp("", Qt::CaseSensitive, QRegExp::RegExp2);
    rdsRTPlusContent2RegExp = QRegExp("", Qt::CaseSensitive, QRegExp::RegExp2);
    rdsRTPlusContent3RegExp = QRegExp("", Qt::CaseSensitive, QRegExp::RegExp2);
    rdsRTPlusContent4RegExp = QRegExp("", Qt::CaseSensitive, QRegExp::RegExp2);
    rdsRTPlusContent1Type = RTPLUS_ITEM_TITLE;//RTPLUS_DUMMY_CLASS;
    rdsRTPlusContent2Type = RTPLUS_ITEM_ARTIST;//RTPLUS_DUMMY_CLASS;
    rdsRTPlusContent3Type = RTPLUS_PHONE_STUDIO;//RTPLUS_DUMMY_CLASS;
    rdsRTPlusContent4Type = RTPLUS_INFO_URL;//RTPLUS_DUMMY_CLASS;
    rdsRTPlusDisItem = true;

    last_rdsLine = "";
    connect(&timer, SIGNAL(timeout()), this, SLOT(CheckFrontendFlags()));
    timer.setInterval(1000);
}


bool Querier::parse_rtplus_items(QString &rtPlusLine, QRegExp &rtPlusRegExp, int item_id, QList<int> &itemsList)
{
    if(rtPlusRegExp.isEmpty())
        return FALSE;

    //rtPlusRegExp.setMinimal(TRUE);
    int pos = rtPlusRegExp.indexIn(rtPlusLine, 0);
    if(pos < 0 || rtPlusRegExp.cap(1).length() < 1)
        return FALSE;

    qDebug() << "RT+ rds " << itemsList.size()/3 <<
                " item_id:" << item_id <<
                " regexp:" << rtPlusRegExp.pattern() <<
                " pos:" << rtPlusLine.indexOf(rtPlusRegExp.cap(1), pos) <<
                " len:" << rtPlusRegExp.cap(1).length() <<
                " STR: " << rtPlusRegExp.cap(1);

    itemsList << item_id
              << rtPlusLine.indexOf(rtPlusRegExp.cap(1), pos)
              << rtPlusRegExp.cap(1).length() - 1;
    return TRUE;
}

void Querier::CheckFrontendFlags()
{
    uint8_t ret;
    //int log_save=loglevel;

    if(device_lost) {
        HidAccessMutex.lock();
        ret = fmtxIoAppDeviceFound();
        HidAccessMutex.unlock();
        if(ret!=FMTX_MODE_OK) {
            if(still_not_exists)
                qDebug() << "Device still not exist!";
            else {
                qDebug() << "Device not found!";
                setFrontendFound(false);
                still_not_exists=true;
            }
            if(timer.interval() != 20000)
                timer.setInterval(20000);
            return;
        } else {
            qDebug() << "Device Is Appear!";
            setFrontendFound(true);
            device_lost=false;
            still_not_exists=false;
            timer.setInterval(5000); //give it a time to init.
            return;
        }
    }

    if(!is_ready) {
        qDebug() << "Querier is not ready yet!";
        return;
    }

    //loglevel = LOG_ERROR;
    numRuns++;
#if 0
    HidAccessMutex.lock();
    ret = fmtxCmdStatus = fmtxIoAppCommand(GET_INT_STATUS, buff, 0);
    HidAccessMutex.unlock();

    if(ret!=FMTX_MODE_OK)
        goto err_transfer;

    int_status=buff[0];

    if(!(int_status & CTS) || (int_status & ERR)) {
        qDebug() << "Error when reading interrupt.";
        return;
    }
#endif
    if(UpdateTuneState()) goto err_transfer;

    //we need signal audio level, so use this interrupt everytime!
    //if (int_status & ASQINT)
    if(UpdateAsqState()) goto err_transfer;

    //if (int_status & STCINT)

    //if (int_status & RDSINT)
    //    if(HandleRDSInt()) goto err_transfer;

    if(rdsInterval){
        rdsToWait += timer.interval();
        if(rdsToWait >= rdsInterval) {
            rdsToWait=0;
            qDebug() << "Sending RDS Time ";
            HidAccessMutex.lock();
            ret = fmtxRDSSendTimeStamp();
            HidAccessMutex.unlock();
            if(ret!=FMTX_MODE_OK)
                goto err_transfer;
        }

    }

    if(rdsRTFile.length() > 0) {
        qDebug() << "Sending RDS RadioText from file" << rdsRTFile;
        QFile rdsFile(rdsRTFile);
        if ( rdsFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
            QTextStream rdsStream( &rdsFile );
            QString rdsLine(rdsStream.read(64));
            while(rdsLine.length() > 0 && rdsLine.endsWith(" "))
                rdsLine.chop(1);

            if(last_rdsLine != rdsLine) {
                last_rdsLine = rdsLine;
                //QMessageBox::about ( NULL, tr("About FmStick"), rdsLine);
                logwrite(LOG_INFO, "RadioText: \"%s\"", rdsLine.toStdString().c_str());
                HidAccessMutex.lock();
                ret = fmtxRDSSetRtMessage(rdsLine.toStdString().c_str());
                HidAccessMutex.unlock();
                if(ret != FMTX_MODE_OK){
                    qDebug() << "Can't set RDS RT Message";
                    goto err_transfer;
                } else {
                    //plainTextEdit_rds_rt
                    setRDSRTMessage(rdsLine);
                }

                /* parse and find RT+ tags */
                QList<int> itemsList;
                int retry_id=0;
                /* song title in the 1st place due it is longer than artist name (in mot cases) */
                parse_rtplus_items(rdsLine, rdsRTPlusContent1RegExp, rdsRTPlusContent1Type, itemsList);
                parse_rtplus_items(rdsLine, rdsRTPlusContent2RegExp, rdsRTPlusContent2Type, itemsList);
                if(itemsList.size() == 0) {
                    parse_rtplus_items(rdsLine, rdsRTPlusContent3RegExp, rdsRTPlusContent3Type, itemsList);
                    parse_rtplus_items(rdsLine, rdsRTPlusContent4RegExp, rdsRTPlusContent4Type, itemsList);
                    retry_id++;
                }

                if(itemsList.size()!=0 || !rdsRTPlusDisItem) {
                    if(itemsList.size() == 0) itemsList << 0;
                    while(itemsList.size() % 6)
                        itemsList << 0;
                    HidAccessMutex.lock();
                    fmtxRDSSetRtPlusInfo(itemsList.at(0), itemsList.at(1), itemsList.at(2),
                                             itemsList.at(3), itemsList.at(4), itemsList.at(5));
                    HidAccessMutex.unlock();
                    if(ret != FMTX_MODE_OK){
                        qDebug() << "Can't set RDS RT Message";
                        goto err_transfer;
                    } else {
                        //plainTextEdit_rds_rt
                        setRDSRTPlusMarks(itemsList, retry_id);
                    }
                }
            } else {
                qDebug() << "RDS RadioText file contains same text";
            }
            rdsFile.close();
        } else {
            qDebug() << "Can't open RDS RadioText file";
        }
    }

    if(timer.interval() != 5000)
        timer.setInterval(5000);
    //loglevel = log_save;
    return;

err_transfer:
    qDebug() << "Bad status from hidIoCOmmand: " << fmtxCmdStatus;
    if(!device_lost)
        qDebug() << "Device Is Lost!";
    device_lost=true;
    //loglevel = LOG_DEBUG;
    setFrontendFound(false);
    still_not_exists=true;
    timer.setInterval(20000);
    return;
}



int Querier::UpdateAsqState()
{
    uint8_t flags;
    int8_t  level;

    HidAccessMutex.lock();
    fmtxCmdStatus = fmtxIoAppAsqStatus(&flags, &level);
    HidAccessMutex.unlock();

    switch(fmtxCmdStatus){
    case FMTX_MODE_OK:
        break;
    case FMTX_MODE_POWER_DOWN:
        flags=0;
        level=-70;
        fmtxCmdStatus = FMTX_MODE_OK;
        break;
    default:
        qDebug() << "Bad status from hidIoCOmmand: " << fmtxCmdStatus;
        return 1;
    }

    setASQOvermod(flags & TX_ASQ_STATUS_OUT_OVERMOD);
    if (flags & TX_ASQ_STATUS_OUT_OVERMOD)
        qDebug() << "AUDIO: Overmodulated!";
    setASQHighLevel(flags & TX_ASQ_STATUS_OUT_IALH);
    if (flags & TX_ASQ_STATUS_OUT_IALH)
        qDebug() << "AUDIO: Input Audio Level Threshold Detected High";
    setASQLowLevel(flags & TX_ASQ_STATUS_OUT_IALL);
    if (flags & TX_ASQ_STATUS_OUT_IALL)
        qDebug() << "AUDIO: Input Audio Level Threshold Detected Low";
    setASQLevel((int) level);
    qDebug() << "AUDIO: " << level << " dBfs";
    return 0;
}

int Querier::UpdateTuneState()
{
    uint8_t power=0;
    double freq=0, rfcap=0;

    HidAccessMutex.lock();
    fmtxCmdStatus = fmtxTransmitterGetTuneStatus(&freq, &power, &rfcap);
    HidAccessMutex.unlock();

    if(fmtxCmdStatus !=FMTX_MODE_OK) {
        qDebug() << "Bad status from hidIoCOmmand: " << fmtxCmdStatus;
        return 1;
    }

    if(freq != last_freq || power != last_power) {
        setTransmitterTuned(freq > 0 && power > 0 ? ICON_TX : ICON_NOTX);
        last_freq = freq;
        last_power = power;
        logwrite(LOG_INFO, "TUNING: device tuned: freq=%f MHz, voltage=%d dBuV, ant_cap=%f pF",
                 freq, power, rfcap);
    }

    if(rfcap != last_rfcap) {
        setTransmitterAntCap(rfcap);
        last_rfcap = rfcap;
    }

    return 0;
}

#if 0
int Querier::HandleRDSInt()
{
    uint8_t buff[16];
    HidAccessMutex.lock();
    if((fmtxCmdStatus = fmtxIoAppCommand(TX_RDS_BUFF, buff, 7, TX_RDS_BUFF_IN_INTACK,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00)) !=FMTX_MODE_OK) {
        HidAccessMutex.unlock();
        qDebug() << "Bad status from hidIoCOmmand: " << fmtxCmdStatus;
        return 1;
    }
    HidAccessMutex.unlock();

    if (buff[1] & TX_RDS_BUFF_OUT_RDSPSXMIT)
        qDebug() << "RDS: PS Group has been transmitted.";
    if (buff[1] & TX_RDS_BUFF_OUT_CBUFXMIT)
        qDebug() << "RDS: Group has been transmitted from the circular buffer.";
    if (buff[1] & TX_RDS_BUFF_OUT_FIFOXMIT)
        qDebug() << "RDS: Group has been transmitted from the FIFO buffer.";
    if (buff[1] & TX_RDS_BUFF_OUT_CBUFWRAP)
        qDebug() << "RDS: Group Circular Buffer has wrapped.";
    if (buff[1] & TX_RDS_BUFF_OUT_FIFOMT)
        qDebug() << "RDS: Group FIFO Buffer is empty.";

    logwrite(LOG_DEBUG, "RDS: circular: %d/%d, fifo: %d/%d", buff[3], buff[2], buff[5], buff[4]);
    return 0;
}
#endif
