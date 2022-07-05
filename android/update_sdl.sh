#!/bin/bash

SRC_DIR='../extlib/sdl/android-project/app/src/main/java/org/libsdl/app'
DEST_DIR='src/main/java/org/libsdl/app'
TMP="$(mktemp)"
SED_LINE='s/SDLActivity\.nativeRunMain(.*);/MainActivity.nativeRunMain();/'
IMPORT_LINE='import net.leezh.taleweaver.MainActivity;'

rm $DEST_DIR/*
cp $SRC_DIR/* $DEST_DIR
sed $SED_LINE $DEST_DIR/SDLActivity.java > $TMP
head -n2 $TMP > $DEST_DIR/SDLActivity.java
echo -n $IMPORT_LINE >> $DEST_DIR/SDLActivity.java
tail -n+2 $TMP >> $DEST_DIR/SDLActivity.java