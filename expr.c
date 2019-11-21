/*
 *	Predmet : Formální jazyky a překladače									 
 *	Projekt : Implementace překladače imperativního jazyka IFJ19			 
 *	Súbor	: expr.c											 
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
		case INT_DIV_SYM: 
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
static prec_table_sym tok_to_sym(token* token)
{
	switch(token->type)
	{
		case TYPE_PLUS: // +
			return PLUS_SYM;
		case TYPE_MINUS: // -
			return MINUS_SYM;


		case TYPE_MULTIPLY: // *
			return MUL_SYM;
		case TYPE_DIVIDE: // /
			return DIV_SYM;
		case TYPE_DIVIDE_INT: // //
			return INT_DIV_SYM;


		//case TYPE_ASSIGN_VALUE:
		//	return EQ_SYM;


		case TYPE_GREATER_THAN: // >
			return MORE_SYM;
		case TYPE_LESS_THAN: // <
			return LESS_SYM;
		case TYPE_GRATER_EQUAL: // =>
			return MORE_EQUAL_SYM;
		case TYPE_LESS_EQUAL: // <=
			return LESS_EQ_SYM;
		case TYPE_EQUALS: // ==
			return EQ_SYM;
		case TYPE_NOT_EQUAL: // <>
			return NOT_EQ_SYM;

		case TYPE_LEFT_PAR: // (
			return LEFT_PAR_SYM;

		case TYPE_RIGHT_PAR: // )
			return RIGHT_PAR_SYM;

		case TYPE_IDENTIFIER: // ID
			return ID_SYM;

		case TYPE_INT: // integer
			return INT_SYM;
		case TYPE_FLOAT: // float // ale vlastne DOUBLE
			return FLOAT_SYM;
		case TYPE_STRING: // string
			return STR_SYM;

		default:
		return DOLLAR_SYM;
	}
}

// vrati datovy typ tokenu passnuteho z parseru
//v pripade vratenia ID_SYM or INT_SYM or FLOAT_SYM or STR_SYM

static /* zmenit*/DataType token_to_data(ParserData* data)
{
	TokenData* symbol;

	switch (data->Token.type) //udaje z parseru
	{
		case TYPE_INT:
			return D_TYPE_INT;

		case TYPE_STRING:
			return D_TYPE_STRING;

		case TYPE_FLOAT:
			return D_TYPE_FLOAT;

		case TYPE_IDENTIFIER:
			return D_TYPE_IDENTIFIER;

		default: D_TYPE_UNDEFINED;
	}
}

//kontroluje spravnost zapisania pravidiel a urcije ktora z 
//pravidiel vypisanych hore splna podmienky -> vzdy len jedna alebo NOT_A_RULE
static prec_rule prec_rules_syntax(int count, symbol_item* item1, symbol_item* item2, symbol_item* item3)
{
	//ked sa nachadza len jeden symbol -> operand
	if(count == 1)
	{
		if(item1->symbol == ID_SYM   ||
		   item1->symbol == INT_SYM  ||
		   item1->symbol == FLOAT_SYM||
		   item1->symbol == STR_SYM)
		{
			return OPERAND;
		} 
		else NOT_A_RULE;
	}

	//ked sa najdu 3 symboly mame viac moznosti vyberu
	if(count == 3)
	{
		// dva operandy a medzi nimi operator
		if(item1->symbol == NON_TERM && item3->symbol == NON_TERM)
		{
			//O + O
			if(item2->symbol == PLUS_SYM)
				return NT_PLUS_NT;

			// O - O
			if(item2->symbol == MINUS_SYM)
				return NT_MINUS_NT;

			// O = O
			if(item2->symbol == EQ_SYM)
				return NT_EQ_NT;

			// O <> O
			if(item2->symbol == NOT_EQ_SYM)
				return NT_NEQ_NT;

			// O <= O
			if(item2->symbol == LESS_EQ_SYM)
				return NT_LEQ_NT;

			// O < O
			if(item2->symbol == LESS_SYM)
				return NT_LTN_NT;

			// O => O
			if(item2->symbol == MORE_EQ_SYM)
				return NT_MEQ_NT;

			// O > O
			if(item2->symbol == MORE_SYM)
				return NT_MTN_NT;

			// O // O
			if(item2->symbol == INT_DIV_SYM)
				return NT_IDIV_NT;

			// O * O
			if(item2->symbol == MUL_SYM)
				return NT_MUL_NT;

			// O / O
			if(item2->symbol == DIV_SYM)
				return NT_DIV_NT;
		}

		// (O) pravidlo s operandom v zatvorkach
		if (item1->symbol == LEFT_PAR && item2->symbol == NON_TERM && item3->symbol == RIGHT_PAR)
		{
			return LBR_NT_RBR;
		}

		//nesplna ziadnu z podmienok pravidiel -> vracia NOT_A_RULE
		else
		{
			return NOT_A_RULE;
		}
	}
}


static int prec_rule_semantics (prec_rule rule, symbol_item* item1, symbol_item* item2, symbol_item* item3, DataType* final) // DataType z funkcie hore
{
	//pomocne bool premenne na pretypovavanie
	bool item1_double = false;
	bool item1_integer = false;
	bool item3_double = false;
	bool item3_integer = false;
	// E
	if (rule == OPERAND)
	{
		*final = item1->DataType;
	}

	// (E)
	if(rule == LBR_NT_RBR)
	{
		*final = item2->DataType;
	}

	// +
	if (rule == NT_PLUS_NT)
	{
		// david + domino
		if (item1->DataType == D_TYPE_STRING && item3->DataType == D_TYPE_STRING)
		{
			*final = D_TYPE_STRING;
			break;
		}

		// 6 + 6 
		if (item1->DataType == D_TYPE_INTEGER && item3->DataType == D_TYPE_INTEGER)
		{
			*final = D_TYPE_INTEGER;
			break;
		}

		if  (item)
	}

	if (rule == NT_IDIV_NT)
	{
		*final = D_TYPE_INT;

		if(item1->DataType == D_TYPE_STRING)
		{
			//return error
		}
		if(item3->DataType == D_TYPE_STRING)
		{
			//return error
		}

		if (item1->DataType == D_TYPE_FLOAT)
		{
			item1_integer = true;
		}

		if (item3->DataType == D_TYPE_FLOAT)
		{
			item3_integer = true;
		}

	}
}
