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

#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>
#include "str.h"
#include "error.h"
#include "stack.h"

typedef enum {
    STATE_START,                    // startovaci stav {S}      0
    STATE_ID_OR_KEYWORD,            // konecny stav {F20}       1
    STATE_INT,                      // konecny stav intu {F1}   2
    STATE_FLOAT,                    // konecny stav {F3}        3
    STATE_FLOAT_POINT,              // medzistav {Q1}           4
    STATE_FLOAT_POINT_NUMBER,       // konecny stav {F2}        5
    STATE_FLOAT_EXP,                // medzisstav {Q2}          6
    STATE_FLOAT_EXP_OP,                // medzistav {Q3}        7
    STATE_STRING_START,                // medzistav {Q6}        8
    STATE_BACKSLASH,                // medzistav {Q7}           9
    STATE_BACKSLASH_X,                // medzistav {Q8}         10
    STATE_BACKSLASH_X_NUM,            // medzistav {Q9}         11
    STATE_DIVIDE,                    // konecny stav {F7}       12
    STATE_GREATER_THAN,                // konecny stav {F11}    13
    STATE_LESS_THAN,                // konecny stav {F9}        14
    STATE_EQUALS,                    // konecny stav {F17}      15
    STATE_NOT_EQUAL_START,            // medzistav {Q4}         16
    STATE_COMMENT,                    // medzistav {Q5}         17
    STATE_BLOCK_STRING_START,        // medzistav {Q10}        18
    STATE_BLOCK_STRING_CONTINUE,    // medzistav {Q11}         19
    STATE_BLOCK_STRING,            // medzistav {Q12}          20
    STATE_BLOCK_STRING_LEAVE_TRY,    // medzistav {Q13}        21
    STATE_BLOCK_STRING_LEAVE,        // medzistav {Q14}        22
    STATE_EOL,                      //  konecny stav {F22}     23
    STATE_INDENT,                   //  konecny stav {F23}     24
    STATE_DEDENT,                   //  konecny stav {F24}     25
    STATE_INDENT_OR_DEDENT,         //  medzistav (Q15}        26
    STATE_BLOCK_STRING_ESC,         //
} state;

typedef enum {
    KEYWORD_DEF,                    //def                       0
    KEYWORD_ELSE,                   //else                      1
    KEYWORD_IF,                     //if                        2
    KEYWORD_NONE,                   //None                      3
    KEYWORD_PASS,                   // nic nerobi               4
    KEYWORD_RETURN,                 //return                    5
    KEYWORD_WHILE,                  //while                     6
    KEYWORD_INPUTS,                 //inputs funkcia            7
    KEYWORD_INPUTI,                 //inputi funkcia            8
    KEYWORD_INPUTF,                 //inputf funkcia            9
    KEYWORD_PRINT,                  //print funkcia             10
    KEYWORD_LEN,                    //len funkcia               11
    KEYWORD_SUBSTR,                 //substr funkcia            12
    KEYWORD_CHR,                    //chr funkcia               13
    KEYWORD_ORD,                    //ord funkcia               14
} Keyword;

typedef enum
{
    /*
        Datove typy
    */
    TYPE_INT,                       //int               0
    TYPE_FLOAT,                     //actually double   1
    TYPE_STRING,                    //string            2
    /*
        ID / Keyword / OtherStuff
    */
    TYPE_IDENTIFIER,                // Identifier       3
    TYPE_KEYWORD,                   // Keyword          4
    TYPE_EOL,                       // typ EOL          5
    TYPE_EOF,                       // typ EOF          6
    TYPE_EMPTY,                     // pociatocny stav  7
    /*
        matematicke operacie
    */
    TYPE_PLUS,                      //	'+'             8
    TYPE_MINUS,                     //	'-'             9
    TYPE_MULTIPLY,                  //	'*'             10
    TYPE_DIVIDE,                    //	'/'             11
    TYPE_DIVIDE_INT,                //	'//'            12
    TYPE_ASSIGN_VALUE,              // '='              13
    /*
        logicke operacie
    */
    TYPE_GREATER_THAN,              //	>               14
    TYPE_LESS_THAN,                 // 	<               15
    TYPE_GREATER_EQUAL,             //	>=              16
    TYPE_LESS_EQUAL,                //  <=              17
    TYPE_EQUALS,                    //  ==              18
    TYPE_NOT_EQUAL,                 //  !=              19
    /*
        Znaky + Space
    */
    TYPE_LEFT_PAR,                   //	'('             20
    TYPE_RIGHT_PAR,                  //	')'             21
    TYPE_COMMA,                      //	','             22
    TYPE_COLON,                      //	':'             23
    TYPE_INDENT,                     //typ INDENT       24
    TYPE_DEDENT,                     //typ DEDENT       25
    TYPE_COMMENT,                    //dokumentacny     26
} token_type;

typedef struct {
    string *string;
    int int_value;
    double decimal_value;
    Keyword keyword;
} token_attribute;

typedef struct {
    token_type type;
    token_attribute attribute;
} token;

int getNextToken(token *token);
void sourceFile(FILE *f);
void setString(string *s);

#endif    // _SCANNER_H