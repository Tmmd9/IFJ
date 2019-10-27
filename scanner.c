/*
 *	Predmet : Formální jazyky a překladače									 
 *	Projekt : Implementace překladače imperativního jazyka IFJ19			 
 *	Súbor	: scanner.c													 
 *				-.c implementácia pre náš scanner (lexikálny analyzátor)	 
 *	Tím		: číslo 10 / varianta II										 
 *	Autori	: Dominik Boboš (xbobos00)										 
 *			  Peter Hudeček (xhudec34)										 
 *			  Timotej Kováčik (xkovac49)									 
 *			  Dávid Oravec (xorave05)										 
 *																			 
 */


#include "scanner.h"				// all states

#include <stdio.h>
#include <ctype.h>					// na isdigit a take srance

#include <stdlib.h>					// na pracu so stringom


/**
//hladam kde kurnik mam otvarat file
//ok otvara sa kdesi v maine a nejako by sa mal 
//prebublat az sem, pravdepodobne pojde parserom az sem

//fixed, otvara sa v maine a 
**/

/**
mali by sme spravit podobne dynamic string ako ma harmim a k tomu este aj 
pridat zasobnik, kvoli indentom a dedentom cize tam dat veci z ial
cize treba vytvorit dalsie .c a .h subory, zatial to idem napacat len sem
**/
#define INICIALIZATION_LENGHT 8 /// Inicialization lenght of string.


/** TESTY TODO **/

FILE *source; 						// Source file na skenovanie

void set_source(FILE *f)		//nejako tu budem pasovat ten subor
{
	source = f;
}

/*
//setovat bude treba asi az v inych .c suboroch, 
tuto to totizto mam vsetko deklarovane
takze tuto funkciu potom asi bude treba inde

void DynamicString_set(Dynamic_string *string)
{
	dyn_string = string;
}
*/

int DynamicString_init(dyn_string *s)
{
	if (s->string = (char *) malloc(INICIALIZATION_LENGHT) == NULL)
	{
		return 0;
	}
	s->length = 0;
	s->string[s->length] = '\0';
	s->alloc_size = INICIALIZATION_LENGHT;
	return 1;
}


// realloc , pridava jeden znak do stringu
int DynamicString_add_char(dyn_string *s, char ch)
{	/* 
		pokial je dlzka 7 znamena to 7znakov + '\0' 
		t.j. 8alokovanych Bytov
		preto chcem reallokovat pokial je aj dlzka slova 7
		preto je takato podmienka
	*/
	if (s->length + 1 >= s->alloc_size)
	{
		unsigned new_length = s->length + INICIALIZATION_LENGHT;
		if(s->string = (char*) realloc(s->string, new_length) == NULL)
		{						//realloc(pointer, size)
			return 0;
		}
		s->alloc_size = new_length;
	}
	s->string[s->length] = ch;
	s->length++;
	s->string[s->length] = '\0';

	return 1;
}

// realloc , concatuje new string na koniec stareho stringu
int DynamicString_add_string(dyn_string *s, char *str)
{	
	unsigned new_str_length = strlen(str);
	/* 
		pokial je dlzka 7 znamena to 7znakov + '\0' 
		t.j. 8alokovanych Bytov
		preto chcem reallokovat pokial je aj dlzka slova 7
		preto je takato podmienka
	*/
	if (s->length + new_str_length + 1 >= s->alloc_size)
	{
		//	+1 kvoli '\0'
		unsigned new_length = s->length + new_str_length + 1;
		if(s->string = (char*) realloc(s->string, new_length) == NULL)
		{						//realloc(pointer, size)
			return 0;
		}
		s->alloc_size = new_length;
	}
	s->length = s->length + new_str_length;
	strcat(s->string, str);
	s->string[s->length] = '\0';

	return 1;
}


/*
neviem kedy to budeme potrebovat ale v takom pripade to staci len odkomentovat :D 

//kopiruje string zo source do dest
int DynamicString_copy(DynamicString *source, DynamicString *dest)
{

	unsigned new_str_length = source->length;
	//pokial je velkost source vacsia ako dest velkost musim realokovat
	if (new_str_length >= dest->alloc_size)
	{
		//realokujem dest->str pointer na velkost source = new_str_length
		// +1 kvoli '\0'
		if (dest->string = (char *) realloc(dest->string, new_str_length + 1) == NULL)
		{
			return false;
		}
		//inkrementujem +1 kvoli '\0'
		dest->alloc_size = new_str_length + 1;
	}

	strcpy(dest->string, source->string);
	dest->length = new_length;

	return true;
}
*/

void DynamicString_free(dyn_string *s)
{
	free(s->string);
}

void DynamicString_clear(dyn_string *s)
{
	s->length = 0;
	s->string[s->length] = '\0';
}

