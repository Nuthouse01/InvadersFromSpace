# microAptiv_UP makefile for MIPSfpga

CC = gcc
LD = gcc
OD = objdump
OC = objcopy
SZ = size

CFLAGS  = -DSDL2 -g -O0 -Wall -Wextra -std=c99 -I/usr/include/SDL2
LDFLAGS = -lSDL2 -lSDL2_image

CSOURCES= \
main.c \
game.c \
wave.c \
util.c \
load_sprites.c \
interface.c

COBJECTS = $(CSOURCES:.c=.o)

all: program.elf $(CASMS)

program.elf : $(AOBJECTS) $(COBJECTS)
	$(LD) $(COBJECTS) $(LDFLAGS) -o sdl.elf

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.S.o:
	$(CC) -c $(CFLAGS) $< -o $@

.c.s:
	$(CC) -S $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f sdl.elf

run:
	./sdl.elf
