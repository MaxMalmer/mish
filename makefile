# Makefile for Laboration 3 5DV088 HT19
# Max Malmer, c17mmr@cs.umu.se
# Version 1.0, 2019-09-24

CC = gcc
CFLAGS = -std=gnu11 -g -Wall -Wextra -Werror -Wmissing-declarations -Wmissing-prototypes -Werror-implicit-function-declaration -Wreturn-type -Wparentheses -Wunused -Wold-style-definition -Wundef -Wshadow -Wstrict-prototypes -Wswitch-default -Wunreachable-code

all:			mish

mish:			mish.o parser.o execute.o sighant.o
	$(CC) mish.o parser.o execute.o sighant.o -o mish

mish.o:			mish.c mish.h parser.h execute.h
	$(CC) $(CFLAGS) -c mish.c

execute.o:		execute.c execute.h parser.h
	$(CC) $(CFLAGS) -c execute.c

parser.o: 		parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

sighant.o: 		sighant.c sighant.h
	$(CC) $(CFLAGS) -c sighant.c

run:			mish
	./mish

valgrind: 		mish
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./mish

clean:
	rm -f mpasswdsort *.o core
