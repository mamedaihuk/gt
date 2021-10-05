include ${DEVKITARM}/ds_rules

ifndef ELF
	ELF=${NAME}.elf
endif

NAME		:=	${shell basename ${CURDIR}}
OBJ		:=	${NAME}.o
CC		:=	arm-none-eabi-gcc
CFILES		:=	*.c
CFLAGS		:=	${CLIBS} -g -Wall -Ofast -march=armv5te \
			-mtune=arm946e-s -fomit-frame-pointer -ffast-math \
			-mthumb -mthumb-interwork -DARM9
CLIBS		:=	-I${DEVKITPRO}/libnds/include
LDFLAGS		:=	-g -mthumb -mthumb-interwork ${OBJ} -L${DEVKITPRO}/libnds/lib -lnds9

alL:	${NAME}

${NAME}:
	${CC} ${CFLAGS} -c ${CFILES} -o ${OBJ}
	${CC} ${LDFLAGS} -o ${ELF}

clean:
	${RM} ${OBJ} ${ELF}
