/*
 *	Predmet : Formální jazyky a překladače									 
 *	Projekt : Implementace překladače imperativního jazyka IFJ19			 
 *	Súbor	: scanner.h														 
 *				-hlavičkový súbor pre náš scanner (lexikálny analyzátor)	 
 *	Tím		: číslo 10 / varianta II										 
 *	Autori	: Dominik Boboš (xbobos00)										 
 *			  Peter Hudeček (xhudec34)										 
 *			  Timotej Kováčik (xkovac49)									 
 *			  Dávid Oravec (xorave05)										 
 *																			 
 */

#ifndef _SCANNER_H_
#define _SCANNER_H_


typedef enum {
	STATE_START,				  // startovaci stav
	STATE_ERROR = 1,			  // lexikalna chyba
	STATE_ID,					  // konecnt stav
	STATE_KEYWORD,				  // konecny stav
	STATE_INT,					  // konecny stav intu
	STATE_FLOAT,				  // konecny stav FLOAT
	STATE_FLOAT_POINT,			  // medzistav float + desatinna ciarka
	STATE_FLOAT_POINT_NUMBER,	  // medzistav float + des. ciarka + cislo
	STATE_FLOAT_EXP,			  // medzistav float a exponent
	STATE_FLOAT_EXP_OP,			  // medzistav napr. 2^-e or 2^+e
	STATE_STRING,				  // konecny stav
	STATE_START,				  // medzistav, zacina string
	STATE_BACKSLASH,			  // medzistav 
	STATE_BACKSLASH_NUMBER,		  // medzistav na hexakod
	STATE_PLUS,					  // konecny stav operatoru +
	STATE_MINUS,				  // konecny stav operatoru -
	STATE_MULTIPLY,				  // konecny stav operatoru *
	STATE_DIVIDE,				  // konecny stav operatoru /
	STATE_DIVIDE_INT,
	STATE_GREATER_THAN,
	STATE_LESS_THAN,
	STATE_GREATER_EQUAL,
	STATE_LESS_EQUAL,
	STATE_EQUAL,
	STATE_NOT_EQUAL,
	STATE_COMMENT,
	STATE_LEFT_PAR,
	STATE_RIGHT_PAR,
	//TODO
} state;

typedef struct {
	//TODO
} token;

token getNextToken();

#endif
