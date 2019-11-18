/*
 *	Predmet : Formální jazyky a překladače									 
 *	Projekt : Implementace překladače imperativního jazyka IFJ19			 
 *	Súbor	: expression.h														 	 
 *	Tím		: číslo 10 / varianta II										 
 *	Autori	: Dominik Boboš (xbobos00)										 
 *			  Peter Hudeček (xhudec34)										 
 *			  Timotej Kováčik (xkovac49)									 
 *			  Dávid Oravec (xorave05)										 
 *																			 
 */


#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include "parser.h"


typedef enum
{
	RULE_GRATER, //	>
	RULE_LESS, // 	< 
	RULE_GRATEREQ, //	>=  
	RULE_LESSEQ, //<=  
	RULE_EQ, // ==
	RULE_NOTEQ, // !=
	RULE_MIN, // -
	RULE_PLUS, // +
	RULE_MUL, // *
	RULE_INTDIV, // '//' celociselne delenie
	RULE_PARS, // ()
} prec_rules;


typedef enum
{
	PLUS,	// +
	MINUS,   // -
	MULTIPLY, // *
	DIVIDE,	  // /
	DIVIDE_INT, // //
	ASSIGN_VALUE, // =
	GRATER_THAN,		// >
	LESS_THAN,
	EQUALS,  // ==
	NOT_EQUAL, // !=
	LEFT_PAR, // (
	RIGHT_PAR, // )
	INT_N,
	FLOAT_N,
	STRING_N,
	IDENTIFIER, // Identifier

	//TODO este zopar bude asi missing
}prec_symbols;


int expression(/*TODO*/)


#endif
