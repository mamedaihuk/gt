#!/bin/dash
############################################################
# munkki	Build system for gt
# Copyright (C) 2021, fm'latghor <leocoogan@tutanota.com>
# SPDX		MIT
############################################################
NAME="munkki"
PROG="mk"
VERSION="0.1"
ASSETS="./assets"
BASE="./baserom.nds"
BUILD="./build"
DATA="./data/"
OUT="./gt.nds"
UTILS="./utils"
SDATTOOL="$UTILS/SDATTool/SDATTool/__main__.py"
TREADER="$UTILS/ghost-treader -c -s"

pre() {
	if [ -z "baserom.nds" ]; then
		printf "Missing %s\n" "$BASE"
	else
		if [ -d "$ASSETS" ]; then
			printf "%s already exists. Use '$PROG clean' to remove generated outputs.\n" "$ASSETS"
			return 1
		else
			mkdir data
			ndstool -x $BASE -d $DATA -o $DATA/banner.bmp >/dev/null
			# echo "Unpacked $ROM"

			find $DATA -type f -name "*.xml.lz" | xargs lzss -d

			$SDATTOOL -o -u $DATA/sound_data.sdat $ASSETS/sound_data >/dev/null
			# return 0
			# Unsure whether I need to return a value since the programs will
			# return their own and there may be varying degrees of success.
		fi
	fi
}

post () {
	text
	sdat
	src
	# repack
}

text () {
	# Encode text
	find $ASSETS -type f -name "*.xml" | xargs lzss -e
	# repack
}

sdat () {
	$SDATTOOL -ns -b $ASSETS/sound_data.sdat $ASSETS/sound_data >/dev/null
	# repack
}

src () {
	make -C src
}

# Kinda borked at the moment
repack() {
	ndstool \
	-c $OUT \
	-9 $BUILD/arm9.bin \
	-9i $BUILD/arm9i.bin \
	-7 $BUILD/arm7.bin \
	-7i $BUILD/arm7i.bin \
	-y9 $BUILD/y9.bin \
	-y7 $BUILD/y7.bin \
	-d $BUILD/data \
	-y $BUILD/overlay \
	-t $BUILD/banner.bin \
	-h $BUILD/header.bin \
	-o $BUILD/banner.bmp \
	-g BGTE 08 GHOSTTRICK-E $VERSION
}

clean() {
	rm -rf $DATA $ASSETS $BUILD $OUT
}

help() {
	printf "$PROG - $NAME (gt $VERSION), build system for gt
		\rusage: \033[1m"$PROG"\033[0m [options] \033[4m"target"\033[0m

		\r\033[1m"Options"\033[0m:
		\r  -h, --help	prints this help message
		\r  -f, --force	ignores if $ASSETS is full
		
		\r\033[1m"Targets"\033[0m:
		\r  pre		unpack the ROM and extract its assets
		\r  post	\tbuild and repack the ROM
		\r    sdat	repack the sound data
		\r    src	\trepack the binaries
		\r    text	repack the text data
		\r  clean	\tremove all generated files and directories\n"
}

main() {
	pre text sdat repack
}

if [ "_$1" = "_" ]; then
	main
fi

while [ ! $# -eq 0 ]
	do case "$1" in
		--help | -h )
			help
			;;
		--force | -f ) #Improve
			clean
			main
			;;
		pre )
			pre
			;;
		post )
			post
			;;
		clean )
			clean
			;;
		sdat )
			sdat
			;;
		text )
			text
			;;
		src )
			src
			;;
		esac
	shift
done
