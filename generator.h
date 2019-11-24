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

#ifndef _GENERATOR_H
#define _GENERATOR_H

#include <stdbool.h>
#include "str.h"


bool addInstr(const char *_instr,...);
bool addCode(const char *_code,...);

#define PRINT                           \
        "LABEL $print \n"               \
        "PUSHFRAME \n"                  \
        "WRITE LF@%1 \n"                \
        "MOVE GF@$result nil@nil \n"    \
        "POPFRAME \n"                   \
        "RETURN \n"                     \

#define INPUTI                          \
        "LABEL $inputi \n"              \
        "PUSHFRAME \n"                  \
        "DEFVAR LF@%return \n"          \
        "READ LF@%return int \n"        \
        "POPFRAME \n"                   \
        "RETURN \n"                     \

#define INPUTS                          \
        "LABEL $inputs \n"              \
        "PUSHFRAME \n"                  \
        "DEFVAR LF@%return \n"          \
        "READ LF@%return string \n"     \
        "POPFRAME \n"                   \
        "RETURN \n"                     \

#define INPUTF                          \
        "LABEL $inputf \n"              \
        "PUSHFRAME \n"                  \
        "DEFVAR LF@%return \n"          \
        "READ LF@%return float \n"      \
        "POPFRAME \n"                   \
        "RETURN \n"                     \

#define LEN                             \
        "LABEL \n"                      \
        "PUSHFRAME \n"                  \
        "DEFVAR LF@%return \n"          \
        "DEFVAR LF@%type \n"            \
        "TYPE LF@%type LF@%1 \n"        \
        "JUMPIFEQ $len_cont \n"         \
        "EXIT int@4 \n"                 \
        "LABEL $len_cont \n"            \
        "STRLEN LF@%return LF@%1 \n"    \
        "POPFRAME \n"                   \
        "RETURN \n"                     \


#endif //_GENERATOR_H
