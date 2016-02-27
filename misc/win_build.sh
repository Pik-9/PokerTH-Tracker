#!/bin/bash

# Daniel Steinhauer (9 of Spades)
# This is just a quick and dirty script to compile this app for Windows.

cd $(dirname $0)
rm -rf ../win_build
mkdir ../win_build
cd ../win_build

lrelease ../PokerTH_tracker.pro
i686-w64-mingw32.static-qmake-qt5 ../PokerTH_tracker.pro
make
mkdir data
mv ../Lang/*.qm data/
cp ../Icons/*.png data/
zip -j PokerTH-Tracker_win32.zip release/PokerTH_tracker.exe
zip PokerTH-Tracker_win32.zip data/*
rm -rf data