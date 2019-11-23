/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: symstack.c
 *				- modul v ktorom je implementacia stacku symbolov
 *				- tento stack sluzi primarne na pracu v PSA
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */

#include <stdlib.h>
#include "symstack.h"


void symbol_init(sstack* symStack)
{
	symStack->top = NULL;
}

void symbol_pop_times(sstack* symStack, int times)
{
    int i = 0;
    while(i<times)
    {
        symbol_pop(symStack);
        i++;
    }
}

void symbol_free(sstack* symStack)
{
    bool pop;
    pop = symbol_pop(symStack);
    while (pop)
    {
        pop = symbol_pop(symStack);
    }
}

bool symbol_push(sstack* symStack, prec_table_sym symbol, DataType type)
{
    s_item *a_item =  (s_item *) malloc(sizeof(s_item));
	if(a_item != NULL)
	{
		a_item->symbol = symbol;
		a_item->data_type = type;
		a_item->next = symStack->top;

		symStack->top = a_item;

		return true;
	}
	return false;
}

bool symbol_pop(sstack* symStack)
{
	if(symStack->top != NULL)
	{
		s_item* top_tmp = symStack->top;
		symStack->top = top_tmp->next;
		free(top_tmp);
		return true; 
	}
	return false;
}

s_item* symbol_top_term(sstack * symStack)
{
    s_item* temp  = symStack->top;
    while(temp){
        if (temp->symbol < STOP) return temp;
        symStack->top = symStack->top->next;
    }
    return NULL;
}

s_item* symbol_top(sstack* symStack)
{
	return symStack->top;
}