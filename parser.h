#include "scanner.h" //ziskavanie tokenov
#include "stack.h" //indent/dedent + neviem co vsetko
#include "error.h" 
#include "expr.h" //precedencna tabulka pre semantiku
/******pridat include pre hash tabulku****/

//struktura co bude uchovavat vsetko dolezite pre parser
typedef struct {
	/*Neni dokoncene, ked ta napadne co
	sem pridat tak to sem pridaj a daj koment ze preco */
	token Token;
	/*sem pojdu ukazatele na predchadzajuci,nasledujuci 
	a sucastny symbol v hash tabulke */

} ParserData;

//Main parsera
int parse();