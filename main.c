/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: main.c
 *				-hlavný spúšťací modul IFJ19
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */

#include <stdio.h>
#include "generator.h"
#include "parser.h"

int main(void) {
    FILE *source = stdin;
    int result = 0;


    sourceFile(source);
    result = parse();
    writeCodeToStdout();


    return result;
}