#!/bin/bash

CURRDIR=`pwd`

rm -rf staging/*
rm -rf *.xo

cd ../bobot/lib
make

cd $CURRDIR
cp -Lr Butialo.activity -t staging

cd staging/Butialo.activity/bobot/drivers/
rm boot.lua buzzer.lua debug.lua ledA.lua ledR.lua ledV.lua \
	leds.lua motorin.lua motorTm.lua move.lua puerta.lua sec.lua motor.lua \
	stmtr.lua temp_lubot.lua display.lua

cd $CURRDIR
cd staging/Butialo.activity
python setup.py fix_manifest
python setup.py dist_xo

cd $CURRDIR
mv staging/Butialo.activity/dist/*.xo .


