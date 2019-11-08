/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: expr.h
 *				-parser - modul pre syntaktickú analýzu
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */


#include "parser.h"
#include "scanner.h"
//#include "instlist.h"
#include <stdlib.h>
#include <ctype.h>
#include "expr.h"


/*
 *
 *
 * soemthing
 */

#define GET_TOKEN()													\
	if ((result = get_next_token(&data->token)) != SCANNER_TOKEN_OK)\
		return result


static int prog(ParserData* data)
{
    int result;
    //<prog> -> KEYWORD_DEF TYPE_INENTIFIER(<params>): TYPE_EOL TYPE_INDENT <statement> TYPE_EOL TYPE_DEDENT <prog>
    if (data->token.attribute.keyword == KEYWORD_DEF && data->token.type == TYPE_KEYWORD)
    {

    }
}


