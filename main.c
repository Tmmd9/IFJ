//testovaci mainik na scanner, niektore funkcie museli byt modifikovane
//aby som mohol testovat

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int main(void) {
    // pre testovanie namiesto stdin vlozit fopen("cesta k testovaciemu suboru","r");
    FILE *source = stdin;

    sourceFile(source);
    parse();

    return EXIT_SUCCESS;
}