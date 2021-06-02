CC=clang

patch:
	xdelta -dfs baserom.nds patch.xd baserom.nds

clean:
	rm -rf build dump *.gz

rebuild: pre-build patch dd banner man build

all: pre-build man banner build

pre-build:
	mkdir build

build:
	ndstool -c baserom.nds -9 ./build/arm9.bin -9i ./build/arm9i.bin -7 ./build/arm7.bin -7i ./build/arm7i.bin -y9 ./build/y9.bin -y7 ./build/y7.bin -d ./build/data -y overlay -t ./build/banner.bin -h ./build/header.bin

banner:
	./banner.py

dd: dump deobfuscate

dump:
	mkdir dump
	ndstool -x baserom.nds -9 ./dump/arm9.bin -9i ./dump/arm9i.bin -7 ./dump/arm7.bin -7i ./dump/arm7i.bin -y9 ./dump/y9.bin -y7 ./dump/y7.bin -d ./dump/data -y overlay -t ./dump/banner.bin -h ./dump/header.bin

deobfuscate:
	cd dump
	for d in ./*/; do (for file in *.xml.lz; do nlzss "$file" > "${file:r}"; done); done
	arm9dec arm9.bin arm9_decompressed

man:
	gzip -k gt.1

install:
	cp gt.1.gz /usr/local/share/man/man1

uninstall:
	rm /usr/local/share/man/man1/gt.1.gz

clean:
	rm gt.1.gz
