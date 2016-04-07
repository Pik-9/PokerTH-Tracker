#-------------------------------------------------
#
# Project created by QtCreator 2015-12-28T17:40:02
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

iconset.path = /usr/local/share/PokerTH_tracker
iconset.files = Icons/PokerTH_Tracker.png

TARGET = PokerTH_tracker
TEMPLATE = app

RESOURCES += Icons.qrc Trans_qmake.qrc
RC_ICONS = Icons/PokerTH_Tracker.ico

target.path = /usr/local/bin

SOURCES += src/main.cpp\
        src/global.cpp\
        src/mainwindow.cpp\
        src/PlayerStat.cpp\
        src/multiview.cpp\
        src/analysis_widget.cpp

HEADERS  += src/mainwindow.hpp\
        src/global.hpp\
        src/PlayerStat.hpp\
        src/multiview.hpp\
        src/analysis_widget.hpp

TRANSLATIONS = Lang/PokerTH_tracker_de.ts

desktop.path = /usr/share/applications
desktop.files += PokerTH_tracker.desktop

INSTALLS += target iconset desktop