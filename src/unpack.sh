#!/bin/dash
mkdir assets
ndstool -x baserom.nds -d assets/data -o assets/banner.bmp
quickbms -Y ./cpac.bms ./assets/data/cpac_2d.bin assets/cpac_2d
quickbms -Y ./cpac.bms ./assets/data/cpac_3d.bin assets/cpac_3d
