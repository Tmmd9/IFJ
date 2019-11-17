#include "scanner.h" //ziskavanie tokenov
#include "stack.h" //indent/dedent + neviem co vsetko
#include "error.h" 
#include "expr.h" //precedencna tabulka pre semantiku
#include "symtable.h"	//tabulka symbolov
#include <stdbool.h>

//struktura co bude uchovavat vsetko dolezite pre parser
typedef struct {
    symTable globalT;      	//na tabulku globalnych symbolov (identifikatorov etc)
    symTable localT;        //na tabulku localnych symbolov (identifikatorov etc)

	token Token;
	/*
	*	veci potrebne k tvorbe stromu 
	*/
	Data * currentID;		//momentalne spracovavane
	Data * leftID;			//lava strana premennej
	Data * rightID;			//prava strana funkcie, vyrazu

    // ked checkujem parametre, tak index toho ktory momentalne spracovavam
	unsigned paramIndex;
	int uniqLabel;			//pre asembly napr. pre rozlisenie while1, while2 a tak :)
	int deepLabel;			//zanorenie

    unsigned in_declaration;        //bool ci sa momentalne nachadzam v deklaracii funkcie
    unsigned not_declared_function; //bool ci sa nachadzam este v nedeklarovanej funkci
    unsigned in_function;
    unsigned in_if_while;

  	//  tStack *stack;                  
	//dunno if necessary ale tak kvoli INDENTOM pre 
	//scanner mal by si to skener sam initnut
} ParserData;

//Main parsera
int parse();