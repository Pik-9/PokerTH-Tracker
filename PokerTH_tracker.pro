#-------------------------------------------------
#
# Project created by QtCreator 2015-12-28T17:40:02
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

iconset.path = /usr/local/share/PokerTH_tracker
iconset.files = Icons/*

TARGET = PokerTH_tracker
TEMPLATE = app

target.path = /usr/local/bin

SOURCES += main.cpp\
        global.cpp\
        mainwindow.cpp\
        PlayerStat.cpp\
        multiview.cpp\
        analysis_widget.cpp

HEADERS  += mainwindow.hpp\
        global.hpp\
        PlayerStat.hpp\
        multiview.hpp\
        analysis_widget.hpp

TRANSLATIONS = Lang/PokerTH_tracker_de.ts

qm_files.path = /usr/local/share/PokerTH_tracker
qm_files.files = Lang/*.qm

desktop.path = /usr/share/applications
desktop.files += PokerTH_tracker.desktop

INSTALLS += target iconset qm_files desktop