NAME=lzss
CC=clang
CFILES=*.c

all: ${NAME}

${NAME}:
	$(CC) $(CFILES) -o ${NAME}

clean:
	rm $(NAME) $(MAN)

install:
	cp $(NAME) /usr/local/bin

uninstall:
	rm /usr/local/bin/${NAME}
