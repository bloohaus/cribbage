CFLAGS=-Wall -g
LFLAGS=-lncurses -lmenu

cribbage:
	gcc $(CFLAGS) $(LFLAGS) cribbage.c -o cribbage