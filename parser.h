#include "scanner.h" //ziskavanie tokenov
#include "stack.h" //indent/dedent + neviem co vsetko
#include "error.h" 
#include "expr.h" //precedencna tabulka pre semantiku
#include "symtable.h"	//tabulka symbolov
#include <stdbool.h>

//struktura co bude uchovavat vsetko dolezite pre parser
typedef struct {
    symTable globalT;               //na tabulku globalnych symbolov (identifikatorov etc)
    symTable localT;                //na tabulku localnych symbolov (identifikatorov etc)
	/*Neni dokoncene, ked ta napadne co
	sem pridat tak to sem pridaj a daj koment ze preco */
	token Token;
    tStack *stack;                  //dunno if necessary ale tak kvoli INDENTOM pre scanner

	Data * currentID;
	Data * leftID;
	Data * rightID;

    // ked checkujem parametre, tak index toho ktory momentalne spracovavam
	unsigned paramIndex;
	int uniqLabel;			//pre asembly napr. pre rozlisenie while1, while2 a tak :)
	int deepLabel;			//zanorenie

    unsigned in_declaration;        //bool ci sa momentalne nachadzam v deklaracii funkcie
    unsigned not_declared_function; //bool ci sa nachadzam este v nedeklarovanej funkci
    unsigned in_function;
    unsigned 

} ParserData;

//Main parsera
int parse();