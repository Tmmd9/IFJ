/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: parser.c
 *				-parser - modul spracováva syntaktickú analýzu 
 *						a sémantickú analýzu
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */

#include <stdio.h>
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
    if((returncode = getNextToken(f, &token, stack)) == TOKEN_OK)	///neviem ci som dobre napisal to stack
    {																///pri debuggingu dat doraz na toto 
        if (type == token.type)										///rovnako ako aj pri samotnej ParserData
            return SYNTAX_OK;
        else
            return ERROR_PARSER;
    }
    return returncode;
}

/*	
*	Funkcia na zabezpecenie spravnej syntaxe
*	v pripade ze nastane nepriazniva situacia
*	koniec viac-riadkoveho komentara TYPE_COMMENT
*	a za nim nenasleduje TYPE_EOL
*
*	Vyuziva Token data zo struct ParserData
*/
int isComment(ParserData *data)
{
	int result;
	if(data.token.type == TYPE_COMMENT)
	{
		///ak nasledujuci token nie je eol, vrat chybu
		if ((result = checkTokenType(f, (&data).Token, data->stack, TYPE_EOL)) == 0)
			return SYNTAX_OK;
		else return result;
	}
	else return ERROR_PARSER;
}
/*
*	 _______________________________________
*	|										|
*	| ZA KAŽDÝM EOL KONTROLUJ isComment()!	|
*	|_______________________________________|
*
*/



static int prog(ParserData* data)
{
	int result;

	/*if((result = getNextToken(f, &data->token, data->stack)) != TOKEN_OK)	///pytam si token 
		return result;	///pokial sa nerovna Token_Ok*/

//1.<prog> -> KEYWORD_DEF TYPE_IDENTIFIER(<params>)TYPE_COLON TYPE_EOL TYPE_INDENT <statement> TYPE_EOL TYPE_DEDENT <prog>
    if (data->Token.attribute.keyword == KEYWORD_DEF && data->Token.type == TYPE_KEYWORD) {
        data->in_declaration = 1;           //je true
        data->not_declared_function = 1;    //je true

        // "Identifikátor funkcie"
        if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_IDENTIFIER)) == 0) {
         	//data.Token.attribute.string->str
            //pridam identifier do tabulky symbolov// checknem ci nenastava redefinicia
        if (/*insert searchFName(nieco, token->attribute.string)*/ != 0)
            return ERROR_PROGRAM_SEMANTIC;   ///pokial sa najde zhoda, je tu pokus o redefiniciu

        //pridam identifier funkcie do tabulky;

        // "("
        if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_LEFT_PAR)) == 0) {
            data.paramIndex = 0;
        	//preskocim na params (poslem im "data" ktore uchovavaju info
        	//ci sme v deklaracii alebo mimo nej atd..
        if ((result = params(&data)) == 0) {
        if (data->Token.type == TYPE_RIGHT_PAR) {  //toto je hlavne test, ptom sa toto ifko moze odstranit
        	//todo-> TYPE_COLON TYPE_EOL TYPE_INDENT <statement> TYPE_EOL TYPE_DEDENT <prog>
        if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_COLON)) == 0) {
    		//tu musime ochranit TypeComent pokial nebude ani jedna z podmienok true, je to syntax err
    	if (((result = checkTokenType(f, &data->Token, data->stack, TYPE_EOL)) == 0) || ((result = isComment(&data)) == 0)) {
		if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_INDENT)) == 0) {
			///petove funkcie
			if ((result = statement(&data)) == 0) {
				
			}
		}
		else return result;
   		}
   		else return result; //nebol EOL ani TYPE_COMMENT EOL
        }
        else return ERROR_PARSER;  //neprisla ':'
        }
        else return ERROR_PARSER;  ///ked sa nieco posralo a neprisla mi z params(&data) ')'
        }
        else return result;
        }
        else return result; //neprisla lava zatvorka
        }
        else return result;	// bolo def, ale nebol spravny identifikator
    }
	else if ((result = isComment(&data)) != 0) {	//ak vrati neziadany return code
    	return result; 								//chyba, vraca chybovy kod
    }
    else
    	return prog(&data);
}

static int params(ParserData *data)
{
    static int result;
    if (data->in_declaration == 1) {
        if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_IDENTIFIER)) == 0) {
            data->paramIndex += 1;

            //skontrolujem ci uz zadana funkcia nema rovnako nazvany parameter->deklarujem nemusim

            //nacitavam dalsi token ak je ciarka ocakavam dalsi param.
            //21. 	<param_next> -> TYPE_COMMA TYPE_IDENTIFIER <param_next> 
            if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_COMMA)) == 0)
                params(&data);	//<param_next>
            else if (data->Token.type == TYPE_RIGHT_PAR) {
                //ulozim ze dana funkcia ma zatial N paramaterov podla data->paramIndex
                return SYNTAX_OK
            }
            else ERROR_PARSER;
        }   
            //nacitany token je prava zatvorka -> <params> -> ε
        else if ((data->Token.type == TYPE_RIGHT_PAR) && (data.paramIndex == 0)) {
            //ulozim ze dana funcia nepotrebuje parametre
            return SYNTAX_OK;
        }
        else
            return ERROR_PARSER; ///param index bol vacsi ako nula ale dosla mi hned zatvorka
    }                            ///pripadne za ciarkou neprisiel dalsi identif
}


static int statement(ParserData *data)
{   
    int result;
    // 5. <statement> -> KEYWORD_IF <expression> TYPE_COLON TYPE_EOL TYPE_INDENT <statement> TYPE_EOL
    // TYPE_DEDENT KEYWORD_ELSE TYPE_COLON TYPE_EOL TYPE_INDENT <statement> TYPE_EOL 
    // TYPE_DEDENT <statement_next>

    //IF
    if ( ( (result = checkTokenType(f, &data->Token, data->stack, TYPE_KEYWORD)) == 0) && 
    	data->Token.attribute.keyword == KEYWORD_IF) {

        /***TODO EXPRESSION***/ //neviem ci to bude funkcia abo co

        //COLON a.k.a dvojbodka
        if((result = checkTokenType(f, &data->Token, data->stack, TYPE_COLON)) == 0) {
            //EOL
            if((result = checkTokenType(f, &data->Token, data->stack, TYPE_EOL)) == 0) {
                //INDENT
                if((result = checkTokenType(f, &data->Token, data->stack, TYPE_INDENT)) == 0) {
                    //rekurzia pre vnutro IF-u
                    statement(&data);
                    /**** nejsom si isty ci treba kontrolovat eol tu ked to kontroluje kazdy
                    statement, vyzaduje midlife crisis na pochopenie ****/
                    if((result = checkTokenType(f, &data->Token, data->stack, TYPE_EOL)) == 0) {
                        //DEDENT
                        if((result = checkTokenType(f, &data->Token, data->stack, TYPE_DEDENT)) == 0) {
                            //ELSE
                            if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_KEYWORD))
                            && data->token.attribute.keyword == KEYWORD_ELSE) {
                                //COLON a.k.a dvojbodka
                                if((result = checkTokenType(f, &data->Token, data->stack, TYPE_COLON)) == 0) {
                                    //EOL
                                    if((result = checkTokenType(f, &data->Token, data->stack, TYPE_EOL)) == 0) {
                                        //INDENT
                                        if((result = checkTokenType(f, &data->Token, data->stack, TYPE_INDENT)) == 0) {
                                            //rekurzia pre vnutro ELSE
                                            statement(&data);
                                            /**** nejsom si isty ci treba kontrolovat eol tu ked to kontroluje kazdy
                                            statement, vyzaduje midlife crisis na pochopenie ****/
                                            if((result = checkTokenType(f, &data->Token, data->stack, TYPE_EOL)) == 0) {
                                                //DEDENT
	                                            if((result = checkTokenType(f, &data->Token, data->stack, TYPE_DEDENT)) == 0) {
	                                        	    //pokracovanie kodiku, koniec tohto LL pravidla
	                                                statement_next(&data);
	                                            } 
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } 
            }
        }
        //SYNTAX ERR -> not really could be scanner or syntax or semantic
        else
            return result;

    }

    // 6. <statement> -> KEYWORD_WHILE TYPE_COLON <expression> TYPE_COLON TYPE_EOL TYPE_INDENT <statement> TYPE_EOL TYPE_DEDENT <statement_next>
    //WHILE
    else if ( ( (result = checkTokenType(f, &data->Token, data->stack, TYPE_KEYWORD)) == 0)
     && data->token.attribute.keyword == KEYWORD_WHILE) {
        //COLON a.k.a dvojbodka
        if((result = checkTokenType(f, &data->Token, data->stack, TYPE_COLON)) == 0) {

            /**** TODO EXPRESSION ****/

            //COLON a.k.a dvojbodka
            if((result = checkTokenType(f, &data->Token, data->stack, TYPE_COLON)) == 0) {
            //EOL
                if((result = checkTokenType(f, &data->Token, data->stack, TYPE_EOL)) == 0) {
                //INDENT
                    if((result = checkTokenType(f, &data->Token, data->stack, TYPE_INDENT)) == 0) {
                        //vnutro while-u rekurzia statementu
                        statement(&data);
                        //EOL
                        if((result = checkTokenType(f, &data->Token, data->stack, TYPE_EOL)) == 0) {
                            //DEDENT
                            if((result = checkTokenType(f, &data->Token, data->stack, TYPE_DEDENT)) == 0) {
                                //pokracovanie kodiku, koniec tohto LL pravidla
                                statement_next(&data);
                            }
                        }
                    }
                }
            }
        }
        //SYNTAX ERR 
        else
            return result;
    }

    // 7. <statement> -> KEYWORD_RETURN <expression>
     else if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_KEYWORD))
     && data->token.attribute.keyword == KEYWORD_RETURN) {
        /**** TODO EXPRESSION ****/
        return result;
    }

    //8.  <statement> -> TYPE_IDENTIFIER TYPE_ASSIGN_VALUE <expression> TYPE_EOL <statement_next>
    if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_KEYWORD))
     && data->token.attribute.keyword == KEYWORD_IF) {

        /**** TODO hodenie do global/lokal symtable kedtak len prepis hodnoty ak to tam je****/
        if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_IDENTIFIER)) == 0) {
            // =
            if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_IDENTIFIER)) == 0) {

                /****TODO 2 EXPRESSION ****/

                //EOL
                if((result = checkTokenType(f, &data->Token, data->stack, TYPE_EOL)) == 0) {
                    //pokracovanie kodiku, koniec tohto LL pravidla
                    statement_next(&data);
                }
            }
        }
    }



}

//9.  <statement_next> -> <statement> || Epsilon (ukoncenie statement rekurzie) 

/**** DIS IS BIG TODO IM RITORD NOW ****/  

static int statement_next(ParserData *data){
    /*ak bude prazdny riadok tak by som asi ukoncil rekurziu lebo neviem ako inak zistit 
    ze je prazdny statement alebo ze zacal nejaky iny, celkovo som asi isto kokot
    */
    if((result = checkTokenType(f, &data.Token, data->stack, TYPE_EOL)) == 0) {
        return;
    }
    else
        statement(&data);
}

/*
*	inicializuje vsetky potrebne data
*	v strukture ParserData
*/
int variablesInit(ParserData *data)
{

}

void variablesFree(ParserData *data)
{

}

int parse()
{

	//subor sa otvara v maine
	int result; 

	string parserStr;
	if (stringInit(&parserStr)) 
		return ERROR_INTERN;
	//set_dynamic_string(&string);

	ParserData *data;
	if (!variablesInit(&data))
	{
		stringStrFree(&string);
		return ERROR_INTERNAL;
	}

	if ((result = getNextToken(&data.Token)) == SCANNER_TOKEN_OK) ///need to change skener as well aby sa tam initoval stack
	{
		if (!code_generator_start())
		{
			stringStrFree(&parserStr);
			variablesFree(&parser_data);
			return ERROR_INTERNAL;
		}

		result = prog(&parser_data);
	}

	stringStrFree(&parserStr);
	variablesFree(&parser_data);

	return result;
}


