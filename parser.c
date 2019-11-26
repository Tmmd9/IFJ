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

#include "parser.h"
#include "scanner.h"
#include "expr.h"
#include <ctype.h>
#include "generator.h"



//FILE *source;

static int params(ParserData *data);

static int statement(ParserData *data);

static int statement_next(ParserData *data);


//        void sourceFile(FILE *f) {
//    source = f;
//}

/*
 *  Funkcia na porovnavanie očakávaného token.typu
 *  s tym co nam realne prijde so suboru vo forme dalsieho
 *  tokenu
 */
int checkTokenType(token *Token, token_type type)
{
    int returncode = 0;
    if((returncode = getNextToken(Token)) == TOKEN_OK)	///neviem ci som dobre napisal to stack
    {																///pri debuggingu dat doraz na toto 
        if (type == Token->type)										///rovnako ako aj pri samotnej ParserData
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
	int result = 0;
	if(data->Token.type == TYPE_COMMENT)
	{
		///ak nasledujuci token nie je eol, vrat chybu
		if ((result = checkTokenType(&data->Token, TYPE_EOL)) == 0)
			return SYNTAX_OK;
		else return result;
	}
	else if (data->Token.type == TYPE_EOL) return SYNTAX_OK;
	else return ERROR_PARSER;
}
/*
*	 _______________________________________
*	|										|
*	| ZA KAŽDÝM EOL KONTROLUJ isComment()!	|
*	|_______________________________________|
*
*/


/*
*   Pomocna funkcia, aby som usetril riadky a zvysil prehladnost
*   Funkcia prida identifikator do globalnej (bool false) alebo 
*   do lokalnej hash tabulky (bool true)
*   int position - 0-LEFT 1-CURRENT 2-RIGHT
*/
int addToHash(ParserData *data, bool isLocal, int position) {
    bool errIntern;
    ///pridavam do lokalnej tabulky
    if (isLocal == true) {

        switch (position) {
            case 0:
                data->leftID = htabAddSymbol(&data->localT, data->Token.attribute.string->str, &errIntern);
                if (errIntern == true) return ERROR_INTERN;
                break;
            case 1:
                data->currentID = htabAddSymbol(&data->localT, data->Token.attribute.string->str, &errIntern);
                if (errIntern == true) return ERROR_INTERN;
                break;
            case 2:
                data->rightID = htabAddSymbol(&data->localT, data->Token.attribute.string->str, &errIntern);
                if (errIntern == true) return ERROR_INTERN;
                break;
            default:
                return ERROR_INTERN;
        }
    } 
    else {
        ///case kedy pridavam do global
        switch (position) {
            case 0:
                data->leftID = htabAddSymbol(&data->globalT, data->Token.attribute.string->str, &errIntern);
                if (errIntern == true) return ERROR_INTERN;
                data->leftID->isGlobal = true;
                break;
            case 1:
                data->currentID = htabAddSymbol(&data->globalT, data->Token.attribute.string->str, &errIntern);
                if (errIntern == true) return ERROR_INTERN;
                data->currentID->isGlobal = true;
                break;
            case 2:
                data->rightID = htabAddSymbol(&data->globalT, data->Token.attribute.string->str, &errIntern);
                if (errIntern == true) return ERROR_INTERN;
                data->rightID->isGlobal = true;
                break;
            default:
                return ERROR_INTERN;
        }
    }
    return SYNTAX_OK;
}



static int prog(ParserData* data)
{
	static int result;

/*  *   *   *   *   *   *   *   vzdy si na zaciatku pytam token     *   *   *   *   *   *   */
    if (data->Token.attribute.keyword != KEYWORD_DEF ) {
        if ((result = getNextToken(&data->Token)) != 0) return result;
    }

/*  *   *   *   *   *   *   *   vzdy si na zaciatku pytam token     *   *   *   *   *   *   */

/*1.<prog> -> KEYWORD_DEF TYPE_IDENTIFIER(<params>)TYPE_COLON TYPE_EOL TYPE_INDENT <statement> TYPE_EOL TYPE_DEDENT <prog>*/
    if (data->Token.attribute.keyword == KEYWORD_DEF && data->Token.type == TYPE_KEYWORD) {
        data->in_declaration = 1;           //je true

        // "Identifikátor funkcie"
        if ((result = checkTokenType(&data->Token, TYPE_IDENTIFIER)) == 0) {
            //pridam identifier do tabulky symbolov// checknem ci nenastava redefinicia
        if ((data->currentID = htabSearch(&data->globalT, data->Token.attribute.string->str)) != NULL &&
            data->currentID->isDefined == true) return ERROR_PROGRAM_SEMANTIC;
            ///pokial sa najde zhoda, je tu pokus o redefiniciu

        if ((data->currentID = htabSearch(&data->globalT, data->Token.attribute.string->str)) == NULL ) {

            bool errIntern;
            data->currentID = htabAddSymbol(&data->globalT, data->Token.attribute.string->str, &errIntern);
            if (errIntern == true) return ERROR_INTERN;

        }
            GENERATE(genFunctionHead,data->Token.attribute.string->str);
            data->leftID = NULL;
       	    //data->not_declared_function = 0;    //lebo ju deklarujeme
            data->currentID->isDefined = true;
        if ((result = checkTokenType(&data->Token, TYPE_LEFT_PAR)) == 0) {
            data->paramIndex = 0;
        	//preskocim na params (poslem im "data" ktore uchovavaju info
        	//ci sme v deklaracii alebo mimo nej atd..
        if ((result = params(data)) == 0) {

        if (data->Token.type == TYPE_RIGHT_PAR) {  //toto je hlavne test, ptom sa toto ifko moze odstranit
        if ((result = checkTokenType(&data->Token, TYPE_COLON)) == 0) {
    	if (((result = checkTokenType(&data->Token, TYPE_EOL)) == 0)) {
        if ((result = checkTokenType(&data->Token, TYPE_INDENT)) == 0) {

            data->in_function = 1;
            data->deepLabel += 1;    //mam indent, zmena urovne

/*  *   *   *   *   pytam si dalsi token lebo statement si sam nepyta   *   *   *   *   *   *   */
            if ((result = getNextToken(&data->Token)) != 0) return result;
        if ((result = statement(data)) != 0) {
            if (data->was_return == true && (result == 0 || result == 2)) {          ///bol return
                data->was_return = false;
                result = 0;
                if ((result = getNextToken(&data->Token)) != 0) return result;      ///pytam si dalsi token lebo return nepyta
                }
            else return result;      //nieco sa posralo v statement
        }
/*  *   *   *   *   pytam si dalsi token lebo statement si sam nepyta   *   *   *   *   *   *   */

        if (data->Token.type == TYPE_DEDENT) {

            data->in_declaration = 0;
            data->in_function = 0;
            data->deepLabel -= 1;

            if(data->Token.type == TYPE_EOF) return SYNTAX_OK;
            //rekurzia aby som sa vratil spat do <prog>
            else {
                //po kazdej definicii funkcie treba uvolnovat tabulku lokalnych premennych
                htabFree(&data->localT);
                return result = prog(data);
            }

        }
        else return result; //neprisiel DEDENT
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
/*  *   *   *   *   *   *   4.  <prog> -> TYPE_EOF <end>    *   *   *   *   *   *   *   */
/*  *   *   *   *   *   *   *   * 24. <end> -> ε    *   *   *   *   *   *   *   *   *   */
    else if (data->Token.type == TYPE_EOF) {
        return result = SYNTAX_OK;
    }
	else if ((result = isComment(data)) == 0) {    //ak dokumentacny retazec alebo basic EOL
        return result = prog(data);
    }
/*  *   *   *   *   * 3.  <prog> -> <statement> TYPE_EOL <prog> *   *   *   *   *   *   */
    else {
	    return result = statement(data);
	}
}

static int params(ParserData *data)
{
    static int result;
    //som vo deklaracii funkcie
    if ((data->leftID == NULL && data->in_declaration == 1) || (data->leftID !=NULL && data->in_declaration == 0 && data->leftID->isDefined == true)) {
        if ((result = checkTokenType(&data->Token, TYPE_IDENTIFIER)) == 0) {
            switch (data->Token.type) {
                case TYPE_INT:
                    stringAddChar(data->currentID->param, 'i');
                    break;
                case TYPE_FLOAT:
                    stringAddChar(data->currentID->param, 'f');
                    break;
                case TYPE_STRING:
                    stringAddChar(data->currentID->param, 's');
                    break;
                default:
                    stringAddChar(data->currentID->param, 'u');
                    break;
            }
            data->paramIndex += 1;
            //data->globalT->param[data->paramIndex] = '\0'; to uz robi addchar samotne

            bool errIntern;
            if (!(data->rightID = htabAddSymbol(&data->localT, data->Token.attribute.string->str, &errIntern))){
                if (errIntern == true) return ERROR_INTERN;
                else return ERROR_PROGRAM_SEMANTIC; ///redefinicia
            }

            GENERATE(genFunctionParam, data->Token.attribute.string->str, data->paramIndex);

            //nacitavam dalsi token ak je ciarka ocakavam dalsi param.
            //21. 	<param_next> -> TYPE_COMMA TYPE_IDENTIFIER <param_next>
            if ((result = checkTokenType(&data->Token, TYPE_COMMA)) == 0)
                result = params(data);	//<param_next>
            else if (data->Token.type == TYPE_RIGHT_PAR) {
                data->currentID->paramCount = data->paramIndex;
                //ulozim ze dana funkcia ma zatial N paramaterov podla data->paramIndex
                return SYNTAX_OK;
            }
            else return ERROR_PARSER;   //neprisla ciarka ani prava zatvorka
        }
            //nacitany token je prava zatvorka -> <params> -> ε
        else if ((data->Token.type == TYPE_RIGHT_PAR) && (data->paramIndex == 0)) {
            //ulozim ze dana funcia nepotrebuje parametre
            data->currentID->paramCount = data->paramIndex;
            return SYNTAX_OK;
        }
        else
            return ERROR_PARSER; ///param index bol vacsi ako nula ale dosla mi hned zatvorka
    }                            ///pripadne za ciarkou neprisiel dalsi identif

    //ten divny pripad kedy volam v definicii funkcie nedefinovanu funkciu
    else if(data->leftID->isDefined == false && data->in_declaration == 1) {
        if ((result = checkTokenType(&data->Token, TYPE_IDENTIFIER)) == 0) {
            switch (data->Token.type) {
                case TYPE_INT:
                    stringAddChar(data->leftID->param, 'i');
                    break;
                case TYPE_FLOAT:
                    stringAddChar(data->leftID->param, 'f');
                    break;
                case TYPE_STRING:
                    stringAddChar(data->leftID->param, 's');
                    break;
                default:
                    stringAddChar(data->leftID->param, 'u');
                    break;
            }
            data->paramIndex += 1;
            //data->globalT->param[data->paramIndex] = '\0'; to uz robi addchar samotne

            bool errIntern;
            if (!(data->rightID = htabAddSymbol(&data->localT, data->Token.attribute.string->str, &errIntern))){
                if (errIntern == true) return ERROR_INTERN;
                else return ERROR_PROGRAM_SEMANTIC; ///redefinicia
            }
            //nacitavam dalsi token ak je ciarka ocakavam dalsi param.
            //21. 	<param_next> -> TYPE_COMMA TYPE_IDENTIFIER <param_next>
            if ((result = checkTokenType(&data->Token, TYPE_COMMA)) == 0)
                params(data);	//<param_next>
            else if (data->Token.type == TYPE_RIGHT_PAR) {
                data->leftID->paramCount = data->paramIndex;
                //ulozim ze dana funkcia ma zatial N paramaterov podla data->paramIndex
                return SYNTAX_OK;
            }
            else return ERROR_PARSER;   //neprisla ciarka ani prava zatvorka
        }
            //nacitany token je prava zatvorka -> <params> -> ε
        else if ((data->Token.type == TYPE_RIGHT_PAR) && (data->paramIndex == 0)) {
            //ulozim ze dana funcia nepotrebuje parametre
            data->leftID->paramCount = data->paramIndex;
            return SYNTAX_OK;
        }
        else
            return ERROR_PARSER; ///param index bol vacsi ako nula ale dosla mi hned zatvorka
    }                            ///pripadne za ciarkou neprisiel dalsi identif

    return result;
}


static int statement(ParserData *data)
{ 
    static int result;
/******************************************** I F *************************************************      
*            	5. 	<statement> -> KEYWORD_IF <expression> TYPE_COLON                             *
*            	TYPE_EOL TYPE_INDENT <statement> TYPE_DEDENT KEYWORD_ELSE TYPE_COLON              *
*            	TYPE_EOL TYPE_INDENT <statement> TYPE_DEDENT <statement_next>                     *
***************************************************************************************************/
    if ((data->Token.type == TYPE_KEYWORD) && (data->Token.attribute.keyword == KEYWORD_IF)) {
    	data->in_if = 1;
    	data->uniqLabel +=1;

/*  *   *   *   *   *   *   *   *   posielam expression do Expr.c   *   *   *   *   *   *   *   *   */
        if ((result = getNextToken(&data->Token)) != 0) return result;
        if ((result = expression(data)) != 0 ) return result;
/*  *   *   *   *   *   *   *   *   posielam expression do Expr.c   *   *   *   *   *   *   *   *   */

        //COLON a.k.a dvojbodka	, pytal som si token v expression
        if(data->Token.type == TYPE_COLON) {      
        if((result = checkTokenType(&data->Token, TYPE_EOL)) == 0) {            
        if((result = checkTokenType(&data->Token, TYPE_INDENT)) == 0) {
        	data->deepLabel +=1;	//mam indent, zmena urovne
            //rekurzia pre vnutro IF-u
            if ((result = getNextToken(&data->Token)) != 0) return result;
			if((result = statement(data)) != SYNTAX_OK) return result;
			//DEDENT zo statement_next sa mi vypytal dalsi token, nemusim pytat novy
		if(data->Token.type == TYPE_DEDENT) {
			data->deepLabel -=1;
		if (((result = checkTokenType(&data->Token, TYPE_KEYWORD)) == 0) &&
			data->Token.attribute.keyword == KEYWORD_ELSE) {
		if((result = checkTokenType(&data->Token, TYPE_COLON)) == 0) {
		if((result = checkTokenType(&data->Token, TYPE_EOL)) == 0) {
		if((result = checkTokenType(&data->Token, TYPE_INDENT)) == 0) {
			data->deepLabel +=1;	//mam indent, zmena urovne
			//rekurzia pre vnutro ELSE
            if ((result = getNextToken(&data->Token)) != 0) return result;
            if((result = statement(data)) != SYNTAX_OK) return result;
			//DEDENT zo statement_next sa mi vypytal dalsi token, nemusim pytat novy
		if(data->Token.type == TYPE_DEDENT) {
			data->deepLabel -=1;
			data->in_if = 0;
            //htabFree(&data->localT);        ///potrebne premazat lokalnu tabulku
			
/*  *   *   *   *   *   *   pokracovanie statementov    *   *   *   *   *   *   *   */
            if ((result = getNextToken(&data->Token)) != 0) return result;
			if((result = statement_next(data)) != SYNTAX_OK) return result;
/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */
		}       //neprisiel DEDENT
		else return ERROR_PARSER;
		}       //neprisiel INDENT
		else return result;
		}       //neprisiel EOL
		else return result;
		}       //neprisiel COLON
		else return result;
		}       //neprisiel ELSE
		else return result;
		}       //neprisiel DEDENT
		else return ERROR_PARSER;
		}       //neprisiel INDENT
		else return result;
		}       //neprisiel EOL
		else return result;
		} 		//neprisiel COLON
		else return result;
    }//IF



/******************************************** W H I L E********************************************      
*               6. <statement> -> KEYWORD_WHILE <expression> TYPE_COLON                           *
*                   TYPE_EOL TYPE_INDENT <statement> TYPE_EOL                                     *
*                   TYPE_DEDENT <statement_next>                                                  *
***************************************************************************************************/
/*6. <statement> -> KEYWORD_WHILE <expression> TYPE_COLON TYPE_EOL TYPE_INDENT <statement> TYPE_EOL TYPE_DEDENT <statement_next>*/
    //WHILE
    else if ((data->Token.type == TYPE_KEYWORD) && (data->Token.attribute.keyword == KEYWORD_WHILE)) {
		data->in_while = 1;
    	data->uniqLabel +=1;
        
/*  *   *   *   *   *   *   *   *   posielam expression do Expr.c   *   *   *   *   *   *   *   *   */
        if ((result = getNextToken(&data->Token)) != 0) return result;
        if ((result = expression(data)) != 0 ) return result;
/*  *   *   *   *   *   *   *   *   posielam expression do Expr.c   *   *   *   *   *   *   *   *   */

		if(data->Token.type == TYPE_COLON) {
	    if((result = checkTokenType(&data->Token, TYPE_EOL)) == 0) {
	    if((result = checkTokenType(&data->Token, TYPE_INDENT)) == 0) {
			data->deepLabel +=1;	//mam indent, zmena urovne

			    //vnutro while-u rekurzia statementu
        if ((result = getNextToken(&data->Token)) != 0) return result;
	    if ((result = statement(data)) != SYNTAX_OK) return result;
	    if (data->Token.type == TYPE_DEDENT) {
			data->deepLabel -=1;	//mam dedent, zmena urovne
			data->in_while = 0;
        //    htabFree(&data->localT);
/*  *   *   *   *   *   *   pokracovanie statementov    *   *   *   *   *   *   *   */
            if ((result = getNextToken(&data->Token)) != 0) return result;
            if((result = statement_next(data)) != SYNTAX_OK) return result;
/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */
	    }
	    else return result; //neprisiel dedent
	    }
	    else return result; //neprisiel indent
	    }
	    else return result; //neprisiel EOL
	    }
	    else return ERROR_PARSER; //neprisiel COLON
    } //WHILE



/******************************************** R E T U R N *******************************************   
*   *   *   *   *   *   *   *7. <statement> -> KEYWORD_RETURN <expression> *   *   *   *   *   *   *
****************************************************************************************************/
     else if ((data->Token.type == TYPE_KEYWORD) && (data->Token.attribute.keyword == KEYWORD_RETURN)) {
        if (data->in_function < 1) return ERROR_SEMANTIC_OTHERS;    ///pokial sa vola mimo funkcie
        data->was_return=true;
/*  *   *   *   *   *   *   *   *   posielam expression do Expr.c   *   *   *   *   *   *   *   *   */
        if ((result = getNextToken(&data->Token)) != 0) return result;
        if ((result = expression(data)) != 0 ) return result;

/*  *   *   *   *   *   *   *   *   posielam expression do Expr.c   *   *   *   *   *   *   *   *   */

        else if (data->Token.type == TYPE_EOL) {
            GENERATE(generateReturn, data->currentID->identifier);
            if ((result = getNextToken(&data->Token)) != 0) return result;
            else return result = SYNTAX_OK;

        }
        else return ERROR_PARSER;

    } //end of return



/*******   DEKLARACIA PREMENNYCH || VOLANIE FUNKCIE || PRIRADOVANIE HODNOTY DO PREMENNEJ   **********
*  *    tu moze nastat jednak definicia a jednak len priradenie hodnoty dolezite hlavne pri WHILE   *
*  *  * 8.  <statement> -> TYPE_IDENTIFIER TYPE_ASSIGN_VALUE <expression> TYPE_EOL <statement_next  *
*  *   *   *    9.	<statement> -> TYPE_IDENTIFIER(<params>) <statement_next>   *   *   *   *   *   *
****************************************************************************************************/
    else if (data->Token.type == TYPE_IDENTIFIER) {
        static int result;

        if ((data->leftID = htabSearch(&data->globalT, data->Token.attribute.string->str)) == NULL) {
            //kvoli pripadu ci nahodou nenastava volanie funkcie ktora neexistuje
            if ((result = checkTokenType(&data->Token, TYPE_ASSIGN_VALUE)) == 0) {

                //ak nie je globalne pridam ju
                if (data->in_function == 1 ) {

                    if ((data->leftID = htabSearch(&data->localT, data->Token.attribute.string->str)) == NULL) {
                        ///pridam premennu lokalne vo funkci
                        if ((result = addToHash(data, true, 0)) != 0) return ERROR_INTERN;
                        char *frame= "LF";
                        GENERATE(declareVar,frame,data->Token.attribute.string->str);
                    }
                    //else -> nerobim nic - nepridavam do tabulky
                }
                else {
                    //pridavam premennu globalne
                    if ((result = addToHash(data, false, 0)) != 0) return ERROR_INTERN;
                    char *frame= "GF";
                    GENERATE(declareVar,frame,data->Token.attribute.string->str);
                }

                if (data->Token.type == TYPE_ASSIGN_VALUE) {

                    //tuto poslem timkovi vyraz na rozparsovanie//

/*  *   *   *   *   *   *   *   *   posielam expression do Expr.c   *   *   *   *   *   *   *   *   */
                    //GENERATE(pushDeclareVar, data->Token.attribute.string->str);
                    if ((result = getNextToken(&data->Token)) != 0) return result;
                    if (data->Token.type == TYPE_KEYWORD && (   data->Token.attribute.keyword == KEYWORD_INPUTI ||
                                                                data->Token.attribute.keyword == KEYWORD_INPUTS ||
                                                                data->Token.attribute.keyword == KEYWORD_INPUTF ||
                                                                data->Token.attribute.keyword == KEYWORD_SUBSTR ||
                                                                data->Token.attribute.keyword == KEYWORD_LEN    ||
                                                                data->Token.attribute.keyword == KEYWORD_CHR    ||
                                                                data->Token.attribute.keyword == KEYWORD_ORD))
                    {
                        return result = statement(data);
                    }
                    else if ((result = expression(data)) != 0 ) return result;

/*  *   *   *   *   *   *   *   *   posielam expression do Expr.c   *   *   *   *   *   *   *   *   */

                    ///tu moze byt aj EOF dont forget
                    if (data->Token.type == TYPE_EOL) {

                        return result = statement_next(data);

                    }
                    else if (data->Token.type == TYPE_EOF) return SYNTAX_OK;
                    else return ERROR_PARSER;

                }
                else return ERROR_PARSER;

                } else if (data->Token.type == TYPE_LEFT_PAR) {
                if (data->in_declaration == 1) {
                    if ((result = addToHash(data, false, 0)) != 0) return ERROR_INTERN;
                    else {
                        data->leftID->isDefined = false;
                        data->leftID->isGlobal = false; ///explicitne to musim prestavit

                        if ((result = params(data)) != 0) return result;

                        if ((result = checkTokenType(&data->Token, TYPE_EOL)) == 0) {

                            return result = statement_next(data);
                        }
                        else return result;
                    }
                }
                return result = ERROR_PROGRAM_SEMANTIC; //volanie funkcie ktora neexistuje
            } else return result = ERROR_PARSER;
        }
//ak uz je definovana globalne, lenze ak aj sme vo funkcii, nevieme ci prepisujeme
// lokalnu alebo glob, lebo python
//tak som sa rozhodol ze sa proste bude prepisovat globalna --fixed no more-uz sa bude upravovat lokalna
        else if (data->leftID->isGlobal == true) {

            if ((result = checkTokenType(&data->Token, TYPE_ASSIGN_VALUE)) == 0) {
                if (data->in_function == 1 ) {
                    if ((data->leftID = htabSearch(&data->localT, data->Token.attribute.string->str)) == NULL) {
                        ///pridam premennu lokalne vo funkci
                        if ((result = addToHash(data, true, 0)) != 0) return ERROR_INTERN;
                    }
                    //else -> nerobim nic - nepridavam do tabulky
                }
/*  *   *   *   *   *   *   *   *   posielam expression do Expr.c   *   *   *   *   *   *   *   *   */
                if ((result = getNextToken(&data->Token)) != 0) return result;
                if (data->Token.type == TYPE_KEYWORD && (data->Token.attribute.keyword == KEYWORD_INPUTI ||
                    data->Token.attribute.keyword == KEYWORD_INPUTS || data->Token.attribute.keyword == KEYWORD_INPUTF ||
                    data->Token.attribute.keyword == KEYWORD_SUBSTR || data->Token.attribute.keyword == KEYWORD_LEN ||
                    data->Token.attribute.keyword == KEYWORD_CHR || data->Token.attribute.keyword == KEYWORD_ORD )) {
                    return result = statement(data);
                }
                else if ((result = expression(data)) != 0 ) return result;
/*  *   *   *   *   *   *   *   *   posielam expression do Expr.c   *   *   *   *   *   *   *   *   */
               ///tu moze byt aj EOF dont forget
                    if (data->Token.type == TYPE_EOL) {

                        return result = statement_next(data);

                    } else if (data->Token.type == TYPE_EOF) return SYNTAX_OK;
                    else return ERROR_PARSER;

            } else return result = ERROR_PARSER;
        }
        /*  *   *   *   *   *   *   VOLANIE FUNKCIE    *   *   *   *   *   *   */
        else if (data->leftID->isDefined == true) {

                if ((result = checkTokenType(&data->Token, TYPE_LEFT_PAR)) == 0) {

                    if (data->leftID->paramCount !=0) {
                        data->paramIndex = 1;
                        while (data->paramIndex <= data->leftID->paramCount) {
                            ++(data->paramIndex);
                            //tu neviem ci to na analyzu kvoli typom treba poslat do expression ale asi ani nie
                            if ((result = checkTokenType(&data->Token, TYPE_IDENTIFIER)) != 0 &&
                            (data->Token.type != TYPE_INT && data->Token.type != TYPE_STRING && data->Token.type != TYPE_FLOAT)) {
                                if(data->Token.type == TYPE_RIGHT_PAR) return ERROR_WRONG_NUMBER_OF_PARAMS;
                                else return result;
                            }
                            if ((result = checkTokenType(&data->Token, TYPE_COMMA)) != 0) {
                                if (data->Token.type == TYPE_RIGHT_PAR)
                                    result = 0;
                                else return result;
                            }
                        }
                        if (data->Token.type == TYPE_RIGHT_PAR) {
                            if ((result = checkTokenType(&data->Token, TYPE_EOL)) == 0) {

                                return result = statement_next(data);

                            } else if (result == 2 && data->Token.type == TYPE_EOF ) return SYNTAX_OK;
                            else return result;
                        } else if ((result = checkTokenType(&data->Token, TYPE_IDENTIFIER)) == 0 || data->Token.type == TYPE_INT ||
                                   data->Token.type == TYPE_STRING || data->Token.type == TYPE_FLOAT)
                            return result = ERROR_WRONG_NUMBER_OF_PARAMS;
                        else return result = ERROR_PARSER;
                    }
                    else {
                        if ((result = getNextToken(&data->Token)) != 0) return result;
                        else if (data->Token.type == TYPE_RIGHT_PAR) {
                            if ((result = checkTokenType(&data->Token, TYPE_EOL)) == 0) {

                                return result = statement_next(data);

                            } else if (result == 2 && data->Token.type == TYPE_EOF) return SYNTAX_OK;
                            else return result;
                        } else if (data->Token.type == TYPE_IDENTIFIER || data->Token.type == TYPE_INT ||
                        data->Token.type == TYPE_STRING || data->Token.type == TYPE_FLOAT) return result = ERROR_WRONG_NUMBER_OF_PARAMS;
                        /*else if(data->Token.type == TYPE_COMMA) {
                            if ((result = getNextToken(&data->Token)) != 0) return result;
                            if (data->Token.type == TYPE_IDENTIFIER || data->Token.type == TYPE_INT ||
                                data->Token.type == TYPE_STRING || data->Token.type == TYPE_FLOAT) return result = ERROR_WRONG_NUMBER_OF_PARAMS;
                            else return result = ERROR_PARSER;
                        } */
                        else return result = ERROR_PARSER;
                    }
                }
                    //ked s funkciou narabam ako s premennou e.g. foo = 2;
                else if (result == 2 && data->Token.type == TYPE_ASSIGN_VALUE) return ERROR_PROGRAM_SEMANTIC;
                else return result; ///neprisla mi zatvorka
        }
        else return ERROR_PROGRAM_SEMANTIC; //bol to identifier ale nic z tohto tu
        }




    else if (data->Token.type == TYPE_KEYWORD && data->Token.attribute.keyword == KEYWORD_PRINT) {
        static int result;
        if ((result = checkTokenType(&data->Token, TYPE_LEFT_PAR)) != 0) return result;
        //poslem do expr
/*  *   *   *   *   *   *   *   *   posielam expression do Expr.c   *   *   *   *   *   *   *   *   */
        while (data->Token.type != TYPE_RIGHT_PAR) {
            if ((result = getNextToken(&data->Token)) != 0) return result;
            if ((result = expression(data)) != 0 ){
                if (result == 2 && (data->Token.type == TYPE_RIGHT_PAR || data->Token.type == TYPE_COMMA)) continue;            ////vy ste toto nejako menili v expression?
                else return result;
            }
            //if (data->Token.type != TYPE_COMMA) return ERROR_PARSER;

        }
       // if ((result = expression(data)) != 0 ) return result;
/*  *   *   *   *   *   *   *   *   posielam expression do Expr.c   *   *   *   *   *   *   *   *   */
        if (data->Token.type == TYPE_RIGHT_PAR) {
            if ((result = getNextToken(&data->Token)) != 0) return result;
            return result = statement_next(data);
        }
        else return ERROR_PARSER;
    }




    else if (data->Token.type == TYPE_KEYWORD && data->Token.attribute.keyword == KEYWORD_PASS) {
        static int result;
        if ((result = getNextToken(&data->Token)) != 0) return result;
        return result = statement_next(data);
    }




    else if (data->Token.type == TYPE_KEYWORD && data->Token.attribute.keyword == KEYWORD_INPUTS) {
        static int result;
        if ((result = checkTokenType(&data->Token, TYPE_LEFT_PAR)) != 0) return result;
        if ((result = checkTokenType(&data->Token, TYPE_RIGHT_PAR)) != 0) return result;
        if (data->leftID != NULL) {
            data->leftID->type = DTYPE_STRING;
            data->leftID = NULL;
        }
        if ((result = getNextToken(&data->Token)) != 0) return result;
        return result = statement_next(data);
    }




    else if (data->Token.type == TYPE_KEYWORD && data->Token.attribute.keyword == KEYWORD_INPUTI) {
        static int result;
        if ((result = checkTokenType(&data->Token, TYPE_LEFT_PAR)) != 0) return result;
        if ((result = checkTokenType(&data->Token, TYPE_RIGHT_PAR)) != 0) return result;
        if (data->leftID != NULL) {
            data->leftID->type = DTYPE_INT;
            data->leftID = NULL;
        }
        if ((result = getNextToken(&data->Token)) != 0) return result;
        return result = statement_next(data);
    }




    else if (data->Token.type == TYPE_KEYWORD && data->Token.attribute.keyword == KEYWORD_INPUTF) {
        static int result;
        if ((result = checkTokenType(&data->Token, TYPE_LEFT_PAR)) != 0) return result;
        if ((result = checkTokenType(&data->Token, TYPE_RIGHT_PAR)) != 0) return result;
        if (data->leftID != NULL) {
            data->leftID->type = DTYPE_DOUBLE;
            data->leftID = NULL;
        }
        if ((result = getNextToken(&data->Token)) != 0) return result;
        return result = statement_next(data);
    }




    else if (data->Token.type == TYPE_KEYWORD && data->Token.attribute.keyword == KEYWORD_LEN) {
        static int result;
        if ((result = checkTokenType(&data->Token, TYPE_LEFT_PAR)) != 0) return result;
            if ((result = getNextToken(&data->Token)) != 0) return result;

        if (data->Token.type == TYPE_INT || data->Token.type == TYPE_FLOAT) return ERROR_ARTIHMETIC;

        else if (data->Token.type == TYPE_IDENTIFIER) {
            if (((data->rightID = htabSearch(&data->globalT, data->Token.attribute.string->str)) == NULL))
                if (((data->rightID = htabSearch(&data->localT, data->Token.attribute.string->str)) == NULL))
                    return ERROR_PROGRAM_SEMANTIC;

            if (data->rightID->type != DTYPE_STRING) return ERROR_ARTIHMETIC;
        }
        else if (data->Token.type == TYPE_STRING) {;}
        else return ERROR_PARSER;

        if (data->leftID != NULL) {
            data->leftID->type = DTYPE_INT;
            data->leftID = NULL;
        }
        if ((result = checkTokenType(&data->Token, TYPE_RIGHT_PAR)) != 0) return result;
            if ((result = getNextToken(&data->Token)) != 0) return result;
            if (( result = statement_next(data)) != 0) return result;
    }




    else if (data->Token.type == TYPE_KEYWORD && data->Token.attribute.keyword == KEYWORD_SUBSTR) {
        static int result;
        if ((result = checkTokenType(&data->Token, TYPE_LEFT_PAR)) != 0) return result;
        //zavolam expr
            int i = 0;
            while (i < 3) {
                switch (i) {
                    case 0:
                    {
                        if ((result = getNextToken(&data->Token)) != 0)
                            return result;
                        if (data->Token.type == TYPE_STRING) {
                            i++;
                            if ((result = checkTokenType(&data->Token, TYPE_COMMA)) != 0) return result;
                            break;
                        } else if (data->Token.type == TYPE_IDENTIFIER) {
                            if (((data->rightID = htabSearch(&data->globalT, data->Token.attribute.string->str)) ==
                                 NULL))
                                if (((data->rightID = htabSearch(&data->localT,
                                                                 data->Token.attribute.string->str)) == NULL))
                                    return ERROR_PROGRAM_SEMANTIC;
                            if (data->rightID->type != DTYPE_STRING) return ERROR_ARTIHMETIC;
                            i++;
                            if ((result = checkTokenType(&data->Token, TYPE_COMMA)) != 0) return result;
                            break;
                        } else if (data->Token.type != TYPE_STRING) return ERROR_ARTIHMETIC;
                        else return ERROR_PARSER;
                    }
                    case 1:
                    {
                        if ((result = getNextToken(&data->Token)) != 0)
                            return result;
                        if (data->Token.type == TYPE_INT) {
                            i++;
                            if ((result = checkTokenType(&data->Token, TYPE_COMMA)) != 0) return result;
                            break;
                        } else if (data->Token.type == TYPE_IDENTIFIER) {
                            if (((data->rightID = htabSearch(&data->globalT, data->Token.attribute.string->str)) ==
                                 NULL))
                                if (((data->rightID = htabSearch(&data->localT,
                                                                 data->Token.attribute.string->str)) == NULL))
                                    return ERROR_PROGRAM_SEMANTIC;
                            if (data->rightID->type != DTYPE_INT) return ERROR_ARTIHMETIC;
                            i++;
                            if ((result = checkTokenType(&data->Token, TYPE_COMMA)) != 0) return result;
                            break;
                        }  else if (data->Token.type != TYPE_INT) return ERROR_ARTIHMETIC;
                        else return ERROR_PARSER;
                    }
                    case 2:
                    {
                        if ((result = getNextToken(&data->Token)) != 0)
                            return result;
                        if (data->Token.type == TYPE_INT) {
                            i++;
                            break;
                        } else if (data->Token.type == TYPE_IDENTIFIER) {
                            if (((data->rightID = htabSearch(&data->globalT, data->Token.attribute.string->str)) ==
                                 NULL))
                                if (((data->rightID = htabSearch(&data->localT,
                                                                 data->Token.attribute.string->str)) == NULL))
                                    return ERROR_PROGRAM_SEMANTIC;
                            if (data->rightID->type != DTYPE_INT) return ERROR_ARTIHMETIC;
                            i++;
                            break;
                        }  else if (data->Token.type != TYPE_INT) return ERROR_ARTIHMETIC;
                        else return ERROR_PARSER;
                    }
                    default:
                        return ERROR_PARSER;
                }
            }
        if (data->leftID != NULL) {
            data->leftID->type = DTYPE_STRING;
            data->leftID = NULL;
        }
        if ((result = getNextToken(&data->Token)) != 0) return result;
        if (data->Token.type == TYPE_RIGHT_PAR) {
            if ((result = getNextToken(&data->Token)) != 0) return result;
            if ((result = statement_next(data)) != 0) return result;
        }
        else {return ERROR_PARSER;}
    }




    else if (data->Token.type == TYPE_KEYWORD && data->Token.attribute.keyword == KEYWORD_CHR) {
        static int result;
        if ((result = checkTokenType(&data->Token, TYPE_LEFT_PAR)) != 0) return result;
        //zavolam expr
        if ((result = getNextToken(&data->Token)) != 0) return result;

        if (data->Token.type == TYPE_STRING || data->Token.type == TYPE_FLOAT) return ERROR_ARTIHMETIC;

        else if (data->Token.type == TYPE_IDENTIFIER) {
            if (((data->rightID = htabSearch(&data->globalT, data->Token.attribute.string->str)) == NULL))
                if (((data->rightID = htabSearch(&data->localT, data->Token.attribute.string->str)) == NULL))
                    return ERROR_PROGRAM_SEMANTIC;

            if (data->rightID->type != DTYPE_INT) return ERROR_ARTIHMETIC;
        }
        else if (data->Token.type == TYPE_INT) {;}
        else return ERROR_PARSER;

        if (data->leftID != NULL) {
            data->leftID->type = DTYPE_STRING;
            data->leftID = NULL;
        }
        if ((result = checkTokenType(&data->Token, TYPE_RIGHT_PAR)) != 0) return result;
        if ((result = getNextToken(&data->Token)) != 0) return result;
        if (( result = statement_next(data)) != 0) return result;
    }




    else if (data->Token.type == TYPE_KEYWORD && data->Token.attribute.keyword == KEYWORD_ORD) {
        static int result;
        if ((result = checkTokenType(&data->Token, TYPE_LEFT_PAR)) != 0) return result;

        int i = 0;
        while (i < 2) {
            switch (i) {
                case 0: {
                    if ((result = getNextToken(&data->Token)) != 0)
                        return result;
                    if (data->Token.type == TYPE_STRING) {
                        i++;
                        if ((result = checkTokenType(&data->Token, TYPE_COMMA)) != 0) return result;
                        break;
                    } else if (data->Token.type == TYPE_IDENTIFIER) {
                        if (((data->rightID = htabSearch(&data->globalT, data->Token.attribute.string->str)) ==
                             NULL))
                            if (((data->rightID = htabSearch(&data->localT,
                                                             data->Token.attribute.string->str)) == NULL))
                                return ERROR_PROGRAM_SEMANTIC;
                        if (data->rightID->type != DTYPE_STRING) return ERROR_ARTIHMETIC;
                        i++;
                        if ((result = checkTokenType(&data->Token, TYPE_COMMA)) != 0) return result;
                        break;
                    } else if (data->Token.type != TYPE_STRING) return ERROR_ARTIHMETIC;
                    else return ERROR_PARSER;
                }
                case 1: {
                    if ((result = getNextToken(&data->Token)) != 0)
                        return result;
                    if (data->Token.type == TYPE_INT) {
                        i++;
                        if ((result = checkTokenType(&data->Token, TYPE_COMMA)) != 0) return result;
                        break;
                    } else if (data->Token.type == TYPE_IDENTIFIER) {
                        if (((data->rightID = htabSearch(&data->globalT, data->Token.attribute.string->str)) ==
                             NULL))
                            if (((data->rightID = htabSearch(&data->localT,
                                                             data->Token.attribute.string->str)) == NULL))
                                return ERROR_PROGRAM_SEMANTIC;
                        if (data->rightID->type != DTYPE_INT) return ERROR_ARTIHMETIC;
                        i++;
                        if ((result = checkTokenType(&data->Token, TYPE_COMMA)) != 0) return result;
                        break;
                    } else if (data->Token.type != TYPE_INT) return ERROR_ARTIHMETIC;
                    else return ERROR_PARSER;
                }
                default:
                    return ERROR_INTERN;
            }
        }
        if (data->leftID != NULL) {
            data->leftID->type = DTYPE_STRING;
            data->leftID = NULL;
        }
        if ((result = checkTokenType(&data->Token, TYPE_RIGHT_PAR)) != 0) return result;
        if ((result = getNextToken(&data->Token)) != 0) return result;
        if (( result = statement_next(data)) != 0) return result;

    }




/*	*   overujem ci nahodou nenastala situacia s komentom alebo je tam len prosté EOL   *   */
    else if ((result = isComment(data)) == 0) return statement_next(data);
/*  *   *  koniec zacyklenia <statement> a <statement_next> -> vrati sa do <prog>   *   *   */
    else if(data->Token.type == TYPE_EOF || data->Token.type == TYPE_DEDENT)    ///latest change BE AWARE                                   ///latest change BE AWARE
        return result = SYNTAX_OK;                                             ///latest change BE AWARE
    else if (data->Token.attribute.keyword == KEYWORD_DEF)                  ///latest change BE AWARE
        return result = prog(data);                                        ///latest change BE AWARE                                       ///latest change BE AWARE
    return result;
}


/*                                                                                          *
*                       10.  <statement_next> -> TYPE_EOL <statement>                       *
*                                                                                           */
static int statement_next(ParserData *data) 
{
	static int result;
    if ((result = isComment(data)) == 0 ) {
        //ak bol eol ocakavam dalsi statement (tam sa mi to ked tak vrati spat do progu)
        if ((result = getNextToken(&data->Token)) == 0)
            return result = statement(data);
        else return result;
    }
/*  *   *   *   *   *   *   *    25.  <statement_next>  ->  ε   *   *   *   *   *   *   *   */
/*  *   *  koniec zacyklenia <statement> a <statement_next> -> vrati sa do <prog>   *   *   */
    else if(data->Token.type == TYPE_EOF || data->Token.type == TYPE_DEDENT)  ///latest change BE AWARE                        ///latest change BE AWARE
        return result = SYNTAX_OK;                                           ///latest change BE AWARE
    else if (data->Token.attribute.keyword == KEYWORD_DEF)                  ///latest change BE AWARE
    	return result = prog(data);                                        ///latest change BE AWARE
    //BE AWARE OF THIS, COULD BE SHIT
    else if (data->Token.type == TYPE_KEYWORD || data->Token.type == TYPE_IDENTIFIER) return result = statement(data);
    else if ((result = checkTokenType(&data->Token, TYPE_EOL)) == 0) return result = statement(data);
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
	data->in_if = 0;
	data->in_while = 0;
	//data->not_declared_function = 0;

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

/*	// SubStr(s =string, i =Integer, n =Integer) returns String
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
*/
/*	// Ord(s =String, i =Integer) returns Integer
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
		return ERROR_INTERN;*/

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
	int result = 0;

	string parserStr;
	if (stringInit(&parserStr))
		return ERROR_INTERN;
	setString(&parserStr);

	ParserData data;
	if (variablesInit(&data) == ERROR_INTERN)
	{
		stringStrFree(&parserStr);
		return ERROR_INTERN;
	}

	//if ((result = getNextToken(&data.Token)) == TOKEN_OK) ->tohle uz nedelaaam

	{

		//nemame nic z generatoru zatial
		if (!generateCode())
		{
			stringStrFree(&parserStr);
			variablesFree(&data);
			return ERROR_INTERN;
		}
		result = prog(&data);
	}
	stringStrFree(&parserStr);
	variablesFree(&data);

	return result;
}

/***************UŽ LEN LADÍM*************I*ENDED*UP*HERE*****************************************
*               __________                   ________    _______    .        .                  *
*                   |       |       |       |           |       |   |\      /|                  *
*                   |       |       |       |_______    |       |   | \    / |                  *
*                   |       |       |               |   |       |   |  \  /  |                  *
*                   |       |_______|       ________|   |_______|   |   \/   |                  *
*                                                                                               *
*****************************************TU*SOM*SKONČIL****UŽ LEN LADÍM**************************/