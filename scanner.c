/*
 *	Predmet : Formální jazyky a překladače									 
 *	Projekt : Implementace překladače imperativního jazyka IFJ19			 
 *	Súbor	: scanner.c													 
 *				-modul na implementáciu scanneru (lexikálny analyzátor)
 *	Tím		: číslo 10 / varianta II										 
 *	Autori	: Dominik Boboš (xbobos00)										 
 *			  Peter Hudeček (xhudec34)										 
 *			  Timotej Kováčik (xkovac49)									 
 *			  Dávid Oravec (xorave05)										 
 *																			 
 */


#include "scanner.h"
#include "error.h"

#include <stdio.h>
#include <ctype.h>					// na isdigit a take srance

FILE *source;
string *string;

void sourceFile(FILE *f){
    source = f;
}

int returnCode(int exitCode, string *s){
    stringStrFree(s);
    return exitCode;
}

int intValue(string *s, token *token){
    char *endptr;
    int value = strtol(s->str,&endptr, 10);
    if (*endptr){
        return returnCode(ERROR_INTERN, s);
    }
    token->attribute.int_value = value;
    token->type = TYPE_INT;

    returnCode(TOKEN_OK,s);
}

int decimalValue(string *s, token *token){
    char *endptr;
    int value = strtod(s->str,&endptr);
    if (*endptr){
        return returnCode(ERROR_INTERN, s);
    }
    token->attribute.decimal_value = value;
    token->type = TYPE_FLOAT;

    returnCode(TOKEN_OK,s);
}

int getNextToken(token *token){
    if (!source) return ERROR_INTERN;
    int state = STATE_START;
    token->type = TYPE_EMPTY;
    char c;

    while(1){
        c = getchar();
        switch (state) {
            case (STATE_START):
                if (isspace(c)) {
                    state = STATE_START;
                } else if (c == '\n') {
                    state = STATE_EOL;
                } else if (c == '#') {
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_COMMENT;
                } else if (isdigit(c)) {
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_INT;
                } else if (c == '\'') {
                    state = STATE_STRING_START;
                } else if (c == '"') {
                    state = STATE_BLOCK_COMMENT_START;
                } else if (isalpha(c) || c == '_') {
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_ID_OR_KEYWORD;
                } else if (c == '!') {
                    state = STATE_NOT_EQUAL_START;
                } else if (c == '<') {
                    state = STATE_LESS_THAN;
                } else if (c == '>'){
                    state = STATE_GREATER_THAN;
                } else if (c == '='){
                    state = STATE_EQUALS;
                } else if (c == '/') {
                    state = STATE_DIVIDE;
                } else if (c == '+'){
                    token->type = TYPE_PLUS;
                    returnCode(TOKEN_OK, s);
                } else if (c == '-'){
                    token->type = TYPE_MINUS;
                    returnCode(TOKEN_OK, s);
                } else if (c == '*'){
                    token->type = TYPE_MULTIPLY;
                    returnCode(TOKEN_OK, s);
                } else if (c == ';'){
                    token->type = TYPE_SEMICOLON;
                    returnCode(TOKEN_OK, s);
                } else if (c == ','){
                    token->type = TYPE_COMMA;
                    returnCode(TOKEN_OK, s);
                } else if (c == '('){
                    token->type = TYPE_LEFT_PAR;
                    returnCode(TOKEN_OK, s);
                } else if (c == ')'){
                    token->type = TYPE_RIGHT_PAR;
                    returnCode(TOKEN_OK, s);
                } else if (c == EOF){
                    token->type = TYPE_EOF;
                    returnCode(TOKEN_OK,s);
                } else returnCode(ERROR_SCANNER, s);
                break;
            case (STATE_INT):
                if (isdigit(c)){
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                } else if (c == '.'){
                    state = STATE_FLOAT_POINT;
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                } else if (toupper(c) == 'e'){
                    state = STATE_FLOAT_EXP;
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                } else {
                    ungetc(c, source);
                    return intValue(s,token);
                }
                break;
            case (STATE_FLOAT_POINT):
                if (isdigit(c)){
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_FLOAT_POINT_NUMBER;
                } else {
                    returnCode(ERROR_SCANNER,s);
                }
                break;
            case (STATE_FLOAT_EXP):
                if (isdigit(c)){
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_FLOAT;
                } else if (c == '+' || c == '-'){
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_FLOAT_EXP_OP;
                } else {
                    ungetc(c,source);
                    return returnCode(ERROR_SCANNER,s);
                }
                break;
            case (STATE_FLOAT_POINT_NUMBER):
                if (isdigit(c)){
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                } else if (toupper(c) == 'e') {
                    state = STATE_FLOAT_EXP;
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                } else {
                    ungetc(c,source);
                    return decimalValue(s,token);
                }
                break;
            case (STATE_FLOAT_EXP_OP):
                if (isdigit(c)) {
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_FLOAT;
                } else {
                    ungetc(c,source);
                    return returnCode(ERROR_SCANNER,s);
                }
                break;
            case (STATE_FLOAT):
                if (isdigit(c)) {
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                } else {
                    ungetc(c,source);
                    return decimalValue(s,token);
                }
                break;
            case (STATE_COMMENT):
                if (c == '\n' || c == EOF){
                    state = STATE_START;
                    ungetc(c,source);
                }
                break;
            case (STATE_STRING_START):
                if (c < 32){
                    return returnCode(ERROR_SCANNER,s);
                } else if (c == '\\'){
                    state = STATE_BACKSLASH;
                } else if (c == '\''){
                    //TODO
                } else {
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                }
                break;
            case (STATE_BACKSLASH):
                if (c == '\\'){
                    c = '\\';
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_STRING_START;
                } else if (c == '"') {
                    c = '"';
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_STRING_START;
                } else if (c == 'n'){
                    c = '\n';
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_STRING_START;
                } else if (c == 't'){
                    c = '\t';
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_STRING_START;
                } else if (c == '\''){
                    c = '\'';
                    if (!(stringAddChar(s, c))) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_STRING_START;
                }
                //TODO
        }
    }
}