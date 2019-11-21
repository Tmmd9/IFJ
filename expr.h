// *	Predmet : Formální jazyky a překladače
// *	Projekt : Implementace překladače imperativního jazyka IFJ19
// *	Súbor	: expr.h
// *				-hlavičkový súbor pre modul na parsovanie expressions (syntaktická analýza)
// *	Tím		: číslo 10 / varianta II
// *	Autori	: Dominik Boboš (xbobos00)
// *			  Peter Hudeček (xhudec34)
// *			  Timotej Kováčik (xkovac49)
// *			  Dávid Oravec (xorave05)
// *



#ifndef _EXPR_H
#define _EXPR_H

#include "parser.h"

//symboly pre precedencnu analyzu
typedef enum
{
	ID_SYM,				///1 ID
	INT_SYM,			///2 int
	FLOAT_SYM,			///3 double
	STR_SYM,			///4 string
	PLUS_SYM,			///5 +
	MINUS_SYM,			///6 -
	MUL_SYM,			///7 *
	DIV_SYM,			///8 /
	INT_DIV_SYM,		///9 //
	LEFT_PAR,			///10 (
	RIGHT_PAR,			///11 )
	EQ_SYM,				///12 =
	NOT_EQ_SYM,			///13 <>
	LESS_EQ_SYM,		///14 <=
	LESS_SYM,			///15 <
	MORE_EQ_SYM,		///16 >=
	MORE_SYM,			///17 >
    DOLLAR_SYM,			///18 $	//eol a ':' (then)
    STOP,				///19 stop symbol used when reducing
    NON_TERM,			///20 non-terminal
} prec_table_sym;



////pravidla pouzivane pre parsing
typedef enum
{
	OPERAND,		///1 E -> i
	NOT_A_RULE		///2 rule doesn't exist
	NT_EQ_NT,		///3 E -> E = E
	NT_NEQ_NT,		///4 E -> E <> E
	NT_LEQ_NT,		///5 E -> E <= E
	NT_LTN_NT,		///6 E -> E < E
	NT_MEQ_NT,		///7 E -> E => E
	NT_MTN_NT,		///8 E -> E > E
	NT_PLUS_NT,		///9 E -> E + E
  NT_MINUS_NT,	///10 E -> E - E
	NT_IDIV_NT,		///11 E -> E \ E
	NT_MUL_NT,		///12 E -> E * E
	NT_DIV_NT,		///13 E -> E / E
	LBR_NT_RBR,		///14 E -> (E)
} Prec_rules;



//	implementacia pravidiel pre vyrazy (expressions),
//	parsuje vyrazy data su pointer na strukturu
//	definovanu v parser.h uchovavajuca interne data

//main funkcia co sa bude volat pri vyrazoch
int expression(ParserData* data);

#endif //_EXPR_H
