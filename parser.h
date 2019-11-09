#include "scanner.h" //ziskavanie tokenov
#include "stack.h" //indent/dedent + neviem co vsetko
#include "error.h" 
#include "expr.h" //precedencna tabulka pre semantiku
/******pridat include pre hash tabulku****/

//struktura co bude uchovavat vsetko dolezite pre parser
typedef struct {
    symTable globalT;               //na tabulku globalnych symbolov (identifikatorov etc)
    symTable localT;                //na tabulku localnych symbolov (identifikatorov etc)
	/*Neni dokoncene, ked ta napadne co
	sem pridat tak to sem pridaj a daj koment ze preco */
	token Token;
    tStack *stack;                  //dunno if necessary ale tak kvoli INDENTOM pre scanner

	/*sem pojdu ukazatele na predchadzajuci,nasledujuci
	 * smting like TDATA* currentID
	 *              TDATA * lefthandside_var
	 *              TDATA * righthandside_var
	a sucastny symbol v hash tabulke */

    // ked checkujem parametre, tak index toho ktory momentalne spracovavam
	unsigned paramIndex;
	//int uniqLabel;
	//int deepLabel;

    unsigned in_declaration;        //bool ci sa momentalne nachadzam v deklaracii funkcie
    unsigned not_declared_function; //bool ci sa nachadzam este v nedeklarovanej funkci

} ParserData;

//Main parsera
int parse();