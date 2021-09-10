SCRIPTS=./scripts/misc

all: pre post

pre:
	${SCRIPTS}/pre

post: build
	${SCRIPTS}/repack

repack:
	${SCRIPTS}/repack

build: text sdat

text:
	${SCRIPTS}/text

sdat:
	${SCRIPTS}/sdat

clean:
	${SCRIPTS}/clean
