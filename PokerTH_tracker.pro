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

INSTALLS += target iconset

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h
