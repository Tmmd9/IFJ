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
	if(data->token.type == TYPE_COMMENT)
	{
		///ak nasledujuci token nie je eol, vrat chybu
		if ((result = checkTokenType(f, (&data).Token, data->stack, TYPE_EOL)) == 0)
			return SYNTAX_OK;
		else return result;
	}
	else if (data->token.type == TYPE_EOL) return SYNTAX_OK
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
        if (htabSearch(&data->globalT, data->Token->attribute.string) != NULL) return ERROR_PROGRAM_SEMANTIC;   
        ///pokial sa najde zhoda, je tu pokus o redefiniciu
        	bool errIntern;
        	data->currentID = htabAddSymbol(&data->globalT, data->Token->attribute.string, &errIntern);
       	if (errIntern == true) return ERROR_INTERN;
        // "("
        if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_LEFT_PAR)) == 0) {
            data.paramIndex = 0;
        	//preskocim na params (poslem im "data" ktore uchovavaju info
        	//ci sme v deklaracii alebo mimo nej atd..
        if ((result = params(&data)) == 0) {
        if (data->Token.type == TYPE_RIGHT_PAR) {  //toto je hlavne test, ptom sa toto ifko moze odstranit
        	//todo-> TYPE_COLON TYPE_EOL TYPE_INDENT <statement> TYPE_EOL TYPE_DEDENT <prog>
        if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_COLON)) == 0) {
        	data->in_declaration = 0;
    		//tu musime ochranit TypeComent pokial nebude ani jedna z podmienok true, 
    		//je to syntax err ->vlastne nie ocheckoval som to a nemusim
    	if (((result = checkTokenType(f, &data->Token, data->stack, TYPE_EOL)) == 0)) {
		if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_INDENT)) == 0) {
			///petove funkcie
			data->in_declaration = 0;
			data->in_function = 1;
			data->deepLabel +=1;	//mam indent, zmena urovne
			if ((result = statement(&data)) == 0) {
				
			}
		}
		else return result;	//neprisiel indent
   		}
   		else return result; //nebol EOL 
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
    //som vo deklaracii funkcie
    if (data->in_declaration == 1) {
        if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_IDENTIFIER)) == 0) {
            switch (data->Token.type) {
            	case TYPE_INT:
            		data->globalT->param[data->paramIndex] = i;
            		break;
            	case TYPE_FLOAT:
            		data->globalT->param[data->paramIndex] = f;
            		break;
            	case TYPE_STRING:
            		data->globalT->param[data->paramIndex] = s;
            		break;
            	default:
            		data->globalT->param[data->paramIndex] = u;
            		break;
            }
            data->paramIndex += 1;
            data->globalT->param[data->paramIndex] = '\0';

            bool errIntern;
            if (!(data->rightID = htabAddSymbol(&data->localT, data->Token.attribute.string->str, &errIntern))){
				if (internal_error == true) return ERROR_INTERN;
				else return ERROR_PROGRAM_SEMANTIC;
			}
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
    if ( ((result = checkTokenType(f, &data->Token, data->stack, TYPE_KEYWORD)) == 0) && 
    	(data->Token.attribute.keyword == KEYWORD_IF)) {
    	data->in_while_or_if = 1;
    	data->uniqLabel +=1;
    	if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_IDENTIFIER)) == 0) {
        	if ((data->leftID = htabSearch(&data->localT, data->Token->attribute.string)) == NULL){ //ak nie je v local
        		if ((data->leftID = htabSearch(&data->globalT, data->Token->attribute.string)) == NULL) //ak nie je v global
        			return ERROR_PROGRAM_SEMANTIC;   
        	}
    	}
    	else return result;
    	if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_IDENTIFIER)) == 0) { //porovnavac
    		
    	}	//ulozim si na stack co to bolo za porovnavac
    	else return result;	//prisiel nevalidny porovnavac
    	if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_IDENTIFIER)) == 0) {
        	if ((data->rightID = htabSearch(&data->localT, data->Token->attribute.string)) == NULL){ //ak nie je v local
        		if ((data->rightID = htabSearch(&data->globalT, data->Token->attribute.string)) == NULL) //ak nie je v global
        			return ERROR_PROGRAM_SEMANTIC;   
        	}
    	}
    	else return result;
		/*
    	if ( (result = expression funkcii poslem (laveID, porovnavac, praveID)) == 0){
    		if ((result = codegenerator(nejakeMakro, lava hodnota, prava, porovnavac?))!=0 )
    			return result;
    	}
    	else return result;
		*/
        //COLON a.k.a dvojbodka
        if((result = checkTokenType(f, &data->Token, data->stack, TYPE_COLON)) == 0) {
           
        if((result = checkTokenType(f, &data->Token, data->stack, TYPE_EOL)) == 0) {
                
        if((result = checkTokenType(f, &data->Token, data->stack, TYPE_INDENT)) == 0) {
        	data->deepLabel +=1;	//mam indent, zmena urovne
            //rekurzia pre vnutro IF-u
			if((result = statement(&data)) != SYNTAX_OK) return result;
			//DEDENT zo statement_next sa mi vypytal dalsi token, nemusim pytat novy
		if(data->Token.type == TYPE_DEDENT) {
			data->deepLabel -=1;
		if ((result = checkTokenType(f, &data->Token, data->stack, TYPE_KEYWORD)) && 
			data->token.attribute.keyword == KEYWORD_ELSE) {
                                //COLON a.k.a dvojbodka
		if((result = checkTokenType(f, &data->Token, data->stack, TYPE_COLON)) == 0) {
                                    //EOL
		if((result = checkTokenType(f, &data->Token, data->stack, TYPE_EOL)) == 0) {
                                        //INDENT
		if((result = checkTokenType(f, &data->Token, data->stack, TYPE_INDENT)) == 0) {
			data->deepLabel +=1;	//mam indent, zmena urovne
			//rekurzia pre vnutro ELSE
            if((result = statement(&data)) != SYNTAX_OK) return result;
			//DEDENT zo statement_next sa mi vypytal dalsi token, nemusim pytat novy
		if(data->Token.type == TYPE_DEDENT) {
			data->deepLabel -=1;
			//pokracovanie statementov
			if((result = statement_next(&data)) != SYNTAX_OK) return result;
		} 
		else return ERROR_PARSER;
		}
		else return result;
		}
		else return result;
		}
		else return result;
		}
		else return result;
		}
		else return ERROR_PARSER;
		} 
		else return result;
		}
		else return result;
		} 		//SYNTAX ERR -> not really could be scanner or syntax or semantic
		else return result;
		}

    // 6. <statement> -> KEYWORD_WHILE TYPE_COLON <expression> TYPE_COLON TYPE_EOL TYPE_INDENT <statement> TYPE_EOL TYPE_DEDENT <statement_next>
    //WHILE
    else if ((data->Token.type == TYPE_KEYWORD) && (data->token.attribute.keyword == KEYWORD_WHILE)) {
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
     else if ((data->Token.type == TYPE_KEYWORD) && (data->token.attribute.keyword == KEYWORD_RETURN)) {
        /**** TODO EXPRESSION ****/
        return result;
    }

    //8.  <statement> -> TYPE_IDENTIFIER TYPE_ASSIGN_VALUE <expression> TYPE_EOL <statement_next>
    else if ((data->Token.type == TYPE_KEYWORD) && (data->token.attribute.keyword == KEYWORD_IF)) {

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
	//overujem ci nahodou nenastala situacia s komentom alebo je tam len prosté EOL
    else if ((result = isComment(&data)) == 0) return statement_next(&data);
    else return result;

}

//10.  <statement_next> -> <statement> || Epsilon (ukoncenie statement rekurzie)->not really

/**** DIS IS BIG TODO IM RITORD NOW ****/  

static int statement_next(ParserData *data) 
{
	int result;
    if ((result = isComment(&data)) == 0 ) return statement(&data);
    else if ( ((result = checkTokenType(f, &data->Token, data->stack, TYPE_DEDENT)) == 0) || 
    		data->Token.attribute.keyword == KEYWORD_DEF )// <statement_next> -> ε	//pravdepodobne staci len ten dedent
    	return SYNTAX_OK; 
    else return result;
}

/*
*	inicializuje vsetky potrebne data
*	v strukture ParserData
*/
int variablesInit(ParserData *data)
{
	htabInit(&data->globalT);
	htabInit(&data->localT);

	data->currentID = NULL;
	data->leftID = NULL;
	data->rightID = NULL;

	data->paramIndex = 0;
	data->uniqLabel = 0;
	data->deepLabel = 0;		//DH tu ma -1 dunno why

	data->in_function = 0;
	data->in_declaration = 0;
	data->in_while_or_if = 0;
	data->non_declared_function = 0;

	/*
	*	inicializacia 
	*	vstavanych 
	*	funkcii
	*/
	bool errIntern;
	Data* id;

	// Len(s) returns int
	id = htabAddSymbol(&data->globalT, "len", &errIntern);
	if (errIntern) 
		return ERROR_INTERN;

	id->isDefined = true;
	id->type = DTYPE_INT;
	if (!htabAddParam(id, DTYPE_STRING)) 
		return ERROR_INTERN;

	// SubStr(s =string, i =Integer, n =Integer) returns String
	id = htabAddSymbol(&data->globalT, "substr", &errIntern);
	if (errIntern) 
		return ERROR_INTERN;

	id->isDefined = true;
	id->type = DTYPE_STRING;
	if (!htabAddParam(id, DTYPE_STRING)) 
		return ERROR_INTERN;
	if (!htabAddParam(id, DTYPE_INT)) 
		return ERROR_INTERN;
	if (!htabAddParam(id, DTYPE_INT)) 
		return ERROR_INTERN;

	// Ord(s =String, i =Integer) returns Integer
	id = htabAddSymbol(&data->globalT, "ord", &errIntern);
	if (errIntern) 
		return ERROR_INTERN;

	id->isDefined = true;
	id->type = DTYPE_INT;
	if (!htabAddParam(id, DTYPE_STRING)) 
		return ERROR_INTERN;
	if (!htabAddParam(id, DTYPE_INT)) 
		return ERROR_INTERN;

	// Chr(i) returns char
	id = htabAddSymbol(&data->globalT, "chr", &errIntern);
	if (errIntern) 
		return ERROR_INTERN;

	id->isDefined = true;
	id->type = DTYPE_STRING;
	if (!htabAddParam(id, DTYPE_INT)) 
		return ERROR_INTERN;

/*
	// Global variable %exp_result for storing result of expression.
	id = htabAddSymbol(&data->globalT, "%exp_result", &errIntern);
	if (errIntern) return ERROR_INTERN;
	id->isDefined = true;
	id->type = TYPE_UNDEFINED;
	id->isGlobal = true;
*/
	return SYNTAX_OK;
}


/*
*	uvolni pouzivane premenne zo symtable
*/
void variablesFree(ParserData *data)
{
	htabFree(&data->globalT);
	htabFree(&data->localT);
}

int parse()
{

	//subor sa otvara v maine
	int result; 

	string parserStr;
	if (!stringInit(&parserStr)) 
		return ERROR_INTERN;
	//set_dynamic_string(&string);

	ParserData *data;
	if (variablesInit(&data) == ERROR_INTERN)
	{
		stringStrFree(&string);
		return ERROR_INTERN;
	}

	if ((result = getNextToken(&data.Token)) == SCANNER_TOKEN_OK) 
	///need to change skener as well aby sa tam initoval stack
	///aby som pri kazdom volani getnexttoken pouzival len &data.Token
	{

		//nemame nic z generatoru zatial
		/*if (!code_generator_start())
		{
			stringStrFree(&parserStr);
			variablesFree(&data);
			return ERROR_INTERN;
		}*/

		result = prog(&data);
	}

	stringStrFree(&parserStr);
	variablesFree(&data);

	return result;
}