/*
 * header for querier
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

#ifndef QUERIER_H
#define QUERIER_H

#include <QtGui>
#include <QObject>
//#include <QThread>
//due: http://developer.qt.nokia.com/wiki/Threads_Events_QObjects
//When shouldnt I use threads?

//class QuerierThread : public QThread
class Querier : public QObject
{
    Q_OBJECT
public:
    explicit Querier(QObject *parent = 0);
    //void run(); // this is virtual method, we must implement it in our subclass of QThread

    QString rdsRTFile;
    QRegExp rdsRTPlusContent1RegExp;
    QRegExp rdsRTPlusContent2RegExp;
    QRegExp rdsRTPlusContent3RegExp;
    QRegExp rdsRTPlusContent4RegExp;
    int rdsRTPlusContent1Type;
    int rdsRTPlusContent2Type;
    int rdsRTPlusContent3Type;
    int rdsRTPlusContent4Type;
    bool    rdsRTPlusDisItem;

    QTimer timer;
    QMutex HidAccessMutex;
    int sleepIntrval;
    int rdsInterval;
    int rdsToWait;
    bool is_ready;
    bool device_lost;
    bool still_not_exists;
    bool parse_rtplus_items(QString &rtPlusLine, QRegExp &rtPlusRegExp, int item_id, QList<int> &itemsList);

signals:
    void setASQOvermod(bool);
    void setASQHighLevel(bool);
    void setASQLowLevel(bool);
    void setASQLevel(int);
    /* 0 - no tune, 1 - tuned, 2 - error */
    void setTransmitterTuned(int);
    void setFrontendFound(bool);
    void setTransmitterAntCap(double);
    void setRDSRTMessage(QString);
    void setRDSRTPlusMarks(QList<int> &itemsList, int retry_id);


public slots:
    void CheckFrontendFlags();

private:
    double last_freq;
    int    last_power;
    double last_rfcap;
    QString last_rdsLine;

    int numRuns;
    int UpdateAsqState();
    int UpdateTuneState();
    //int HandleRDSInt();

};

#endif // QUERIER_H
