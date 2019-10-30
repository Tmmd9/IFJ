CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Wextra -g

all: scanner

str.o: str.c str.h
	$(CC) $(CFLAGS) -c str.c -o $@

scanner.o: scanner.c scanner.h error.h
	$(CC) $(CFLAGS) -c scanner.c -o $@

scanner: str.o scanner.o
	$(CC) $(CFLAGS) str.o scanner.o -o $@

clean:
	rm -f *.o