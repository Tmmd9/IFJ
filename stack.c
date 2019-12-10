/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: stack.c
 *				-tento modul bol vypracovany do predmetu IAL prvej domacej ulohy
 *				 implementuje pomocny zasobnik
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */

#include "stack.h"

int STACK_SIZE = MAX_STACK;

void stackError(int error_code) {
    static const char *SERR_STRINGS[
            MAX_SERR + 1] = {"Unknown error", "Stack error: INIT", "Stack error: PUSH", "Stack error: TOP"};
    if (error_code <= 0 || error_code > MAX_SERR)
        error_code = 0;
    printf("%s\n", SERR_STRINGS[error_code]);
    err_flag = 1;
}

void stackInit(tStack *s) {
    /* ak dostala funkcia param NULL -> error*/
    if (s == NULL) stackError(SERR_INIT);
    else s->top = -1; //nastavime vrchol zasobniku
}

int stackEmpty(const tStack *s) {
    //ak je stack prazdny vraciame 1, inak 0
    return (s->top != -1) ? 0 : 1;
}

void stackTop(const tStack *s, char *c) {
    // ak je zasobnik prazdny vraciame error, else cez param c vratime znak z vrcholu stacku
    if (stackEmpty(s) != 0) stackError(SERR_TOP);
    else *c = s->arr[s->top];
}


void stackPop(tStack *s) {
    if (stackEmpty(s) == 0) {
        s->arr[s->top] = '\0';    //ak nie je stack prazdny odstranime znak na vrchu
        s->top -= 1; // posun o odstraneny prvok na stacku vzad
    }
}

int stackFull(const tStack *s) {
    //kontrola ci je zasobnik plny, ak ano vraciame 1 inak 0
    return ((STACK_SIZE - 1) == s->top) ? 1 : 0;
}


void stackPush(tStack *s, char c) {
    if (stackFull(s) == 1) {
        stackError(SERR_PUSH);    // pokial je stack plny hadzeme err
    } else {
        s->top += 1;    // posun stacku dopredu (spravi miesto)
        s->arr[s->top] = c;    //vlozenie znaku na vrch stacku
    }
}