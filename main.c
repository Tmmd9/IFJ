/*
 *	Predmet : Formální jazyky a překladače									 
 *	Projekt : Implementace překladače imperativního jazyka IFJ19			 
 *	Súbor	: main.c													 
 *				Main file pre interpret IFJ19	 
 *	Tím		: číslo 10 / varianta II										 
 *	Autori	: Dominik Boboš (xbobos00)										 
 *			  Peter Hudeček (xhudec34)										 
 *			  Timotej Kováčik (xkovac49)									 
 *			  Dávid Oravec (xorave05)										 
 *																			 
 */


#include <stdio.h>
#include <stdlib.h>

#include "scanner.h" // temporary for bridging source file
#include "error.h"

int main()
{
	FILE* source;

//#ifdef DEBUG    	harmim things, maybe only David understand that
	if (!(source = fopen("test.ifj", "r")))
	{
		source = stdin;
	}
//#else				harmim things, maybe only David understand that
	//source = stdin;
//#endif			harmim things, maybe only David understand that

	set_source(source);

	return SYNTAX_OK;
}