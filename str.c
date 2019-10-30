/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: str.c
 *				-modul pre prácu so stringami
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */

#include "str.h"
#include <string.h>
#include <stdlib.h>

//inicializacia stringu, berie paramater structu
int stringInit(string *s){
    //alokacia na default hodnotu 8, pri mallocu sizeof char by nemalo vyznam
    if ((s->str = (char*) malloc(INT_ALLOC_SIZE)) == NULL){
        return STR_ERR; //ak sa nepodari vratime scanneru 1
    }
    //ak sa podarila, nastavime pociatocne hodnoty a vratime 0
    s->allocSize = INT_ALLOC_SIZE;
    s->length = 0;
    s->str[0] = '\0';
    return STR_SUCCESS;
}

//uvolnenie retazca str zo structu string
void stringStrFree(string *s){
    free(s->str);
}

//struct string uvedeny do povodneho stavu
void stringFree(string *s){
    s->length = 0;
	s->allocSize = 0;   //toto tam nechybalo nahodou?
    stringStrFree(s);
}

//pridanie znaku do dynamickeho retazca
int stringAddChar(string *s, char c){
    //pokial dlzka retazca presahuje alokovane miesto, musime realokovat
    //s novou dlzkou
    if (s->length + 1 >= s->allocSize){
        unsigned int newSize = s->length + INT_ALLOC_SIZE;
        if ((s->str = (char *) realloc(s->str, newSize)) == NULL){
            return STR_ERR;
        }
        s->allocSize = newSize;
    }
    s->str[s->length++] = c;    //pridavame znak na koniec retazca
    s->str[s->length] = '\0';   //ukoncujeme retazec
    return STR_SUCCESS;
}

int stringCat(string *s, const char *str){
    unsigned int newStrLength = strlen(str);
    /*
        pokial je dlzka 7 znamena to 7znakov + '\0'
        t.j. 8alokovanych Bytov
        preto chcem reallokovat pokial je aj dlzka slova 7
        preto je takato podmienka
    */
    if (s->length + newStrLength + 1 >= s->allocSize)
    {
        //	+1 kvoli '\0'
        unsigned int newLength = s->length + newStrLength + 1;
        if((s->str = (char*) realloc(s->str, newLength)) == NULL)
        {						//realloc(pointer, size)
            return STR_ERR;
        }
        s->allocSize = newLength;
    }
    s->length = s->length + newStrLength;
    strcat(s->str, str);
    s->str[s->length] = '\0';

    return STR_SUCCESS;
}

//kopiruje string zo source do dest
int stringCpy(string *source, string *dest){
	unsigned newStrLength = source->length;
	//pokial je velkost source vacsia ako dest velkost musim realokovat
	if (newStrLength >= dest->allocSize)
	{
		//realokujem dest->str pointer na velkost source = new_str_length
		// +1 kvoli '\0'
		if ((dest->str = (char *) realloc(dest->str, newStrLength + 1)) == NULL)
		{
			return STR_ERR;
		}
		//inkrementujem +1 kvoli '\0'
		dest->allocSize = newStrLength + 1;
	}
	strcpy(dest->str, source->str);
	dest->length = newStrLength;
	return STR_SUCCESS;
}

// porovnava retazec ulozeny v structe s konstantnym retazcom
int stringCmp(string* s, const char* str) {
	return (!(strcmp(s->str, str))) ? 1 : 0;
}
