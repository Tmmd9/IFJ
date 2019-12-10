#                                                                   \
 	Predmet : Formální jazyky a překladače                          \
 	Projekt : Implementace překladače imperativního jazyka IFJ19    \
 	Súbor	: Makefile                                              \
 				-makefile pre zostavenie projektu IFJ19             \
	Tím		: číslo 10 / varianta II                                \
 	Autori	: Dominik Boboš (xbobos00)                              \
 			  Peter Hudeček (xhudec34)                              \
 			  Timotej Kováčik (xkovac49)                            \
 			  Dávid Oravec (xorave05)                               \



CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Wextra -g

all: IFJ19

#IS_IT_OK_DIR = is_it_ok_test
#IS_IT_OK_SCRIPT = ./tests/is_it_ok.sh

expr.o: expr.c expr.h parser.h symstack.h scanner.h error.h generator.h
	$(CC) $(CFLAGS) -c expr.c -o $@

generator.o: generator.c generator.h str.h parser.h scanner.h
	$(CC) $(CFLAGS) -c generator.c -o $@

main.o: main.c generator.h parser.h
	$(CC) $(CFLAGS) -c main.c -o $@

parser.o: parser.c parser.h scanner.h stack.h error.h symtable.h expr.h generator.h
	$(CC) $(CFLAGS) -c parser.c -o $@

scanner.o: scanner.c scanner.h error.h str.h stack.h
	$(CC) $(CFLAGS) -c scanner.c -o $@

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c -o $@

str.o: str.c str.h
	$(CC) $(CFLAGS) -c str.c -o $@

symstack.o: symstack.c symstack.h expr.h symtable.h
	$(CC) $(CFLAGS) -c symstack.c -o $@

symtable.o: symtable.c symtable.h str.h
	$(CC) $(CFLAGS) -c symtable.c -o $@

IFJ19: expr.o generator.o main.o parser.o scanner.o stack.o str.o symstack.o symtable.o
	$(CC) $(CFLAGS) expr.o generator.o main.o parser.o scanner.o stack.o str.o symstack.o symtable.o -o $@

pack:
	zip xbobos00.zip *.c *.h Makefile README.md rozdeleni *.pdf

clean:
	rm -f *.o xbobos00.zip *.pdf