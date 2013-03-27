#-------------------------------------------------
#
# Project created by QtCreator 2011-10-05T01:35:53
#
#-------------------------------------------------
include(qtsingleapplication/common.pri)
include(qtsingleapplication/src/qtsingleapplication.pri)
include(QtWebApp-v1.2.9/lib/bfHttpServer/src/bfHttpServer.pri)

QT       += core gui

TEMPLATE = app

CODECFORTR = UTF-8
TRANSLATIONS += translations/fmstick_ru.ts \
                translations/fmstick_uk.ts

macx {
    TARGET = FmStick
} else {
    TARGET = fmstick
}

# disable debug messages in release
CONFIG(debug, debug|release) {
    # Define version string (see below for releases)
    VER = 1.1dbg
} else {
    DEFINES += QT_NO_DEBUG
    DEFINES += QT_NO_DEBUG_OUTPUT
    VER = 1.1
}

# Tip from: http://www.qtcentre.org/wiki/index.php?title=Version_numbering_using_QMake
VERSTR = '\\"$${VER}\\"'          # place quotes around the version string
DEFINES += VERSION=\"$${VERSTR}\" # create a VERSION macro containing the version string

SOURCES +=\
    mainwindow.cpp \
    main.cpp \
    logging.c \
    fmtx.c \
    bistream.c \
    querier.cpp \
    mainwindow_cfg.cpp \
    requestmapper.cpp \
    controller/txcontroller.cpp \
    controller/rdscontroller.cpp \
    controller/audiocontroller.cpp \
    controller/tableitem.cpp \
    controller/hardwarecontroller.cpp \
    controller/softwarecontroller.cpp \
    mainwindow_audio.cpp \
    mainwindow_tx.cpp \
    mainwindow_rds.cpp \
    mainwindow_http.cpp \
    mainwindow_helpers.cpp \
    mainwindow_actions.cpp

mac: SOURCES += hidmac.c
win32: SOURCES += hidwin.cpp
linux-g++*: SOURCES += hid-libusb.c

HEADERS +=\
    mainwindow.h \
    hidapi.h \
    logging.h \
    commanddefs.h \
    propertydefs.h \
    fmtx.h \
    querier.h \
    bitstream.h \
    requestmapper.h \
    controller/txcontroller.h \
    controller/rdscontroller.h \
    controller/audiocontroller.h \
    controller/tableitem.h \
    controller/hardwarecontroller.h \
    controller/softwarecontroller.h

RESOURCES += \
    fmstick.qrc

FORMS    += mainwindow.ui

mac:LIBS += /System/Library/Frameworks/CoreFoundation.framework/CoreFoundation \
    /System/Library/Frameworks/IOKit.framework/Versions/A/IOKit
win32:LIBS += -lsetupapi

# libusb-1.0 on Linux uses pkg-config
linux-g++* {
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0
}

win32 {
    # application icon on Windows
    RC_FILE = fmstick.rc
} else:macx {
    # app icon on OSX
    ICON = images/fmstick.icns
}

OTHER_FILES += AUTHORS.txt LICENSE.txt NEWS.txt README.txt \
    fmstick.rc \
    images/fmstick.ico \
    translations/fmstick_ru.ts \
    translations/fmstick_uk.ts \
    html/style.css \
    html/index.html \
    html/index.js \
    html/grayscale.svg

# To build qm translation files
!isEmpty(TRANSLATIONS) {
    isEmpty(QMAKE_LRELEASE) {
        win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
        else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    }
    isEmpty(TS_DIR):TS_DIR = translations
    TSQM.name = lrelease ${QMAKE_FILE_IN}
    TSQM.input = TRANSLATIONS
    TSQM.output = $$TS_DIR/${QMAKE_FILE_BASE}.qm
    TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN}
    TSQM.CONFIG = no_link
    QMAKE_EXTRA_COMPILERS += TSQM
    PRE_TARGETDEPS += compiler_TSQM_make_all
} else:message(No translation files in project)

unix {
  #VARIABLES
  isEmpty(PREFIX) {
    PREFIX = /usr
  }
  BINDIR = $$PREFIX/bin
  DATADIR =$$PREFIX/share

  DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"


  target.path =$$BINDIR

  desktop.path = $$DATADIR/applications
  desktop.files += $${TARGET}.desktop

  icon64.path = $$DATADIR/icons/hicolor/64x64/apps
  icon64.files += images/$${TARGET}.png

  i18n.path = $$DATADIR/$${TARGET}/translations
  i18n.extra = -$(INSTALL_FILE) translations/*.qm $(INSTALL_ROOT)$${i18n.path}
  i18n.uninstall = -$(DEL_FILE) -r $(INSTALL_ROOT)$${i18n.path}/*.qm
  TRSTR = '\\"$${i18n.path}\\"'          # place quotes around the version string
  DEFINES += TRANSLATIONSPATH=\"$${TRSTR}\"

  #MAKE INSTALL
  INSTALLS += target desktop icon64 i18n

}
