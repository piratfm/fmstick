/*
 * FmStick control application main routine
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

#include <QtGui/QApplication>
#include "mainwindow.h"
#if 1
#include <QtSingleApplication>
#endif


#include "fmtx.h"

int main(int argc, char *argv[])
{
    QtSingleApplication a(argc, argv);

    if (a.sendMessage("Wake up!"))
        return 0;

#if 0
    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("fmstick_") + locale);
    a.installTranslator(&translator);
    qDebug() << QString("fmstick_") + locale;
#endif

    QCoreApplication::setOrganizationName("FreeFadio");
    QCoreApplication::setOrganizationDomain("tipok.org.ua");
    QCoreApplication::setApplicationName("FmStick");
    QCoreApplication::setApplicationVersion("1.2");

    MainWindow w;

    QObject::connect(&a, SIGNAL(messageReceived(const QString&)),
                     &w, SLOT(GotMsgFromAnotherInstance(const QString&)));

    a.setActivationWindow(&w);

    w.show();
    w.readFromConfig();

    /* systray? */
    w.HideIfNeeded();

    return a.exec();
}
