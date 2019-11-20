#include <stdlib.h>

#include "symstack.h"


void symbol_init(sstack* stack)
{
	stack->top = NULL;
}

bool symbol_push(sstack* stack, ptable_symbol symbol, Data_type type)
{
	if(a_item != NULL)
	{
		a_item->symbol = symbol;
		a_item->data_type = type;
		a_item->next = stack->top;

		stack->top = a_item;

		return true;
	}
		return false;
}

bool symbol_pop(sstack* stack)
{
	if(stack->top != NULL)
	{
		symbol_item* top_tmp = stack->top;
		stack->top = top_tmp->next;
		free(tmp)
		return true; 
	}
		return false;	
}

void symbol_pop_times(sstack* stack, int times)
{
	int i = 0;
	while(i<times)
	{
		symbol_pop(stack);
		i++;
	}
}

symbol_item* symbol_top(sstack* stack)
{
	return stack->top
}


void symbol_free(sstack* stack)
{
	bool pop;
	pop = stack_pop(stack);
	while (pop)
	{
		pop = stack_pop(stack);
	}
}