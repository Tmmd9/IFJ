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
	STATE_START,					// startovaci stav {S}
	STATE_ERROR = -1,				// lexikalna chyba
	STATE_ID_OR_KEYWORD,			// konecny stav {F20}
	STATE_INT,						// konecny stav intu {F1}
	STATE_FLOAT,					// konecny stav {F3}
	STATE_FLOAT_POINT,				// medzistav {Q1}
	STATE_FLOAT_POINT_NUMBER,		// konecny stav {F2}
	STATE_FLOAT_EXP,				// medzisstav {Q2}
	STATE_FLOAT_EXP_OP,				// medzistav {Q3}
	STATE_STRING,					// konecny stav {F21}
	STATE_STRING_START,				// medzistav {Q6}
	STATE_BACKSLASH,				// medzistav {Q7}
	STATE_BACKSLASH_X,				// medzistav {Q8}
	STATE_BACKSLASH_X_NUM,			// medzistav {Q9}		
	STATE_PLUS,						// konecny stav {F5}
	STATE_MINUS,					// konecny stav {F4}
	STATE_MULTIPLY,					// konecny stav {F6}
	STATE_DIVIDE,					// konecny stav {F7}
	STATE_DIVIDE_INT,				// konecny stav {F8}
	STATE_GREATER_THAN,				// konecny stav {F11}
	STATE_LESS_THAN,				// konecny stav {F9}
	STATE_GREATER_EQUAL,			// konecny stav {F12}
	STATE_LESS_EQUAL,				// konecny stav {F10}
	STATE_EQUALS,					// konecny stav {F17}
	STATE_NOT_EQUAL_START,			// medzistav {Q4}
	STATE_NOT_EQUAL,				// konecny stav {F13}
	STATE_EQUAL_TO,					// konecny stav {F18}
	STATE_SEMICOLON,				// konecny stav {F14}
	STATE_COMMENT,					// medzistav {Q5}
	STATE_LEFT_PAR,					// konecny stav {F15}
	STATE_RIGHT_PAR,				// konecny stav {F16}
	STATE_COMMA,					// konecny stav {F19}
	STATE_BLOCK_COMMENT_START,		// medzistav {Q10}
	STATE_BLOCK_COMMENT_CONTINUE,	// medzistav {Q11}
	STATE_BLOCK_COMMENT,			// medzistav {Q12}
	STATE_BLOCK_COMMENT_LEAVE_TRY,	// medzistav {Q13}
	STATE_BLOCK_COMMENT_LEAVE,		// medzistav {Q14}
	STATE_EOL,						// konecny stav {F22}
} state;

typedef struct {
	//TODO
} token;

token getNextToken();

#endif	// !_SCANNER_H_
