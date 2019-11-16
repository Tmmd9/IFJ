
#ifdef _SYMTACK_H
#define _SYMTACK_H


#include <stdbool.h>

//#include "expression.h" //TODO
#include "symtable.h"

typedef struct item
{
	ptable_symbol symbol; //este nieje
	Data_type data_type;
	struct s_item *next;
} s_item;

typedef struct
{
	s_item *top;

} symbol_stack;

void symbol_init(sstack* stack);
/* 
	inicializacia zasobniku
*/

bool symbol_push(sstack* stack);
/*
	pushne symbol do stacku

*/
bool symbol_pop(sstack* stack);
/*
 	popne vrchny znak zo zasobniku
*/

void symbol_pop_times(sstack* stack, int times);
/*
	popne vrchnu znak zo zasobniku x krat
*/

symbol_item* symbol_top(sstack* stack);
/*
	vrati vrchny znak zo zasobniku
*/
void symbol_free(sstack* stack);
/*
	uvolnenie zdrojov
*/


#endif