.ONESHELL:

SHELL=/bin/zsh

all: pre post

pre: unpack assets

post: build repack

unpack:
	ndstool -x baserom.nds -d data

assets:
	for d in data/st01; do (for file in *.xml; do ./scripts/decode.py -i ${file} -o ${file}.txt; done); done

build:
	./scripts/encode.py -i ${file} -o ${file:r}

repack:
	ndstool -c pak_rom.nds -d build/data

clean:
	rm -rf data assets build
