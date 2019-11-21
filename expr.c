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


#include "expr.h"
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
	DOLLAR_IND,
}prec_tab_ind;

typedef enum
{
	EQ,		// = EQUAL
	SH,		// <
	RE, 	// >
	ER,		// error
}prec_sign;

int prec_tab[7][7]={
//		+-,*///,cmp, ( , ) , i , $
	   {RE, SH,  RE, SH, RE, SH, RE}, // +-
	   {RE, RE,  RE, SH, RE, SH, RE}, // * / //
	   {SH, SH,  ER, SH, RE, SH, RE}, // cmp
	   {SH, SH,  SH, SH, EQ, SH, ER}, // (
	   {RE, RE,  RE, ER, SH, ER, RE}, // )
	   {RE, RE,  RE, ER, RE, ER, RE}, // i
	   {SH, SH,  SH, SH, ER, SH, ER}, // $
};

sstack* stack;

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
		case LEFT_PAR:
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
		case TYPE_GREATER_EQUAL: // =>
			return MORE_EQ_SYM;
		case TYPE_LESS_EQUAL: // <=
			return LESS_EQ_SYM;
		case TYPE_EQUALS: // ==
			return EQ_SYM;
		case TYPE_NOT_EQUAL: // <>
			return NOT_EQ_SYM;

		case TYPE_LEFT_PAR: // (
			return LEFT_PAR;

		case TYPE_RIGHT_PAR: // )
			return RIGHT_PAR;

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
	Data* symbol;

	switch (data->Token.type) //udaje z parseru
	{
		case TYPE_INT:
			return DTYPE_INT;

		case TYPE_STRING:
			return DTYPE_STRING;

		case TYPE_FLOAT:
			return DTYPE_DOUBLE;

		case TYPE_IDENTIFIER:
		    symbol = htabSearch(*data->localT, data->Token.attribute.string);
			if(symbol == NULL)
			    return DTYPE_UNDEFINED;
			return symbol->type;

		default: DTYPE_UNDEFINED;
	}
}

//kontroluje spravnost zapisania pravidiel a urcije ktora z 
//pravidiel vypisanych hore splna podmienky -> vzdy len jedna alebo NOT_A_RULE
static Prec_rules prec_rules_syntax(int count, s_item* item1, s_item* item2, s_item* item3)
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


static int prec_rule_semantics (Prec_rules rule, s_item* item1, s_item* item2, s_item* item3, DataType* final) // DataType z funkcie hore
{
	//pomocne bool premenne na pretypovavanie
	bool item1_double = false;
	bool item1_integer = false;
	bool item3_double = false;
	bool item3_integer = false;
	// E
	if (rule == OPERAND)
	{
		*final = item1->data_type;
	}

	// (E)
	if(rule == LBR_NT_RBR)
	{
		*final = item2->data_type;
	}

	// +
	if (rule == NT_PLUS_NT)
	{
		// david + domino
		if (item1->data_type == DTYPE_STRING && item3->data_type == DTYPE_STRING)
		{
			*final = DTYPE_STRING;
			break;
		}

		// 6 + 6 
		if (item1->data_type == DTYPE_INT && item3->data_type == DTYPE_INT)
		{
			*final = DTYPE_INT;
			break;
		}

		if  (item)
	}

	if (rule == NT_IDIV_NT)
	{
		*final = DTYPE_INT;

		if(item1->data_type == DTYPE_STRING)
		{
			//return error
		}
		if(item3->data_type == DTYPE_STRING)
		{
			//return error
		}

		if (item1->data_type == DTYPE_DOUBLE)
		{
			item1_integer = true;
		}

		if (item3->data_type == DTYPE_DOUBLE)
		{
			item3_integer = true;
		}

	}
}

int expression(ParserData *data)
{
    //Inicializacia symstacku a potrebnych premennych
    symbol_init(stack);
    symbol_push(stack, DOLLAR_SYM, DTYPE_UNDEFINED); //pociatocny item pre kontrolu prazdnoty stacku
    s_item* sym_on_top;
    prec_table_sym sym_in_token;
    int result;

    do{
        //priradenie hodnot
        sym_on_top = symbol_top(stack);
        sym_in_token = tok_to_sym(&data->Token);

        switch(prec_tab[assign_prec_tab_ind(sym_on_top->symbol)][assign_prec_tab_ind(sym_in_token)])
             {   //PRI EQUAL PUSHNEM NA STACK A VYPYTAM TOKEN
                case EQ:
                    symbol_push(stack,sym_in_token, token_to_data(data));
                    if((result = getNextToken(&data->Token)))
                        return result;
                    //PRI SHIFTE PRIDAM STOP ZARAZKU PRE REDUKOVANIE, PUSHNEM NA STACK A VYPYTAM TOKEN
                 case SH:
                     symbol_push(stack,STOP,DTYPE_UNDEFINED);
                     symbol_push(stack,sym_in_token, token_to_data(data));
                     if((result =getNextToken(&data->Token)))
                         return result;
                 case RE:
                //TODO TREBA FUNKCIU NA REDUKCIU CEZ PRAVIDLA
         }
        //dokym nebude vsetko spracovane tak cyklim
    } while( sym_on_top->symbol == DOLLAR_SYM && sym_in_token == DOLLAR_SYM);
}
