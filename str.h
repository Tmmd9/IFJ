/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: str.h
 *				-hlavičkový súbor k modulu str.c
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */
#ifndef _STR_H
#define _STR_H

#define STR_SUCCESS 0
#define STR_ERR 1
#define INT_ALLOC_SIZE 8

// struktura reprezentujuca string
typedef struct {
    char *str;  //retazec ukonceny '\0'
    unsigned int length;    //dlzka retazca
    unsigned int allocSize; //velkost alokovanej pamate
} string;

int stringInit(string *s);

void stringStrFree(string *s);

void stringFree(string *s);

int stringAddChar(string *s, char c);

int stringAddConst(string *s, const char *str);

int stringCpy(string *source, string *dest);

int stringCmp(string *s, const char *str);


#endif // _STR_H
