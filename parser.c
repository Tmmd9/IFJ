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

FILE *source;

void sourceFile(FILE *f) {
    source = f;
}

/*
 *  Funkcia na porovnavanie očakávaného token.typu
 *  s tym co nam realne prijde so suboru vo forme dalsieho
 *  tokenu
 */
int checkTokenType(FILE *source, token *token, tStack *stack, token_type type)
{
    int returncode;
    if((returncode = getNextToken(f, &token, data->stack)) == TOKEN_OK)
    {
        if (type == token.type)
            return SYNTAX_OK;
        else
            return ERROR_PARSER;
    }
    return returncode;
}


static int prog(ParserData* data)
{
    int result;
//1. <prog> -> KEYWORD_DEF TYPE_INENTIFIER(<params>): TYPE_EOL TYPE_INDENT <statement> TYPE_EOL TYPE_DEDENT <prog>
    if (data->token.attribute.keyword == KEYWORD_DEF && data->token.type == TYPE_KEYWORD) {
        data->in_declaration = 1;           //je true
        data->not_declared_function = 1;    //je true

        // "Identifikátor funkcie"
        if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_IDENTIFIER)) == 0) {
            
            //pridam identifier do tabulky symbolov// checknem ci nenastava redefinicia
            
            if (/*insert searchFName(nieco, token->attribute.string)*/ != 0)
                return ERROR_PROGRAM_SEMANTIC;   ///pokial sa najde zhoda, je tu pokus o redefiniciu
            else
                //pridam identifier funkcie do tabulky;

            // "("
            if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_LEFT_PAR)) == 0) {
                data->paramIndex = 0;
                //preskocim na params (poslem im "data" ktore uchovavaju info
                //ci sme v deklaracii alebo mimo nej atd..
                if ((result = params(&data)) == 0) {
                    if (data->Token.type == TYPE_RIGHT_PAR) {  //toto je hlavne test, ptom sa toto ifko moze odstranit
                        //todo-> : TYPE_EOL TYPE_INDENT <statement> TYPE_EOL TYPE_DEDENT <prog>
                    }
                    else ///ked sa nieco posralo a neprisla mi z params(&data) ')'
                        return ERROR_PARSER;    
                }
            }
            else
                return result;
        }
        else
            return result;
    }


}

static int params(ParserData *data)
{
    static int result;
    if (data->in_declaration == 1) {
        if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_IDENTIFIER)) == 0) {
            data->paramIndex += 1;

            //skontrolujem ci uz zadana funkcia nema rovnako nazvany parameter

            //nacitavam dalsi token ak je ciarka ocakavam dalsi param.
            //14. <param_next> -> TYPE_COMMA TYPE_IDENTIFIER <param_next>
            if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_COMMA)) == 0)
                params(&data);
            else if (data->Token == TYPE_RIGHT_PAR) {
                //ulozim ze dana funkcia ma zatial N paramaterov podla data->paramIndex
                return SYNTAX_OK
            }
            else ERROR_PARSER;
        }   
            //nacitany token je prava zatvorka -> <params> -> ε
        else if (data->Token == TYPE_RIGHT_PAR && data->paramIndex == 0) {
            //ulozim ze dana funcia nepotrebuje parametre
            return SYNTAX_OK;
        }
        else
            return ERROR_PARSER; ///param index bol vacsi ako nula ale dosla mi hned zatvorka
    }                            ///pripadne za ciarkou neprisiel dalsi identif
}

