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
#include "stack.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

FILE *source;
tStack *stack;
string *parserStr;
static int firstToken = 1;  //kvoli indentom a dedentom
static bool initStack = false;


static int returnCode(int exitCode, string *s) {
    stringStrFree(s);
    return exitCode;
}

// funkcia na ziskanie tokenu typu integer
int intValue(string *s, token *token) {
    if (s->str[0] == '0' && s->str[1]) return returnCode(ERROR_SCANNER,s);
    char *endptr;
    int value = (int) strtol(s->str, &endptr, 10);
    if (*endptr) {
        return returnCode(ERROR_INTERN, s);
    }
    token->attribute.int_value = value;
    firstToken = 0;
    token->type = TYPE_INT;

    return returnCode(TOKEN_OK, s);
}

// funkcia na ziskanie tokenu typu float
int decimalValue(string *s, token *token) {
    char *endptr;
    double value = strtod(s->str, &endptr);
    if (*endptr) {
        return returnCode(ERROR_INTERN, s);
    }
    token->attribute.decimal_value = value;
    firstToken = 0;
    token->type = TYPE_FLOAT;

    return returnCode(TOKEN_OK, s);
}

int identifierOrKeyword(string *s, token *token) {
    if (stringCmp(s, "def")) token->attribute.keyword = KEYWORD_DEF;
    else if (stringCmp(s, "else")) token->attribute.keyword = KEYWORD_ELSE;
    else if (stringCmp(s, "if")) token->attribute.keyword = KEYWORD_IF;
    else if (stringCmp(s, "None")) token->attribute.keyword = KEYWORD_NONE;
    else if (stringCmp(s, "pass")) token->attribute.keyword = KEYWORD_PASS;
    else if (stringCmp(s, "return")) token->attribute.keyword = KEYWORD_RETURN;
    else if (stringCmp(s, "while")) token->attribute.keyword = KEYWORD_WHILE;
    else if (stringCmp(s, "inputs")) token->attribute.keyword = KEYWORD_INPUTS;
    else if (stringCmp(s, "inputi")) token->attribute.keyword = KEYWORD_INPUTI;
    else if (stringCmp(s, "inputf")) token->attribute.keyword = KEYWORD_INPUTF;
    else if (stringCmp(s, "print")) token->attribute.keyword = KEYWORD_PRINT;
    else if (stringCmp(s, "len")) token->attribute.keyword = KEYWORD_LEN;
    else if (stringCmp(s, "substr")) token->attribute.keyword = KEYWORD_SUBSTR;
    else if (stringCmp(s, "chr")) token->attribute.keyword = KEYWORD_CHR;
    else if (stringCmp(s, "ord")) token->attribute.keyword = KEYWORD_ORD;
    else { token->type = TYPE_IDENTIFIER; firstToken = 0;}

    if (token->type != TYPE_IDENTIFIER) {
        firstToken = 0;
        token->type = TYPE_KEYWORD;
        return returnCode(TOKEN_OK, s);
    }

    if (stringCpy(s, token->attribute.string)) {
        return returnCode(ERROR_INTERN, s);
    }

    return returnCode(TOKEN_OK, s);
}

void sourceFile(FILE *f) {
    source = f;
}

void setString(string *s) {
    parserStr = s;
}

int getNextToken(token *token) {
    if (!source) return ERROR_INTERN;
    if (!parserStr) return ERROR_INTERN;
    token->attribute.string = parserStr;

    string str;
    string *s = &str;
    if (stringInit(s)) {
        return returnCode(ERROR_INTERN, s);
    }
    if (!initStack){
        stack = (tStack *) malloc(STACK_SIZE * sizeof(tStack));
        if (!stack) {
            return ERROR_INTERN;
        }
        stackInit(stack);
        stackPush(stack, (char) 0);
        initStack = 1;
    }

    state state = STATE_START;    //pociatocny stav
    token->type = TYPE_EMPTY;    //pociatocny typ tokenu
    char c, *endptr, top;
    char arr[3] = {0, 0, 0};
    static int spaceCount = 0, spaceCountBeforeEOL = 0, quoteCount = 0;
    bool commentary = false, freeStack = false;

    while (1) {
        c = (char) getc(source);
        switch (state) {
            case (STATE_START):
                if (c == ' ') {        //ak je znak medzera
                    //vraciam znak do source a nastavime state
                    ungetc(c, source);
                    state = STATE_INDENT_OR_DEDENT;
                } else if (c == '\n') {        //novy riadok
                    state = STATE_EOL;
                } else if (c == '#') {        //riadkovy komentar
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    commentary = true;      //kvoli dedentom
                    state = STATE_COMMENT;
                } else if (isdigit(c)) {    //cislo
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_INT;
                } else if (c == '\'') {        //zaciatok stringu
                    state = STATE_STRING_START;
                } else if (c == '"') {        //zaciatok blokoveho komentara
                    state = STATE_BLOCK_STRING_START;
                } else if (isalpha(c) || c == '_') {//id or keyword
                    //zistime co je na tope stacku
                    stackTop(stack, &top);
                    //pokial je na tope cislo vacsie ako momentalny pocet medzier
                    //prechadzame do stavu dedent
                    if (firstToken && (top > spaceCount)) {
                        ungetc(c, source);
                        state = STATE_DEDENT;
                        break;
                    }
                    if (stringAddChar(s, c)) {
                        returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_ID_OR_KEYWORD;
                } else if (c == '!') {
                    state = STATE_NOT_EQUAL_START;
                } else if (c == '<') {
                    state = STATE_LESS_THAN;
                } else if (c == '>') {
                    state = STATE_GREATER_THAN;
                } else if (c == '=') {
                    state = STATE_EQUALS;
                } else if (c == '/') {
                    state = STATE_DIVIDE;
                } else if (c == '+') {
                    firstToken = 0;
                    token->type = TYPE_PLUS;
                    return returnCode(TOKEN_OK, s);
                } else if (c == '-') {
                    firstToken = 0;
                    token->type = TYPE_MINUS;
                    return returnCode(TOKEN_OK, s);
                } else if (c == '*') {
                    firstToken = 0;
                    token->type = TYPE_MULTIPLY;
                    return returnCode(TOKEN_OK, s);
                } else if (c == ':') {
                    firstToken = 0;
                    token->type = TYPE_COLON;
                    return returnCode(TOKEN_OK, s);
                } else if (c == ',') {
                    firstToken = 0;
                    token->type = TYPE_COMMA;
                    return returnCode(TOKEN_OK, s);
                } else if (c == '(') {
                    firstToken = 0;
                    token->type = TYPE_LEFT_PAR;
                    return returnCode(TOKEN_OK, s);
                } else if (c == ')') {
                    firstToken = 0;
                    token->type = TYPE_RIGHT_PAR;
                    return returnCode(TOKEN_OK, s);
                } else if (c == EOF) {
                    //ak pride EOF a stack nie je prazdny, musime vyprazdnit
                    //a generovat dedenty
                    if (!stackEmpty(stack)) {
                        stackTop(stack, &top);
                        if (top == 0) {
                            token->type = TYPE_EOF;
                            return returnCode(TOKEN_OK, s);
                        } else {
                            freeStack = true;
                            state = STATE_DEDENT;
                        }
                    } else {
                        token->type = TYPE_EOF;
                        return returnCode(TOKEN_OK, s);
                    }
                } else return returnCode(ERROR_SCANNER, s);
                break;

            case (STATE_INDENT_OR_DEDENT):
                if (firstToken && !isspace(c)) {
                    state = STATE_DEDENT;
                } else if (!firstToken) {
                    state = STATE_START;
                } else {
                    spaceCount++;
                    state = STATE_INDENT;
                }
                break;

            case (STATE_INDENT):
                if (isspace(c)) {
                    spaceCount++;
                } else if (c == '\n') {
                    spaceCount = 0;
                    state = STATE_START;
                } else if (isalnum(c)) {
                    stackTop(stack, &top);
                    if (top < spaceCount) {
                        stackPush(stack, (char) spaceCount);
                        firstToken = 0;
                        ungetc(c, source);
                        token->type = TYPE_INDENT;
                        return returnCode(TOKEN_OK, s);
                    } else if (top > spaceCount) {
                        ungetc(c, source);
                        state = STATE_DEDENT;
                    } else {
                        ungetc(c, source);
                        state = STATE_START;
                    }
                } else if (c == '"') {
                    ungetc(c, source);
                    state = STATE_START;
                } else {
                    return returnCode(ERROR_SCANNER, s);
                }
                break;

            case (STATE_DEDENT):
                stackTop(stack, &top);
                char helpStack;
                while (top != spaceCount) {
                    stackTop(stack, &top);
                    if (top == 0 && spaceCount != 0) {
                        return returnCode(ERROR_SCANNER, s);
                    }
                    if (top == spaceCount) break;
                    stackPop(stack);
                    stackTop(stack, &helpStack);
                    if (top > spaceCount && spaceCount > helpStack) {
                        return returnCode(ERROR_SCANNER, s);
                    }
                    if (freeStack) {
                        while (top != 0) {
                            if (helpStack == 0 && spaceCount == 0 &&
                                top == 0 && spaceCountBeforeEOL != top) {
                                return returnCode(ERROR_SCANNER, s);
                            }
                            token->type = TYPE_DEDENT;
                            return returnCode(TOKEN_OK, s);
                        }
                    }
                    break;
                }
                ungetc(c, source);
                token->type = TYPE_DEDENT;
                return returnCode(TOKEN_OK, s);

                break;

            case (STATE_INT):
                //ak dostaneme cislo, pridavame do vysledneho retazca
                if (isdigit(c)) {
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                } else if (c == '.') {
                    state = STATE_FLOAT_POINT;
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                } else if (tolower(c) == 'e') {
                    state = STATE_FLOAT_EXP;
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                } else {
                    //ak je dalsim znakom nieco ine ako cislo, E or .
                    //funkcia ungetc nam znak ktory mame momentalne
                    //v nasom 'c' vrati spat do streamu
                    //a vraciame vysledok funkcie intValue
                    ungetc(c, source);
                    return intValue(s, token);
                }
                break;

            case (STATE_FLOAT_POINT):
                if (isdigit(c)) {
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_FLOAT_POINT_NUMBER;
                } else {
                    return returnCode(ERROR_SCANNER, s);
                }
                break;

            case (STATE_FLOAT_EXP):
                if (isdigit(c)) {
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_FLOAT;
                } else if (c == '+' || c == '-') {
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_FLOAT_EXP_OP;
                } else {
                    ungetc(c, source);
                    return returnCode(ERROR_SCANNER, s);
                }
                break;

            case (STATE_FLOAT_POINT_NUMBER):
                if (isdigit(c)) {
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                } else if (tolower(c) == 'e') {
                    state = STATE_FLOAT_EXP;
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                } else {
                    ungetc(c, source);
                    return decimalValue(s, token);
                }
                break;

            case (STATE_FLOAT_EXP_OP):
                if (isdigit(c)) {
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_FLOAT;
                } else {
                    ungetc(c, source);
                    return returnCode(ERROR_SCANNER, s);
                }
                break;

            case (STATE_FLOAT):
                if (isdigit(c)) {
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                } else {
                    ungetc(c, source);
                    return decimalValue(s, token);
                }
                break;

            case (STATE_COMMENT):
                if (c == '\n' || c == EOF) {
                    state = STATE_START;
                    ungetc(c, source);
                    firstToken = 1;
                }
                break;

            case (STATE_STRING_START):
                //ascii < 32 -> error
                if (c < 32) {
                    return returnCode(ERROR_SCANNER, s);
                } else if (c == '\\') {    //escape sekvencia
                    state = STATE_BACKSLASH;
                } else if (c == '\'') {    //koniec stringu
                    if (stringCpy(s, token->attribute.string)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    firstToken = 0;
                    token->type = TYPE_STRING;

                    return returnCode(TOKEN_OK, s);
                } else {
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                }
                break;

            case (STATE_ID_OR_KEYWORD):
                if (isalnum(c) || c == '_') {
                    if (stringAddChar(s, (char) tolower(c))) {
                        return returnCode(ERROR_INTERN, s);
                    }
                } else {
                    ungetc(c, source);
                    return identifierOrKeyword(s, token);
                }
                break;

            case (STATE_BACKSLASH):
                if (c == '\\') {
                    c = '\\';
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_STRING_START;
                } else if (c == '"') {
                    c = '"';
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_STRING_START;
                } else if (c == 'n') {
                    c = '\n';
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_STRING_START;
                } else if (c == 't') {
                    c = '\t';
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_STRING_START;
                } else if (c == '\'') {
                    c = '\'';
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_STRING_START;
                } else if (c == 'x') {
                    state = STATE_BACKSLASH_X;
                } else if (c == EOF) {
                    return returnCode(ERROR_SCANNER, s);
                } else {
                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_STRING_START;
                }
                break;

            case (STATE_BACKSLASH_X):
                if (isxdigit(c)) {
                    arr[0] = c;
                    state = STATE_BACKSLASH_X_NUM;
                } else {
                    return returnCode(ERROR_SCANNER, s);
                }
                break;

            case (STATE_BACKSLASH_X_NUM):
                if (isxdigit(c)) {
                    arr[1] = c;

                    int value = (int) strtol(arr, &endptr, 16);
                    if (*endptr) {
                        return returnCode(ERROR_INTERN, s);
                    }

                    c = (char) value;

                    if (stringAddChar(s, c)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    state = STATE_STRING_START;
                } else {
                    return returnCode(ERROR_SCANNER, s);
                }
                break;

            case (STATE_BLOCK_STRING_START):
                if (c == '"') {
                    state = STATE_BLOCK_STRING_CONTINUE;
                } else if (c == EOF) {
                    return returnCode(ERROR_SCANNER, s);
                } else return returnCode(ERROR_SCANNER, s);
                break;

            case (STATE_BLOCK_STRING_CONTINUE):
                if (c == '"') {
                    state = STATE_BLOCK_STRING;
                } else if (c == EOF) {
                    return returnCode(ERROR_SCANNER, s);
                } else return returnCode(ERROR_SCANNER, s);
                break;

            case (STATE_BLOCK_STRING):  //sme v dokumentacnom retazci
                // pokial je dalsi znak uvodzovka, prechadzame dalej a pocitame "
                if (c == '"') {
                    if (firstToken) commentary = true;
                    quoteCount += 1;
                    state = STATE_BLOCK_STRING_LEAVE_TRY;
                } else if (c == EOF) {
                    return returnCode(ERROR_SCANNER, s);
                } else if (firstToken) {
                    commentary = true;
                    if (c == '\\'){
                        state = STATE_BLOCK_STRING_ESC;
                    }
                    break;
                } else if (c == '\\'){
                    state = STATE_BLOCK_STRING_ESC;
                } else {
                    //ak je znak hocijaky iny nez EOF alebo ", pridavame do retazca
                    if (stringAddChar(s,c)) {
                        return returnCode(ERROR_SCANNER,s);
                    }
                }
                break;

            case (STATE_BLOCK_STRING_ESC):
                if (c == '"'){
                    if (!commentary){
                        if (stringAddChar(s,c)) {
                            return returnCode(ERROR_SCANNER,s);
                        }
                    }
                    state = STATE_BLOCK_STRING;
                } else {
                    if (!commentary){
                        if (stringAddChar(s,'\\')) {
                            return returnCode(ERROR_SCANNER,s);
                        }
                        if (stringAddChar(s,c)) {
                            return returnCode(ERROR_SCANNER,s);
                        }
                    }
                    state = STATE_BLOCK_STRING;
                }
                break;

            case (STATE_BLOCK_STRING_LEAVE_TRY):
                // pokial je dalsi znak uvodzovka, prechadzame dalej a pocitame "
                if (c == '"' && quoteCount == 1) {
                    quoteCount += 1;
                    state = STATE_BLOCK_STRING_LEAVE;
                } else if (c == EOF) {
                    return returnCode(ERROR_SCANNER, s);
                } else if (firstToken) {
                    quoteCount = 0;
                    state = STATE_BLOCK_STRING;
                } else {
                    /* pokial to je iny znak, nulujeme count a do retazca
                     * pridavame explicitne " a aktualne spracovany znak*/
                    quoteCount = 0;
                    if (stringAddChar(s,'"')) {
                        return returnCode(ERROR_SCANNER,s);
                    }
                    if (stringAddChar(s,c)) {
                        return returnCode(ERROR_SCANNER,s);
                    }
                    state = STATE_BLOCK_STRING;
                }
                break;

            case (STATE_BLOCK_STRING_LEAVE):
                /* pokial pridu tretie " v rade, ukladame vysledny string
                 * a nulujeme counter */
                if (c == '"' && quoteCount == 2) {
                    quoteCount = 0;
                    if (commentary){
                        firstToken = 0;
                        token->type = TYPE_COMMENT;
                        return returnCode(TOKEN_OK,s);
                    }
                    if (stringCpy(s, token->attribute.string)) {
                        return returnCode(ERROR_INTERN, s);
                    }
                    token->type = TYPE_STRING;
                    return returnCode(TOKEN_OK,s);
                } else if (c == EOF) {
                    return returnCode(ERROR_SCANNER, s);
                } else {
                    /* ak pride iny znak, opat nulujeme count a explicitne
                     * pridavame 2x " a aktualny znak*/
                    quoteCount = 0;
                    if (stringAddChar(s,'"')) {
                        return returnCode(ERROR_SCANNER,s);
                    }
                    if (stringAddChar(s,'"')) {
                        return returnCode(ERROR_SCANNER,s);
                    }
                    if (stringAddChar(s,c)) {
                        return returnCode(ERROR_SCANNER,s);
                    }
                    state = STATE_BLOCK_STRING;
                }
                break;

            case (STATE_EQUALS):
                if (c == '=') {
                    firstToken = 0;
                    token->type = TYPE_EQUALS;
                } else {
                    firstToken = 0;
                    ungetc(c, source);
                    token->type = TYPE_ASSIGN_VALUE;
                }
                return returnCode(TOKEN_OK, s);

            case (STATE_NOT_EQUAL_START):
                if (c == '=') {
                    firstToken = 0;
                    token->type = TYPE_NOT_EQUAL;
                } else {
                    return returnCode(ERROR_SCANNER, s);
                }
                return returnCode(TOKEN_OK, s);

            case (STATE_LESS_THAN):
                if (c == '=') {
                    firstToken = 0;
                    token->type = TYPE_LESS_EQUAL;
                } else {
                    firstToken = 0;
                    ungetc(c, source);
                    token->type = TYPE_LESS_THAN;
                }
                return returnCode(TOKEN_OK, s);

            case (STATE_GREATER_THAN):
                if (c == '=') {
                    firstToken = 0;
                    token->type = TYPE_GREATER_EQUAL;
                } else {
                    firstToken = 0;
                    ungetc(c, source);
                    token->type = TYPE_GREATER_THAN;
                }
                return returnCode(TOKEN_OK, s);

            case (STATE_DIVIDE):
                if (c == '/') {
                    firstToken = 0;
                    token->type = TYPE_DIVIDE_INT;
                } else {
                    firstToken = 0;
                    ungetc(c, source);
                    token->type = TYPE_DIVIDE;
                }
                return returnCode(TOKEN_OK, s);

            case (STATE_EOL):
                ungetc(c, source);
                firstToken = 1;
                if (!commentary){
                    spaceCountBeforeEOL = spaceCount;
                }
                spaceCount = 0;
                token->type = TYPE_EOL;
                return returnCode(TOKEN_OK, s);

            default: ;
        }
    }
}