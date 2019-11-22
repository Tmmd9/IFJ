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

/* 	pre pomocnu funkciu addToHash	*/
#define LEFT 0
#define CURRENT 1
#define RIGHT 2

#include "scanner.h" //ziskavanie tokenov
#include "stack.h" //indent/dedent + neviem co vsetko
#include "error.h" 
#include "expr.h" //precedencna tabulka pre semantiku
#include "symtable.h"	//tabulka symbolov
#include <stdbool.h>

//struktura co bude uchovavat vsetko dolezite pre parser
typedef struct {
    htab globalT;      	//na tabulku globalnych symbolov (identifikatorov etc)
    htab localT;        //na tabulku localnych symbolov (identifikatorov etc)

	token Token;
	/*
	*	veci potrebne k tvorbe stromu
	*/
	Data * currentID;		//momentalne spracovavane
	Data * leftID;			//lava strana premennej
	Data * rightID;			//prava strana funkcie, vyrazu

    // ked checkujem parametre, tak index toho ktory momentalne spracovavam
	unsigned paramIndex;
	int uniqLabel;			//pre asembly napr. pre rozlisenie while1, while2 a tak :)
	int deepLabel;			//zanorenie

    unsigned in_declaration;        //bool ci sa momentalne nachadzam v deklaracii funkcie
    unsigned not_declared_function; //bool ci sa nachadzam este v nedeklarovanej funkci
    unsigned in_function;
    unsigned in_if;
    unsigned in_while;

  	//  tStack *stack;                  
	//dunno if necessary ale tak kvoli INDENTOM pre 
	//scanner mal by si to skener sam initnut
} ParserData;

//Main parsera
int parse();