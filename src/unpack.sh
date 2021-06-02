#!/bin/dash
cd dump
ndstool -x ../baserom.nds -9 arm9.bin -9i arm9i.bin -7 arm7.bin -7i arm7i.bin -y9 y9.bin -y7 y7.bin -d data -y overlay -t banner.bin -h header.bin
