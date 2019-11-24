//testovaci mainik na scanner, niektore funkcie museli byt modifikovane
//aby som mohol testovat

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "generator.h"
#include "parser.h"

int main(void) {
    // pre testovanie namiesto stdin vlozit fopen("cesta k testovaciemu suboru","r");
    FILE *source = fopen("/Users/tereziabobosova/Documents/VUT/3.semester/IFJ/test.txt", "r");
    //FILE *source = fopen("C:\\Users\\orave\\CLionProjects\\untitled4\\project\\cmake-build-debug\\CMakeFiles\\cmake-build-debug.dir\\test.txt", "r");
    //FILE *source = stdin;
    int result = 0;


    sourceFile(source);
    result = parse();
    writeCodeToStdout();


    return result;
}