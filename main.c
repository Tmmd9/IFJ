//testovaci mainik na scanner, niektore funkcie museli byt modifikovane
//aby som mohol testovat

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "stack.h"
#include "parser.h"
#include "symtable.h"

int main(void) {
    FILE *f = fopen("test.txt", "r");
    tStack *stack = (tStack *) malloc(STACK_SIZE * sizeof(tStack));
    if (!stack) {
        return ERROR_INTERN;
    }
    stackInit(stack);
    stackPush(stack, (char) 0);
   
        token token;
        string string;
        ParserData data;
        if (stringInit(&string)) {
            return ERROR_INTERN;
        }

        int SyntaxCheck = prog(data); //premena kde nahram return parsera a podla toho printujem err alebo syntax ok
        if(SyntaxCheck == SYNTAX_OK){
        	printf("Syntax is OK. \n");
        	return SyntaxCheck;
        }s
        else if(SyntaxCheck == ERROR_PARSER){
        	printf("Syntax error. \n");
        	return SyntaxCheck;
        }
        else if(SyntaxCheck == ERROR_PROGRAM_SEMANTIC){
        	printf("Semantical error. \n");
        	return SyntaxCheck;
        }
        else if(SyntaxCheck == ERROR_ARITHMETIC){
        	printf("Arithmetical error. \n");
        	return SyntaxCheck;
        }
        else if(SyntaxCheck == ERROR_WRONG_NUMBER_OF_PARAMS){
        	printf("Wrong number of parameters. \n");
        	return SyntaxCheck;
        }
        else if(SyntaxCheck == ERROR_SEMANTIC_OTHERS) {
        	printf("Semantical error. \n");
      		return SyntaxCheck;
    	}
        else if(SyntaxCheck == ERROR_DIVIDING_ZERO)
        	printf("Dividing with the value of zero. \n")
    	}
        else if(SyntaxCheck == ERROR_INTERN) {
        	printf("Internal error. \n");
        	return SyntaxCheck;
        }
 /*       token.attribute.string = &string;
        int code = getNextToken(f, &token, stack);

        if (code) {
            fprintf(stderr, "Scanner error!\n");
            stringStrFree(&string);

            return code;
        }

        printf("Token type is %d.\n", token.type);

        if (token.type == TYPE_KEYWORD) {
            printf("Token keyword is %d.\n", token.attribute.keyword);
        } else if (token.type == TYPE_IDENTIFIER) {
            printf("Token identifier is %s.\n", token.attribute.string->str);
        } else if (token.type == TYPE_INT) {
            printf("Token int is %d.\n", token.attribute.int_value);
        } else if (token.type == TYPE_FLOAT) {
            printf("Token double is %f.\n", token.attribute.decimal_value);
        } else if (token.type == TYPE_STRING) {
            printf("Token string is %s.\n", token.attribute.string->str);
        } else if (token.type == TYPE_PLUS) {
            //printf("Token type is %d.\n", token.type);
        } else if (token.type == TYPE_MINUS) {
            //printf("Token type is %d.\n", token.type);
        } else if (token.type == TYPE_MULTIPLY) {
            //printf("Token type is %d.\n", token.type);
        } else if (token.type == TYPE_DIVIDE) {
            //printf("Token type is %d.\n", token.type);

        } else if (token.type == TYPE_DIVIDE_INT) {
            printf("Token type is %d.\n", token.type);

        } else if (token.type == TYPE_EOL) {
            printf("Token type is EOLIK.\n");
        } else if (token.type == TYPE_INDENT) {
            printf("Token type is INDENTIK.\n");
        } else if (token.type == TYPE_DEDENT) {
            printf("Token type is DEDENTIK.\n");
        }

        if (token.type == TYPE_EOF) {
            stringStrFree(&string);
            break;
        }
        */
        stringStrFree(&string);
    


    return EXIT_SUCCESS;
}