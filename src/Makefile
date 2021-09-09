SHELL=/bin/zsh
SCRIPTS=./scripts/misc
SDATTOOL=./scripts/SDATTOOL

all: pre post

pre:
	${SCRIPTS}/pre

post: build
	${SCRIPTS}/repack

build: text sdat

text:
	${SCRIPTS}/text

sdat:
	${SDATTOOL}/__main__.py -ns -b assets/sound_data.sdat assets/sound_data

clean:
	rm -rf data assets build pak_rom.nds
