//testovaci mainik na scanner, niektore funkcie museli byt modifikovane
//aby som mohol testovat

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int main(void) {
    FILE *source = stdin;

    sourceFile(source);

    return EXIT_SUCCESS;
}