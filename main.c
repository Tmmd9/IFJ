//testovaci mainik na scanner, niektore funkcie museli byt modifikovane
//aby som mohol testovat

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "stack.h"

int main(void) {
    FILE *f = fopen("test.txt", "r");
    tStack *stack = (tStack *) malloc(STACK_SIZE * sizeof(tStack));
    if (!stack) {
        return ERROR_INTERN;
    }
    stackInit(stack);
    stackPush(stack, (char) 0);
    while (true) {
        token token;
        string string;
        if (stringInit(&string)) {
            return ERROR_INTERN;
        }
        token.attribute.string = &string;
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
        stringStrFree(&string);
    }


    return EXIT_SUCCESS;
}