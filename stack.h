/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: stack.h
 *				-tento hlavickovy subor bol prevzaty z nasich projektov do
 *				 IAL domacej ulohy jedna
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */

#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>

#define MAX_STACK 20
extern int STACK_SIZE;
/*
 * Hodnota MAX_STACK udává skutečnou velikost statického pole pro uložení
 * hodnot zásobníku. Při implementaci operací nad ADT zásobník však
 * předpokládejte, že velikost tohoto pole je pouze STACK_SIZE. Umožní to
 * jednoduše měnit velikost zásobníku v průběhu testování. Při implementaci
 * tedy hodnotu MAX_STACK vůbec nepoužívejte.
 */
int err_flag;

/* Chybové kódy pro funkci stackError */
#define MAX_SERR    3                                   /* počet možných chyb */
#define SERR_INIT   1                                  /* chyba při stackInit */
#define SERR_PUSH   2                                  /* chyba při stackPush */
#define SERR_TOP    3                                   /* chyba při stackTop */

/* ADT zásobník implementovaný ve statickém poli */
typedef struct {
    char arr[MAX_STACK];                             /* pole pro uložení hodnot */
    int top;                                /* index prvku na vrcholu zásobníku */
} tStack;

/* Hlavičky funkcí pro práci se zásobníkem. */
void stackError(int error_code);

void stackInit(tStack *s);

int stackEmpty(const tStack *s);

int stackFull(const tStack *s);

void stackTop(const tStack *s, char *c);

void stackPop(tStack *s);

void stackPush(tStack *s, char c);

#endif // !_STACK_H_

