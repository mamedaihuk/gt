.POSIX:
	ifndef PREFIX
		PREFIX = /usr/local
	endif

NAME=lzss
CC=clang
CFILES=*.c
CFLAGS=-Wall -Wextra -02
all: ${NAME}

${NAME}:
	${CC} ${CFILES} -o ${NAME}

clean:
	${RM} ${NAME}

install:
	cp ${NAME} ${DESTDIR}${PREFIX}/bin

uninstall:
	${RM} ${DESTDIR}${PREFIX}/bin/${NAME}
