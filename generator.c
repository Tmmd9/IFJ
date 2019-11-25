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

bool generateHead(){
    addInstr(".IFJcode19");

    addInstr("DEFVAR GF@$temp1");
    addInstr("DEFVAR GF@$temp2");
    addInstr("DEFVAR GF@$temp3");
    addInstr("DEFVAR GF@$result");
    addInstr("MOVE GF@$result nil@None");
    addInstr("DEFVAR GF@$return");

    addInstr("JUMP $$main");

    return true;
}



bool generateBuiltIn(){
    addInstr(PRINT);
    addInstr(INPUTI);
    addInstr(INPUTS);
    addInstr(INPUTF);
    addInstr(LEN);
    addInstr(SUBSTR);
    addInstr(ORD);
    addInstr(CHR);

    return true;
}

bool generateCode(){

    stringInit(&genCode);
    generateHead();
    generateBuiltIn();

    return true;
}

bool writeCodeToStdout(){
    fprintf(stdout,"%s",genCode.str);
    stringFree(&genCode);

    return true;
}

bool generateMain(){
    addInstr("LABEL $$main");
    addInstr("CREATEFRAME");
    addInstr("PUSHFRAME");

    return true;
}

bool generateMainEnd(){
    addInstr("POPFRAME");
    addInstr("CLEARS");

    return true;
}

bool generateValue(token *token){
    char tempStr[30];
    if (token->type == TYPE_INT){
        sprintf(tempStr,"%d",token->attribute.int_value);
        addCode("int@");
        addCode(tempStr);
        addCode("\n");
        return true;
    } else if (token->type == TYPE_FLOAT){
        sprintf(tempStr,"%a",token->attribute.decimal_value);
        addCode("float@");
        addCode(tempStr);
        addCode("\n");
        return true;
    } else if (token->type == TYPE_STRING){
        addCode("string@");
        addCode(token->attribute.string->str);
        addCode("\n");
        return true;
    }
    return false;
}

bool pushVar(token *token){
    addCode("PUSHS LF@%1");
    addCode(token->attribute.string->str);
   // addCode("\n");
    return true;
}

bool pushValue(token *token){
    addCode("PUSHS ");
    generateValue(token);
    //addCode("\n");
    return true;
}

bool op1ToDouble(){
    addInstr("INT2FLOATS");
    return true;
}

bool op3ToDouble(){
    addInstr("POPS GF@$temp1");
    addInstr("FLOAT2INTS");
    addInstr("PUSHS GF@$temp2");

    return true;
}

bool generateConcatenation(){
    addInstr("POPS GF@$temp2");
    addInstr("POPS GF@$temp1");
    addInstr("CONCAT GF@$result GF@$temp1 GF@$temp2");
    addInstr("PUSHS GF@$result");

    return true;
}



