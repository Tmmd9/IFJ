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
#include <stlib.h>

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
    stringStrFree(s);
}

//pridanie znaku do dynamickeho retazca
int stringAddChar(string *s, char c){
    //pokial dlzka retazca presahuje alokovane miesto, musime realokovat
    //s novou dlzkou
    if (s->length + 1 >= s->allocSize){
        unsigned int = newSize = s->length + INT_ALLOC_SIZE;
        if ((s->str = (char *) realloc(s->length, newSize)) == NULL){
            return STR_ERR;
        }
        s->allocSize = newSize;
    }
    s->str[s->length++] = c;    //pridavame znak na koniec retazca
    s->str[s->length] = '\0';   //ukoncujeme retazec
    return STR_SUCCESS;
}
