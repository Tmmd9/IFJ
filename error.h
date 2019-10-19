/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	S�bor	: error.h
 *				-hlavičkový súbor pre error kódy
 *	T�m		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */

#ifndef _ERROR_H_
#define _ERROR_H_

#define TOKEN_OK 0
#define SYNTAX_OK 0
#define ERROR_SCANNER 1					//chyba v lex.analyze
#define ERROR_PARSER 2					//chyba v syntakt. analyze
#define ERROR_PROGRAM_SEMANTIC 3		//chyba semantiky (nedefinovana var,.)
#define ERROR_ARTIHMETIC 4				//semanticka aritmeticka chyba
#define ERROR_WRONG_NUMBER_OF_PARAMS 5	//semanticka chyba(zly pocet param.)
#define ERROR_SEMANTIC_OTHERS 6			//ine semanticke chyby
#define ERROR_DIVIDING_ZERO 9			//delenie nulou
#define ERROR_INTERN 99					//interna chyba programu (alokacia)

#endif // !_ERROR_H_