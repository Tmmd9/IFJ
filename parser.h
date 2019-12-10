/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: parser.h
 *				-hlavičkový súbor k modulu parser.c
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */
#ifndef _PARSER_H
#define _PARSER_H

#include "scanner.h"
#include "stack.h"
#include "error.h"
#include "symtable.h"
#include <stdbool.h>


/* 	pre pomocnu funkciu addToHash udáva position */
#define LEFT 0
#define CURRENT 1
#define RIGHT 2

//struktura co bude uchovavat vsetko dolezite pre parser
typedef struct {
    htab globalT;      	//na tabulku globalnych symbolov (identifikatorov etc)
    htab localT;        //na tabulku localnych symbolov (identifikatorov etc)

	token Token;

	Data * currentID;		//momentalne spracovavane
	Data * leftID;			//lava strana premennej
	Data * rightID;			//prava strana funkcie, vyrazu

    // ked checkujem parametre, tak index toho ktory momentalne spracovavam
	unsigned paramIndex;
	int uniqLabel;			//pre asembly napr. pre rozlisenie while1, while2 a tak :)

    unsigned in_declaration;        //bool ci sa momentalne nachadzam v deklaracii funkcie
    unsigned was_return;
    unsigned in_function;
    unsigned in_if;
    unsigned in_while;

} ParserData;

//Main parsera
int parse();

#endif // _PARSER_H