/*
 *	Predmet : Formální jazyky a překladače									 
 *	Projekt : Implementace překladače imperativního jazyka IFJ19			 
 *	Súbor	: expression.c												 
 *	Tím		: číslo 10 / varianta II										 
 *	Autori	: Dominik Boboš (xbobos00)										 
 *			  Peter Hudeček (xhudec34)										 
 *			  Timotej Kováčik (xkovac49)							 
 *			  Dávid Oravec (xorave05)								 
 *																	 
 */


#include "expression.h"
#include "symstack.h"
#include "scanner.h"
#include "error.h"
//#include "code_gen.h" este neni

typedef enum
{
	PLUS_MINUS_IND,
	MUL_DIV_IDIV_IND,
	COMPARE_IND,
	LEFT_PAR_IND,
	RIGHT_PAR_IND,
	I_IND,
	DOLLAR_IND;
}prec_tab_ind;

typedef enum
{
	EQ,		// = EQUAL
	SH,		// <
	RE, 	// >
	ER,		// error
}prec_sign;

int prec_tab[7][7]=
{
//		+-,*///,cmp, ( , ) , i , $
	   {RE, SH,  RE, SH, RE, SH, RE}; // +-
	   {RE, RE,  RE, SH, RE, SH, RE}; // * / //
	   {SH, SH,  ER, SH, RE, SH, RE}; // cmp
	   {SH, SH,  SH, SH, EQ, SH, ER}; // (
	   {RE, RE,  RR, ER, SH, ER, RE}; // )
	   {RE, RE,  RE, ER, RE, ER, RE}; // i
	   {SH, SH,  SH, SH, ER, SH, ER}; // $
}

//priradenie hodnoty symbolom z tabulky (tie ktore su spolu v casi 
//su v tom istom riadku v tabulke)
static prec_tab_ind assign_prec_tab_ind(prec_table_sym symbol)
{
	switch(symbol)
	{
		// +-
		case PLUS_SYM:
		case MINUS_SYM:
		return PLUS_MINUS_IND;

		// * / //
		case MUL_SYM:
		case DIV_SYM:
		case INT_DIV_SYM: 		//chyba v expr.h
		return MUL_DIV_IDIV_IND;

		//compare znaky
		case EQ_SYM:
		case NOT_EQ_SYM:
		case LESS_EQ_SYM:
		case LESS_SYM:
		case MORE_SYM:
		case MORE_EQ_SYM:
		return COMPARE_IND;

		//i -> datove typy
		case ID_SYM:
		case INT_SYM:
		case FLOAT_SYM:
		case STR_SYM:
		return I_IND;

		//lava zatvorka
		case LEST_PAR:
		return LEFT_PAR_IND;

		//prava zatvorka
		case RIGHT_PAR:
		return RIGHT_PAR_IND;

		//default
		default:
		return DOLLAR_IND;
	}
}

// konverzia tokenu zo scanneru na symbol
static prec_table_sym tok_to_sym(Token* token)
{
	switch(token->type)
	{
		case TYPE_PLUS:
			return PLUS_SYM;
		case TYPE_MINUS:
			return MINUS_SYM;


		case TYPE_MULTIPLY:
			return MUL_SYM;
		case TYPE_DIVIDE:
			return DIV_SYM;
		case TYPE_DIVIDE_INT:
			return INT_DIV_SYM;


		//case TYPE_ASSIGN_VALUE:
		//	return EQ_SYM;


		case TYPE_GREATER_THAN:
			return MORE_SYM;
		case TYPE_LESS_THAN:
			return LESS_SYM;
		case TYPE_GRATER_EQUAL:
			return MORE_EQUAL_SYM;
		case TYPE_LESS_EQUAL:
			return LESS_EQ_SYM;
		case TYPE_EQUALS:
			return EQ_SYM;
		case TYPE_NOT_EQUAL:
			return NOT_EQ_SYM;

		case TYPE_LEFT_PAR:
			return LEFT_PAR_SYM;

		case TYPE_RIGHT_PAR:
			return RIGHT_PAR_SYM;

		case TYPE_IDENTIFIER:
			return ID_SYM;

		case TYPE_INT:
			return INT_SYM;
		case TYPE_FLOAT:
			return FLOAT_SYM;
		case TYPE_STRING:
			return STR_SYM;

		default:
		return DOLLAR_SYM;
	}
}