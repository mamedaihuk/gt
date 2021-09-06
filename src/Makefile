.ONESHELL:

SHELL=/bin/zsh

all: pre post

pre: unpack assets

post: build repack

unpack:
	ndstool -x baserom.nds -d data

assets:
	#decode text
	./scripts/SDATTool/__main__.py -o -u data/sound_data.sdat assets/sound_data

build:
	./scripts/encode.py -i ${file} -o ${file:r}
	./scripts/SDATTool/__main__.py -o -b assets/sound_data build/sound_data.sdat

repack:
	ndstool -c pak_rom.nds -9 build/arm9.bin -9i build/arm9i.bin -7 build/arm7.bin -7i build/arm7i.bin -y9 build/y9.bin -y7 build/y7.bin -d build/data -y build/overlay -t build/banner.bin -h build/header.bin -o build/banner.bmp

clean:
	rm -rf data assets build
