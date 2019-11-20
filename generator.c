/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: generator.c
 *				-modul v ktorom sa generuju casti IFJcode19
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */

#include "generator.h"
#include "scanner.h"

string genCode;
bool addInstr(const char *_instr,...) {
    if (stringAddConst(&genCode, (_instr))){
        return false;
    }
    if (stringAddConst(&genCode, " \n")){
        return false;
    }
    return true;
}

bool addCode(const char *_code,...){
    return (stringAddConst(&genCode, (_code))) ? false : true;
}

//bool generateStart_of_Function(char *)




