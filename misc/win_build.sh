#!/bin/bash

# Daniel Steinhauer (9 of Spades)
# This is just a quick and dirty script to compile this app for Windows.

# The publisher's PGP key to sign with
PGP_KEY=992E9547

cd $(dirname $0)
rm -rf ../win_build
mkdir ../win_build
cd ../win_build

i686-w64-mingw32.static-qmake-qt5 ../PokerTH_tracker.pro
make
gpg2 -bu $PGP_KEY release/PokerTH_tracker.exe
zip -j PokerTH-Tracker_win32.zip release/PokerTH_tracker.exe release/PokerTH_tracker.exe.sig ../README.txt