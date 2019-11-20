/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: generator.h
 *				-hlavičkový súbor k modulu generator.h
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdbool.h>
#include "str.h"


bool addInstr(const char *_instr,...);
bool addCode(const char *_code,...);

#endif //GENERATOR_H
