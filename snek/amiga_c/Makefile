# Makefile for building on the Amiga.
# This works best with ADE (available on Aminet).
CC=gcc
CFLAGS=-O2
LDFLAGS=-noixemul -s
OBJS=audio.o game.o list.o main.o vec2.o view.o

amisnek: ${OBJS}
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJS): %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f amisnek *.o
