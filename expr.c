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
#include <stdlib.h>
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

sstack* symStack;

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

static DataType token_to_data(ParserData* data)
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
		    symbol = htabSearch(&data->localT, data->Token.attribute.string->str);
		    //Ak sa nenajde v lokalnej tabulke pozrem ci neni definovana v globalnej
			if(symbol == NULL){
                symbol = htabSearch(&data->globalT, data->Token.attribute.string->str);
                if(symbol == NULL)
			        return DTYPE_UNDEFINED;
                return  symbol->type;
			}
			return symbol->type;

		default: return DTYPE_UNDEFINED;
	}
}

static int stop_count(bool* stop_found)
{
	s_item* top_sym = symbol_top(symStack); // vrchny symbol zo stacku
 	int n = 0;

 	while (top_sym)
 	{
 		if(top_sym->symbol == STOP)
 		{
 			*stop_found = true; // stop bol najdeny .. break
 			break;
 		}
 		else if(top_sym->symbol != STOP)
 		{
 			*stop_found = false; //stop nebol najdeny.. inkrementacia n
 			n++;
 		}
 		top_sym = top_sym->next; //posunutie na dalsi prvok
 	}
 	return n; // vracia poziciu STOP symbolu
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
		else return NOT_A_RULE;
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
    return NOT_A_RULE;
}


static int prec_rule_semantics (Prec_rules rule, s_item* item1, s_item* item2, s_item* item3, DataType* final) // DataType z funkcie hore
{
	//pomocne bool premenne na pretypovavanie
	bool item1_double = false;
	bool item3_double = false;

	//osetrenie spravnosti pre (E)
	if (rule == LBR_NT_RBR)
	{
		if(item2->data_type == DTYPE_UNDEFINED)
		{
			return ERROR_PROGRAM_SEMANTIC;//nedefinovana premenna
		}
	}

	//osetrenie spravnosti pre operand
	if (rule == OPERAND && item1->data_type == DTYPE_UNDEFINED)
	{
		return ERROR_PROGRAM_SEMANTIC; // nedefinovana premenna
	}

	if (rule == OPERAND && item1->data_type == DTYPE_BOOL)
	{
		return ERROR_ARTIHMETIC;
	}

	if (rule != LBR_NT_RBR) {
        if (rule != OPERAND) {
            if (item1->data_type == DTYPE_UNDEFINED)
                return ERROR_PROGRAM_SEMANTIC;
            if (item3->data_type == DTYPE_UNDEFINED)
                return ERROR_PROGRAM_SEMANTIC;
            if (item1->data_type == DTYPE_BOOL)
                return ERROR_ARTIHMETIC;
            if (item3->data_type == DTYPE_BOOL)
                return ERROR_ARTIHMETIC;
        }
    }



	// E
	if (rule == OPERAND)
	{
		*final = item1->data_type;
	}

	// (E)
	else if(rule == LBR_NT_RBR)
	{
		*final = item2->data_type;
	}

	/*// pravidla pre +
	if (rule == NT_PLUS_NT)
	{
		// david + domino
		if (item1->data_type == DTYPE_STRING && item3->data_type == DTYPE_STRING)
		{
			*final = DTYPE_STRING;
		}
		// 6 + 6
		else if (item1->data_type == DTYPE_INT && item3->data_type == DTYPE_INT)
		{
			*final = DTYPE_INT;
		}
		else *final = DTYPE_DOUBLE;
		if (item1->data_type == DTYPE_INT)
	}*/


	//pravidla pre -
	else if (rule == NT_MINUS_NT || rule == NT_MUL_NT || rule == NT_PLUS_NT)
	{

		//dva stringy sa daju len +sovat...
		if (item1->data_type == DTYPE_STRING && item3->data_type == DTYPE_STRING && rule == NT_PLUS_NT)
		{
			*final = DTYPE_STRING;
		}

		//.. ostatne davaju err
		else if (item1->data_type == DTYPE_STRING || item3->data_type == DTYPE_STRING)
			return ERROR_ARTIHMETIC;

		//integer +(ale aj - a *, dalej uz len +) integer = INTEGER
		else if(item1->data_type == DTYPE_INT && item3->data_type == DTYPE_INT)
			*final = DTYPE_INT;

		//double + int = retype int na DOUBLE
		else if(item1->data_type == DTYPE_DOUBLE && item3->data_type == DTYPE_INT)
		{
			item3_double = true;
			*final = DTYPE_DOUBLE;
		}

		// to iste ako hore obratene
		else if(item1->data_type == DTYPE_INT || item3->data_type == DTYPE_DOUBLE)
		{
			item1_double = true;
			*final = DTYPE_DOUBLE;
		}

		//double + double = DOUBLE
		else if(item1->data_type == DTYPE_DOUBLE || item3->data_type == DTYPE_DOUBLE)
		{
			*final = DTYPE_DOUBLE;
		}
	}

	else if (rule == NT_DIV_NT){
	    *final = DTYPE_DOUBLE;

        if(item1->data_type == DTYPE_STRING || item3->data_type == DTYPE_STRING){
            return ERROR_ARTIHMETIC;
        } else if (item1->data_type == DTYPE_INT) {
            item1_double = true;
        } else if (item3->data_type == DTYPE_INT) {
            item3_double = true;
        }
	}


	//pravidla pre idiv
	else if (rule == NT_IDIV_NT)
	{
		*final = DTYPE_INT;

		if(item1->data_type == DTYPE_STRING)
		{
			return ERROR_ARTIHMETIC;
		}
		else if(item3->data_type == DTYPE_STRING)
		{
			return ERROR_ARTIHMETIC;
		}

		else if (item1->data_type == DTYPE_DOUBLE || item3->data_type == DTYPE_DOUBLE)
		{
            return ERROR_ARTIHMETIC;
		}
	}

	//pravidla pre compare
	else if(rule == NT_EQ_NT ||
	   		rule == NT_NEQ_NT||
	   		rule == NT_LTN_NT||
	   		rule == NT_MEQ_NT||
	   		rule == NT_MTN_NT||
	   		rule == NT_LEQ_NT)

	{	//int > int
		if(item1->data_type == DTYPE_INT && item3->data_type == DTYPE_INT)
		    *final = DTYPE_BOOL;

		//double > double
		else if (item1->data_type == DTYPE_DOUBLE && item3->data_type == DTYPE_DOUBLE)
		    *final = DTYPE_BOOL;

		//int > double
		else if(item1->data_type == DTYPE_INT && item3->data_type == DTYPE_DOUBLE) {
            *final = DTYPE_BOOL;
            item1_double = true;
        }

		//double > int
		else if(item1->data_type == DTYPE_DOUBLE && item3->data_type == DTYPE_INT) {
            *final = DTYPE_BOOL;
            item3_double = true;
        }

		//string > string
		else if(item1->data_type == DTYPE_STRING && item3->data_type == DTYPE_STRING)
		    *final = DTYPE_BOOL;

		else return ERROR_ARTIHMETIC;
	}

	if (item1_double){

	}
	if (item3_double){

	}

	return SYNTAX_OK; // SYNTAX_OK
}


static int RE_rule(ParserData *data)
{
	int result;

	Prec_rules actual_rule;
	DataType final_data_type;

	s_item* item1 = NULL;
	s_item* item2 = NULL;
	s_item* item3 = NULL;

	bool until_stop = false; // premenna na funkciu na najdenie STOP

	int counter = stop_count(&until_stop);

	if (until_stop == false)
	{
		return ERROR_PARSER; //syntax err
	}

	if(counter == 3 || counter == 1);
	else
	    return ERROR_PARSER;

	// pocet sa musi rovnat 1 alebo 3 inak syntax err ziadne pravidlo nema 2 členy

	if (counter == 3 && until_stop == true)
	{
		item3 = symStack->top;
		item2 = symStack->top->next;
		item1 = symStack->top->next->next;
        actual_rule = prec_rules_syntax(counter, item1, item2, item3);
	}

	else if (counter == 1 && until_stop == true)
	{
		item1 = symStack->top;
		actual_rule = prec_rules_syntax(counter, item1, NULL ,NULL );
	}
	else return ERROR_PARSER;

	//kontrola spravnosti semantiky
	if(actual_rule != NOT_A_RULE)
	{
		result = prec_rule_semantics(actual_rule, item1, item2, item3, &final_data_type);
		if (result != 0)
		{
			return result;
		}

		//konkatenacia
		if (actual_rule == NT_PLUS_NT && final_data_type == DTYPE_STRING)
		{
			//GENERATE CODE
		}
		else{;
            //GENEREATE CODE
		}
		data->leftID->type = final_data_type;
		symbol_pop_times(symStack, (++counter));
		symbol_push(symStack,NON_TERM,final_data_type);
	}

	else return ERROR_PARSER;

	return SYNTAX_OK; // syntax OK
}

int expression(ParserData *data)
{
    //alokacia pamate pre stack, pri nepodarenej alokacii vracia error
    symStack = (sstack *) malloc(MAX_STACK_SIZE * sizeof(sstack));
    if (!symStack) {
        return ERROR_INTERN;
    }
    //inicializacia
    symbol_init(symStack);
    //na zaciatok pushneme DOLAR na stack, aby sme vedeli kde zacina
    symbol_push(symStack, DOLLAR_SYM, DTYPE_UNDEFINED);

    s_item* sym_on_top;
    prec_table_sym sym_in_token;
    int result;
    bool end = false;

    // zacina cyklus, chceme aby zbehol aspon raz, konci pri nastaveni priznaku end na true
    do{
        //priradenie hodnot
        sym_on_top = symbol_top_term(symStack);     //vracia prvy terminal zo stacku
        sym_in_token = tok_to_sym(&data->Token);    //vracia symbol z tokenu pomocou fce

        switch(prec_tab[assign_prec_tab_ind(sym_on_top->symbol)][assign_prec_tab_ind(sym_in_token)])
             {
                case EQ:
                    //pri operacii equal pushneme novy symbol na stack
                    if (!symbol_push(symStack,sym_in_token, token_to_data(data))){
                        symbol_free(symStack);
                        return ERROR_INTERN;
                    }
                    if((result = getNextToken(&data->Token))){
                        symbol_free(symStack);
                        return result;
                    }
                    break;
                case SH:
                     //kedze nastava shift, vkladame hosnotu STOP za prvy terminal na stacku
                     if (!sym_insert_stop_NT(symStack,STOP,DTYPE_UNDEFINED)){
                         symbol_free(symStack);
                         return ERROR_INTERN;
                     }
                     //pushneme na stack dalsi symbol
                     if (!symbol_push(symStack,sym_in_token, token_to_data(data))){
                         symbol_free(symStack);
                         return ERROR_INTERN;
                     }
                     if((result = getNextToken(&data->Token))) {
                        symbol_free(symStack);
                        return result;
                     }
                     break;
                case RE:
                    if ((result = RE_rule(data))) {
                        symbol_free(symStack);
                        return result;
                    }
                    break;
                default:
                    if (sym_on_top->symbol == DOLLAR_SYM && sym_in_token == DOLLAR_SYM){
                        end = true;
                    } else {
                        symbol_free(symStack);
                        return ERROR_PARSER;
                    }
         }
    } while (!end);
    data->leftID = NULL;
    return result;
}