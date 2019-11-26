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
#include "parser.h"

bool addInstr(const char *_instr,...);
bool addCode(const char *_code,...);
bool generateHead();
bool generateBuiltIn();
bool generateCode();
bool writeCodeToStdout();
bool generateMain();
bool generateMainEnd();
bool pushVar(token *token);
bool pushValue(token *token);
bool op1ToDouble();
bool op3ToDouble();
bool generateConcatenation();
bool generateAdds();
bool generateSubs();
bool generateMuls();
bool generateDivs();
bool generateIDivs();
bool generateLess();
bool generateMore();
bool generateEquals();
bool generateNotEqual();
bool generateMoreEqual();
bool generateLessEqual();
bool generateSaveExprResult(char *id, char *frame);
bool declareVar(char *frame, char *ID);
bool genFunctionHead(char *ID);
bool genFunctionParam(char *ID, int index);
bool generateReturn(char *ID);

#define PRINT                                                                 \
        "LABEL $print \n"                                                     \
        "PUSHFRAME \n"                                                        \
        "WRITE LF@%1 \n"                                                      \
        "MOVE GF@$result nil@nil \n"                                          \
        "POPFRAME \n"                                                         \
        "RETURN \n"                                                           \

#define INPUTI                                                                \
        "LABEL $inputi \n"                                                    \
        "PUSHFRAME \n"                                                        \
        "DEFVAR LF@%return \n"                                                \
        "READ LF@%return int \n"                                              \
        "POPFRAME \n"                                                         \
        "RETURN \n"                                                           \

#define INPUTS                                                                \
        "LABEL $inputs \n"                                                    \
        "PUSHFRAME \n"                                                        \
        "DEFVAR LF@%return \n"                                                \
        "READ LF@%return string \n"                                           \
        "POPFRAME \n"                                                         \
        "RETURN \n"                                                           \

#define INPUTF                                                                \
        "LABEL $inputf \n"                                                    \
        "PUSHFRAME \n"                                                        \
        "DEFVAR LF@%return \n"                                                \
        "READ LF@%return float \n"                                            \
        "POPFRAME \n"                                                         \
        "RETURN \n"                                                           \

#define LEN                                                                   \
        "LABEL $len \n"                                                       \
        "PUSHFRAME \n"                                                        \
        "DEFVAR LF@%return \n"                                                \
        "DEFVAR LF@%type \n"                                                  \
        "TYPE LF@%type LF@%1 \n"                                              \
        "JUMPIFEQ $len_cont LF@%type int@0 \n"                                \
        "EXIT int@4 \n"                                                       \
        "LABEL $len_cont \n"                                                  \
        "STRLEN LF@%return LF@%1 \n"                                          \
        "POPFRAME \n"                                                         \
        "RETURN \n"                                                           \

#define SUBSTR                                                                \
        "LABEL $substr \n"                                                    \
        "PUSHFRAME \n"                                                        \
        "DEFVAR LF@%return \n"                                                \
        "MOVE LF@%return string@nil \n"                                      \
        "DEFVAR LF@%length \n"                                                \
        "CREATEFRAME \n"                                                      \
        "DEFVAR TF@%1 \n"                                                     \
        "MOVE TF@%1 LF@%1 \n"                                                 \
        "CALL $len \n"                                                        \
        "MOVE LF@%length TF@%return \n"                                       \
        "DEFVAR LF@%help \n"                                                  \
        "LT LF@%help LF@%2 int@0 \n"                                          \
        "JUMPIFEQ $substr$ret LF@%help bool@true \n"                          \
        "GT LF@%help LF@%2 LF@%length \n"                                     \
        "JUMPIFEQ $substr$ret LF@%help bool@true \n"                          \
        "LT LF@%help LF@%3 int@0 \n"                                          \
        "JUMPIFEQ $substr$ret LF@%help bool@true \n"                          \
        "LT LF@%help LF@%length int@0 \n"                                     \
        "JUMPIFEQ $substr$ret LF@%help bool@true \n"                          \
        "EQ LF@%help LF@%length int@0 \n"                                     \
        "JUMPIFEQ $substr$ret LF@%help bool@true \n"                          \
        "DEFVAR LF@%len_i \n"                                                 \
        "SUB LF@%len_i LF@%length LF@%2 \n"                                   \
        "ADD LF@%len_i LF@%len_i int@1 \n"                                    \
        "LT LF@%help LF@%3 LF@%len_i \n"                                      \
        "JUMPIFEQ $edit LF@%help bool@true \n"                                \
        "LT LF@%help LF@%3 int@0 \n"                                          \
        "JUMPIFEQ $edit LF@%help bool@true \n"                                \
        "JUMP $substr_work \n"                                                \
        "LABEL $edit \n"                                                      \
        "MOVE LF@%2 LF@%len_i \n"                                             \
        "LABEL $substr_work \n"                                               \
        "DEFVAR LF@idx \n"                                                    \
        "MOVE LF@idx LF@%2 \n"                                                \
        "SUB LF@idx LF@idx int@1 \n"                                          \
        "DEFVAR LF@char \n"                                                   \
        "DEFVAR LF@work_loop_cond \n"                                         \
        "LABEL $substr$work_loop \n"                                          \
        "GETCHAR LF@char LF@%1 LF@idx \n"                                     \
        "CONCAT LF@%return LF@%return LF@char \n"                             \
        "ADD LF@idx LF@idx int@1 \n"                                          \
        "SUB LF@%3 LF@%3 int@1 \n"                                            \
        "GT LF@work_loop_cond LF@%3 int@0 \n"                                 \
        "JUMPIFEQ $substr$work_loop LF@work_loop_cond bool@true \n"           \
        "LABEL $substr$ret \n"                                                \
        "POPFRAME \n"                                                         \
        "RETURN \n"                                                           \

#define ORD                                                                   \
        "LABEL $ord \n"                                                       \
        "PUSHFRAME \n"                                                        \
        "DEFVAR LF@%return \n"                                                \
        "MOVE LF@%return int@0 \n"                                            \
        "DEFVAR LF@%help \n"                                                  \
        "LT LF@$help LF@%2 int@1 \n"                                          \
        "JUMPIFEQ $ord$ret LF@help bool@true \n"                              \
        "DEFVAR LF@%length \n"                                                \
        "CREATEFRAME \n"                                                      \
        "DEFVAR TF@%1 \n"                                                     \
        "MOVE TF@%1 LF@%1 \n"                                                 \
        "CALL $len \n"                                                        \
        "MOVE LF@%length TF@%return \n"                                       \
        "LT LF@%help LF@%length LF@%1 \n"                                     \
        "JUMPIFEQ $ord$ret LF@%help bool@true \n"                             \
        "SUB LF@%2 LF@%2 int@1 \n"                                            \
        "STRI2INT LF@%return LF@%1 LF@%2 \n"                                   \
        "LABEL $ord$ret \n"                                                   \
        "POPFRAME \n"                                                         \
        "RETURN \n"                                                           \

#define CHR                                                                   \
        "LABEL $chr \n"                                                       \
        "PUSHFRAME \n"                                                        \
        "DEFVAR LF@%return \n"                                                \
        "MOVE LF@%return string@\n"                                           \
        "DEFVAR LF@help \n"                                                   \
	    "LT LF@help LF@%1 int@1 \n"                                           \
	    "JUMPIFEQ $chr$ret LF@help bool@true \n"                              \
	    "GT LF@help LF@%1 int@255 \n"										  \
	    "JUMPIFEQ $chr$ret LF@help bool@true \n"						      \
	    "INT2CHAR LF@%return LF@%1 \n"										  \
	    "LABEL $chr$ret \n"													  \
	    "POPFRAME \n"														  \
	    "RETURN \n"                                                           \


#endif //_GENERATOR_H
