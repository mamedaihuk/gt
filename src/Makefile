include ${DEVKITARM}/ds_rules

ifndef ELF
	ELF=${NAME}.elf
endif

NAME=gt
OBJ=${NAME}.o
CC=arm-none-eabi-gcc
CFILES=*.c
CFLAGS=${CLIBS} -g -Wall -Ofast -march=armv5te -mtune=arm946e-s -fomit-frame-pointer -ffast-math -mthumb -mthumb-interwork -DARM9
CLIBS=-I${DEVKITPRO}/libnds/include
LDFLAGS=-specs=ds_arm9.specs -g -mthumb -mthumb-interwork ${OBJ} -L${DEVKITPRO}/libnds/lib -lnds9

all:
	${CC} ${CFLAGS} -c ${CFILES} -o ${OBJ}
	${CC} ${LDFLAGS} -o ${ELF}

clean:
	${RM} ${OBJ} ${ELF}
