/*
 * additional helpers functions located here
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

#include <QMouseEvent>
#include <QtGui/QApplication>
#include "fmtx.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef Q_WS_WIN
#include <windows.h>
#endif

void MainWindow::GotMsgFromAnotherInstance(const QString& str)
{
    qDebug() << "got msg" << str << "from another instance";
    if(str == "Wake up!") {
        if(!this->isVisible())
            this->show();
    }
}

void MainWindow::HideIfNeeded()
{
    if (this->isVisible() && ui->checkBox_sw_hideInTray->isChecked()) {
        this->hide();
    }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        if (this->isVisible()) {
            this->hide();
        } else {
            this->show();
        }
    }
}

void MainWindow::quit() {
        acceptClose = true;
        close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (acceptClose) {
            writeSettings(&settings, false);
            event->accept();
    } else {
            hide();
            event->ignore();
    }
}

void MainWindow::hideEvent(QHideEvent *event) {
     writeSettings(&settings, false);
     event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    if(!is_normal && event->button() == Qt::LeftButton) {
        moving = true;
        offset = event->pos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    if(!is_normal && moving)
        this->move(event->globalPos() - offset);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    if(!is_normal && event->button() == Qt::LeftButton) {
        moving = false;
    }
}

bool MainWindow::GetRunAtStartup()
{
    bool bVal = false;

#ifdef Q_OS_WIN32
    //no access to HKEY_LOCAL_MACHINE
    QSettings autostart("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
    bVal = autostart.contains("FmStick");
#elif defined(Q_OS_LINUX)
    QString filePath(std::getenv("XDG_CONFIG_DIRS"));
    if (filePath.isEmpty()) {
        // XDG_CONFIG_DIRS is not defined, we'll use the default value
        // as mandated by http://standards.freedesktop.org/autostart-spec/autostart-spec-latest.html
        filePath = QDir::homePath() + QDir::separator() + ".config";
    }
    qDebug() << "User config dir is " << filePath;
    filePath = filePath + QDir::separator() + "autostart";
    qDebug()  << "User autostart dir is " << filePath;
    filePath = filePath + QDir::separator() + "FmStick.desktop";
    qDebug()  << "User autostart dir is " << filePath;

    QFile file(filePath);
    bVal = file.exists();

#elif defined(Q_OS_MAC)
    qDebug() << "todo!";
    /*
    QProcess *p = new QProcess;
    QString s = "defaults write /Library/Preferences/loginwindow AutoLaunchedApplicationDictionary -array-add '{ "Path" = "/Applications/FmStick/FmStick.app"; "Hide" = "0"; }'";
    p->start("bash", QStringList()<< "-c"<< s);
    */
    bVal = false;
#else
    qDebug() << "unsupported autostart for this system.";
#endif
    qDebug() << __FUNCTION__ << bVal;
    return bVal;
}


void MainWindow::SetRunAtStartup(int iVal)
{
    qDebug() << __FUNCTION__ << iVal;
#ifdef Q_OS_WIN32
    //no access to HKEY_LOCAL_MACHINE
    QSettings autostart("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
    if (iVal) {
        autostart.setValue("FmStick", QString("\"%1\"").arg(QDir::toNativeSeparators(QCoreApplication::applicationFilePath())));
    } else {
        autostart.remove("FmStick");
    }
    //autostart.sync();
#elif defined(Q_OS_LINUX)
    QString filePath(std::getenv("XDG_CONFIG_DIRS"));
    if (filePath.isEmpty()) {
        // XDG_CONFIG_DIRS is not defined, we'll use the default value
        // as mandated by http://standards.freedesktop.org/autostart-spec/autostart-spec-latest.html
        filePath = QDir::homePath() + QDir::separator() + ".config";
    }
    qDebug() << "User config dir is " << filePath;
    filePath = filePath + QDir::separator() + "autostart";
    qDebug()  << "User autostart dir is " << filePath;
    filePath = filePath + QDir::separator() + "FmStick.desktop";
    qDebug()  << "User autostart dir is " << filePath;

    QFile file(filePath);
    if (iVal && !file.exists()) {
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&file);
            out << "[Desktop Entry]\n";
            out << "Version=1.0\n";
            out << "Type=Application\n";
            out << "Name=FmStick\n";
            out << "Comment=FmStick Transmitter Control Application\n";
            out << QString("Exec='%1'\n").arg(QCoreApplication::applicationFilePath()).toAscii();
            out << "NoDisplay=false\n";
            out << "Hidden=false\n";

            // optional, as QFile destructor will already do it:
            file.close();
        }
    } else if(file.exists()) {
        file.remove();
    }
#if 0
    //QSettings autostart("autostart", "FmStick.desktop");
    //autostart.setPath(QSettings::IniFormat, QSettings::UserScope, "autostart/");
    QSettings autostart("$HOME/.config/autostart/FmStick.desktop",QSettings::IniFormat);
    autostart.setIniCodec("UTF-8");
    autostart.beginGroup("Desktop Entry");
    autostart.setValue("Version", "1.0");
    autostart.setValue("Type", "Application");
    autostart.setValue("Name", "FmStick");
    autostart.setValue("Comment", "FmStick Transmitter Control application");
    autostart.setValue("TryExec", "fmstick");
    autostart.setValue("Exec", "fmstick %F");
    autostart.endGroup();
    qDebug() << "path to config:" << autostart.fileName();
#endif

#elif defined(Q_OS_MAC)
    qDebug() << "todo!";
    /*
    QProcess *p = new QProcess;
    QString s = "defaults write /Library/Preferences/loginwindow AutoLaunchedApplicationDictionary -array-add '{ "Path" = "/Applications/FmStick/FmStick.app"; "Hide" = "0"; }'";
    p->start("bash", QStringList()<< "-c"<< s);
    */
#else
    qDebug() << "unsupported autostart for this system.";
#endif
}

void MainWindow::SaveRadioTextDialog()
{
    rt_popup->hide();
    ui->plainTextEdit_rds_rt->setPlainText(rt_editor->toPlainText());
}

void MainWindow::DisplayRadioTextDialog()
{
    //rt_editor->setPlainText(ui->plainTextEdit_rds_rt->toPlainText());
    rt_editor->document()->setHtml(rtplus_text);
    rt_popup->show();
}

void MainWindow::setupTrayIcon()
{

        rt_popup = new QDialog(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint );
        rt_popup->setFixedHeight(150);
        rt_popup->setFixedWidth(250);

        QPushButton *button_set = new QPushButton(tr("Set"));
        button_set->setIcon(QIcon(":/fmstick/images/1318446474_accept.png"));
        QPushButton *button_cancel = new QPushButton(tr("Cancel"));
        button_cancel->setIcon(QIcon(":/fmstick/images/cancel.png"));

        rt_editor = new QPlainTextEdit();

        QHBoxLayout *buttons_layout = new QHBoxLayout();
        buttons_layout->addStretch();
        buttons_layout->addWidget(button_set);
        buttons_layout->addWidget(button_cancel);

        QVBoxLayout *vert_layout = new QVBoxLayout(rt_popup);
        vert_layout->addWidget(rt_editor);
        vert_layout->addLayout(buttons_layout);


        connect(button_set, SIGNAL(clicked()), this, SLOT(SaveRadioTextDialog()));
        connect(button_cancel, SIGNAL(clicked()), rt_popup, SLOT(hide()));

        //trayIconMenu = new QMenu(this);
        trayIconMenu = new QMenu();
        trayIconMenu->addAction(ui->actionNormalize_Window);
        trayIconMenu->addAction(ui->actionChange_RadioText);
        trayIconMenu->addAction(ui->actionToggle_transmission);
        trayIconMenu->addAction(ui->actionExit);
        trayIcon = new QSystemTrayIcon(this);
        trayIcon->setContextMenu(trayIconMenu);
        trayIcon->setIcon(warningicon);
        trayIcon->show();
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}


void MainWindow::StationButtonContextMenu(const QPoint &point)
{
    qDebug() << __FUNCTION__;

    QMenu *menu = new QMenu;
    menu->addAction(ui->actionSkin_changeMode);
    menu->addAction(ui->actionMinimize_to_Tray);
    menu->addAction(ui->actionChange_RadioText);
    menu->addAction(ui->actionExit);

    //menu->addAction(QString("Test Item"), this, SLOT(test_slot()));
    menu->exec(ui->commandLinkButton_station->mapToGlobal(point));
}

void MainWindow::toggleWindowMode()
{
    is_normal = !is_normal;
    SetWindowMode(is_normal);
}

void MainWindow::SetWindowMode(bool normal)
{
    ui->tabWidget->setVisible(normal);
    ui->menuBar->setVisible(normal);
    if(toolBarIsVisible)
        ui->toolBar->setVisible(normal);
    else
        ui->toolBar->setVisible(false);
    ui->statusBar->setVisible(normal);

    if(normal) {
        ui->actionSkin_changeMode->setText(tr("Minimize UI Window"));
        ui->actionSkin_changeMode->setIcon(QIcon(":/fmstick/images/minimize.png"));
    } else {
        ui->actionSkin_changeMode->setText(tr("Normalize UI Window"));
        ui->actionSkin_changeMode->setIcon(QIcon(":/fmstick/images/maximize.png"));
    }

    /* all 3 cmds are needed to change size of the window */
    this->adjustSize();
    this->resize(1,1);
    this->updateGeometry();


    /* disable window buttons */
    Qt::WindowFlags flags;
#if 0
    flags = this->windowFlags();
    if(normal) {
        flags &= ~(Qt::FramelessWindowHint);
    } else {
        flags |= (Qt::FramelessWindowHint);
    }
#else
    if(normal) {
        flags = 0;
    } else {
        flags = Qt::CustomizeWindowHint;
        if(ui->checkBox_be_on_the_top->isChecked())
            flags |= Qt::WindowStaysOnTopHint;
    }
#endif
    this->setWindowFlags(flags);
    this->show();
    is_normal = normal;
}


void MainWindow::ShowAboutApp()
{
    QMessageBox::about ( this, tr("About FmStick"),
                         tr("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                            "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
                            "p, li { white-space: pre-wrap; }\n"
                            "</style></head>\n"
                            "<body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
                            "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                            "FmStick Control Application.<br />\n"
                            "Version: %1.<br />\n"
                            "Created using Qt4 libraries.<br />\n"
                            "<br />\n"
                            "Thanks VRTP Forum users for<br />\n"
                            "info and help with testing.<br />\n"
                            "<br />\n"
                            "&copy; tipok.org.ua<br />\n"
                            "License: GNU GPLv3<br />\n"
                            "Commercial licenses avaliable too."
                            "</p></body></html>").arg(CurrentVersion));
}

void MainWindow::UpdateRDSRTFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                    tr("Choose a text file to open"),
                    QDir::homePath(),
                    tr("Text files (*.txt);;Log Files (*.log);;All Files (*.*)"));
    if(fileName.isNull()) {
        qDebug() << "text file is not chosen";
        return;
    }
    qDebug() << "RadioText updater will use file:" << fileName;
    ui->lineEdit_rds_title_file->setText(fileName);
    querier->rdsRTFile = fileName;
}


void MainWindow::UpdateFirmware()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                    tr("Choose a firmware file to open"),
                    QDir::homePath(),
                    tr("Binary Files (*.bin)"));
    if(fileName.isNull()) {
        qDebug() << "firmware is not chosen";
        return;
    }
    qDebug() << "Opening firmware file:" << fileName;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this, tr("Can't open file"), tr("Can't open firmware file for reading"));
        return;
    }

    if(file.size() < 16 || file.size() > 6040) {
        QMessageBox::warning(this, tr("Bad Firmware"), tr("Bad firmware size!"));
        return;
    }

    QByteArray fwHeader = file.read(16);
    if(fwHeader[2] != (char) 0x12 ||
             fwHeader[3] != (char) 0x34) {
        QMessageBox::warning(this, tr("Bad Firmware"), tr("Bad firmware signature!"));
        return;
    }


    int ret = QMessageBox::question ( this, tr("Are you sure?"),
               tr("Do you really want to update device's firmware?\n"
                                             "Bad firmware may make device inoperable!"),
               QMessageBox::Yes|QMessageBox::Abort, QMessageBox::Abort);

    if(ret != QMessageBox::Yes) {
        qDebug() << "Aborted";
        return;
    }



    //QFrame* popup = new QFrame(this, Qt::Popup);
    QDialog* popup = new QDialog(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    QProgressBar *pbar = new QProgressBar(popup);

    pbar->setMinimum(0);
    pbar->setMaximum(file.size() / 10);
    pbar->setTextVisible(true);
    pbar->setFixedWidth(300);
    popup->show();

    int offset=0;
    while(offset < file.size()) {
        int to_write = file.size() - offset;
        if(to_write > 16)
            to_write=16;
        qDebug() << "writing " << to_write << "bytes to: " << offset << "/" << file.size();
        if(fmtxEEPROMWriteFwChunk((const uint8_t *) fwHeader.constData(), to_write, offset) != FMTX_MODE_OK){
            QMessageBox::warning(this, tr("Can't write firmware"), tr("Error while writing firmware!"));
            delete pbar;
            delete popup;
            return;
        }
        offset+=to_write;
        pbar->setValue(offset/10);
        fwHeader = file.read(16);
    }
    delete pbar;
    delete popup;
    qDebug() << "firmware updated successfuly";
    QMessageBox::information(this, tr("Success"), tr("Firmware updated successfuly!\n"
                                                     "Please unplug and plug FmStick\n"
                                                     "again to run new firmware"));
}




/**********************************************************
 * EEPROM READ/SAVE
 **********************************************************/
void MainWindow::SaveEEPROMParams()
{
    int ret;
    qDebug() << __FUNCTION__;

    ret = QMessageBox::question ( this, tr("Are you sure?"),
                                      tr("Do you really want to write EEPROM config?\n"
                                      "Bad configuration may make frontend inoperable!"),
               QMessageBox::Yes|QMessageBox::Abort, QMessageBox::Abort);

    if(ret != QMessageBox::Yes) {
        qDebug() << "Aborted";
        return;
    }

    bool is_ready_save = querier->is_ready;
    bool liveEditingEnabled_save = liveEditingEnabled;
    querier->is_ready=false;
    liveEditingEnabled=true;
    fmtxCmdSpecial = FMTX_SPECIAL_EEPROM;   ///< now work with eeprom instead of frontend
    fmtxEEPROMInfoClean();
    SetFrontendParams();
    ret = fmtxEEPROMWriteConfig(ui->checkBox_hw_run_txAtPowerUp->isChecked());
    fmtxCmdSpecial = FMTX_SPECIAL_FRONTEND;
    liveEditingEnabled = liveEditingEnabled_save;
    querier->is_ready=is_ready_save;

    if(ret != FMTX_MODE_OK){
        qDebug() << __FUNCTION__ << " write error!";
        QMessageBox::warning(this, tr("Can't write EEPROM"), tr("Error while writing EEPROM Config!"));
        return;
    }

    qDebug() << __FUNCTION__ << "saved!";
    QMessageBox::information(this, tr("Write successed"), tr("EEPROM Config written successfuly!\n"
                                                             "You may restart device to run\n"
                                                             "configuration from EEPROM."));
}

void MainWindow::ShowEEPROMParams()
{
    qDebug() << __FUNCTION__;
    querier->HidAccessMutex.lock();
    fmtxCmdSpecial = FMTX_SPECIAL_EEPROM;   ///< now work with eeprom instead of frontend
    GetFrontendParams();
    fmtxCmdSpecial = FMTX_SPECIAL_FRONTEND;
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetRunEepromCfg(bool bVal)
{
    if(bVal) {
        int buttonClicked =
                QMessageBox::question ( this,
                                        tr("Save all to EEPROM?"),
                                        tr("Do You wish also save current configuration into EEPROM?\n"
                                           "Push \"No\", if You want only toggle autostart switch"),
                                        QMessageBox::Save,
                                        QMessageBox::No
                                        );
        if(buttonClicked == QMessageBox::Save){
            qDebug() << "save full eeprom";
            fmtxCmdStatus = fmtxEEPROMSetStartupFlag(1, 0);
            SaveEEPROMParams();
        } else {
            qDebug() << "dont save full eeprom";
            fmtxCmdStatus = fmtxEEPROMSetStartupFlag(1, 1);
        }
    } else {
        int buttonClicked =
                QMessageBox::question ( this,
                                        tr("Disable autostart?"),
                                        tr("Do You really want to disable auto-starting transmitter from EEPROM?\n"
                                           "You will not be able to transmit until you run this application again."),
                                        QMessageBox::Yes,
                                        QMessageBox::No
                                        );
        if(buttonClicked == QMessageBox::Yes){
            qDebug() << "force un-run from eeprom";
            fmtxCmdStatus = fmtxEEPROMSetStartupFlag(0, 1);
            if(fmtxCmdStatus != FMTX_MODE_OK)
                qDebug() << "can't set eeprom run flag in full eeprom";

        } else {
            ui->checkBox_hw_run_txAtPowerUp->blockSignals(true);
            ui->checkBox_hw_run_txAtPowerUp->setChecked(true);
            ui->checkBox_hw_run_txAtPowerUp->blockSignals(false);
            qDebug() << "dont un-run from eeprom";
            return;
        }
    }

    if(fmtxCmdStatus != FMTX_MODE_OK)
        qDebug() << "can't set eeprom run flag";
}











QDir MainWindow::directoryOf(const QString &subdir)
{
    QDir dir(QApplication::applicationDirPath());
#if defined(Q_OS_WIN)
    if (dir.dirName().toLower() == "debug" || dir.dirName().toLower() == "release")
        dir.cdUp();
#elif defined(Q_OS_MAC)
    if (dir.dirName() == "MacOS") {
        dir.cdUp();
        dir.cdUp();
        dir.cdUp();
    }
#endif
    dir.cd(subdir);
    return dir;
}

void MainWindow::createLanguageMenu()
{
    QAction *action;
    languageMenu = new QMenu(this);
    languageActionGroup = new QActionGroup(this);
    connect(languageActionGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(switchLanguage(QAction *)));

    /* add auto-select of lang */
    action = new QAction(QString("&%1 %2").arg(1).arg(tr("Auto Select")), this);
    action->setCheckable(true);
    action->setData("auto");
    if (UsedLanguage == "auto")
        action->setChecked(true);
    languageMenu->addAction(action);
    languageActionGroup->addAction(action);

    /* add english translation (default in app) */
    action = new QAction(QString("&%1 %2").arg(2).arg("English"), this);
    action->setCheckable(true);
    action->setData("en");
    languageMenu->addAction(action);
    languageActionGroup->addAction(action);
    if (UsedLanguage == "en")
        action->setChecked(true);

    /* look for other translations */
#ifdef TRANSLATIONSPATH
    QDir qmDir = QDir(TRANSLATIONSPATH);
#else
    QDir qmDir = directoryOf("translations");
#endif
    QStringList fileNames = qmDir.entryList(QStringList("fmstick_*.qm"));
    for (int i = 0; i < fileNames.size(); ++i) {
        QString locale = fileNames[i];
        locale.remove(0, locale.indexOf('_') + 1);
        locale.chop(3);
        QTranslator translator;
        translator.load(fileNames[i], qmDir.absolutePath());
        QString language = translator.translate("MainWindow", "English");
        action = new QAction(tr("&%1 %2").arg(i + 3).arg(language), this);
        action->setCheckable(true);
        action->setData(locale);
        languageMenu->addAction(action);
        languageActionGroup->addAction(action);
        if (locale == UsedLanguage)
            action->setChecked(true);
    }
}

void MainWindow::switchLanguage(QAction *action)
{
    if(action)
        UsedLanguage = action->data().toString();
    qDebug() << __FUNCTION__ << "new lang: " << UsedLanguage;
    QString locale = UsedLanguage;
    if(UsedLanguage == "en") {
        qApp->removeTranslator(&appTranslator);
        qApp->removeTranslator(&qtTranslator);
        retranslateUi();
        return;
    } else if(UsedLanguage == "auto")
        locale = QLocale::system().name();

#ifdef TRANSLATIONSPATH
    QDir qmDir = QDir(TRANSLATIONSPATH);
#else
    QDir qmDir = directoryOf("translations");
#endif

    appTranslator.load("fmstick_" + locale, qmDir.absolutePath());
    qtTranslator.load("qt_" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(&appTranslator);
    qApp->installTranslator(&qtTranslator);
    retranslateUi();
}

void MainWindow::goToHomePage()
{
    qDebug() << __FUNCTION__;
    openBrowser("http://tipok.org.ua/node/35", this);
}

bool MainWindow::openBrowser(const QString& rUrl, QWidget* parent)
{
    bool result = false;
    QApplication::setOverrideCursor(Qt::BusyCursor);
#ifdef Q_WS_WIN
    qDebug() << __FUNCTION__ << "winbrowser";
    result = int(ShellExecuteW(parent->winId(),
            0,
            //rUrl.ucs2(),
            //rUrl.unicode(),
            (const WCHAR *) rUrl.utf16(),
            0,
            0,
            SW_SHOWNORMAL)) > 32;
#else
    qDebug() << __FUNCTION__ << "unixbrowser";
    Q_UNUSED(parent);
    // Try a range of browsers available on UNIX, until we (hopefully)
    // find one that works.  Start with the most popular first.

    QProcess process;
    bool is_started;

    QStringList environment = QProcess::systemEnvironment();
    process.setEnvironment(environment);

    /* use default browser */
    is_started = process.startDetached("x-www-browser", QStringList() << rUrl);
    //bool QProcess::startDetached ( const QString & program )
    qDebug() << __FUNCTION__ << is_started;
    qDebug() << __FUNCTION__ << process.processEnvironment().toStringList();

    if (!is_started) {
        is_started = process.startDetached("firefox", QStringList() << rUrl);
        qDebug() << __FUNCTION__ << is_started;
    }

    if (!is_started) {
        is_started = process.startDetached("mozilla", QStringList() << rUrl);
        qDebug() << __FUNCTION__ << is_started;
    }

    if (!is_started) {
        is_started = process.startDetached("konqueror", QStringList() << rUrl);
        qDebug() << __FUNCTION__ << is_started;
    }

    if (!is_started) {
        is_started = process.startDetached("netscape", QStringList() << rUrl);
        qDebug() << __FUNCTION__ << is_started;
    }

    if (!is_started) {
        is_started = process.startDetached("chrome", QStringList() << rUrl);
        qDebug() << __FUNCTION__ << is_started;
    }

    if (!is_started) {
        is_started = process.startDetached("safari", QStringList() << rUrl);
        qDebug() << __FUNCTION__ << is_started;
    }

    result = is_started;
#endif
    QApplication::restoreOverrideCursor();

    return result;
}

void MainWindow::toggleToolBar()
{
    toolBarIsVisible = !toolBarIsVisible;
    ui->toolBar->setVisible(toolBarIsVisible);
    this->adjustSize();
    this->resize(1,1);
    this->updateGeometry();

}
