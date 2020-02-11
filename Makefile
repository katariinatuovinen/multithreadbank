CC=gcc
CFLAGS=-Wall -pedantic -pthread -std=c99 -g -D_POSIX_C_SOURCE
LIB=ar rcs

all: cli srv

srv: srv.c libparser.a	parserlib.h log.h
	$(CC) $(CFLAGS) -o srv srv.c log.c parserlib.c -L. -lparser

cli: cli.c
	$(CC) $(CFLAGS) -o cli cli.c

parserlib.o: parserlib.c
	$(CC) $(CFLAGS) -c parserlib.c

libparser.a: parserlib.o
	$(LIB) libparser.a parserlib.o && rm -f parserlib.o

clean:
	rm -f srv cli *.o *.a
