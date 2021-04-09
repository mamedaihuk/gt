CC=clang

all: man

man:
	gzip -k gt.1

install:
	cp gt.1.gz /usr/local/share/man/man1

uninstall:
	rm /usr/local/share/man/man1/gt.1.gz

clean:
	rm gt.1.gz
