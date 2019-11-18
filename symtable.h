/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: symtable.c
 *				- hlavickovy subor pre hash tabulku (tabulka symbolov)
 *				- prevzata z predmetu IJC, autor: David Oravec
 *				- modifikovana pre ulahcenie prace v nasom IFJ projekte
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "str.h"

//max velkost musi byt pvocislo, moze byt este modifikovana podla potreby
//a podla casovych vysledkov
#define MAX_HTAB_SIZE 32771

typedef enum {
    DTYPE_INT,          //datovy typ INT
    DTYPE_DOUBLE,       //datovy typ DOUBLE
    DTYPE_STRING,       //datovy typ STRING
    DTYPE_UNDEFINED,    //NEDEFINOVANY datovy typ
} DataType;

typedef struct {
    DataType type;      //jeden z datovych typov
    string *param;      //parametre        
    char *identifier;   //identifikator                     
    bool isDefined;     //bool ci bola funkcia definovana
    bool isGlobal;      //bool na globalne premenne
    unsigned paramCount; //pocet parametrov vo funkci -neviem ci to je uplne potreba ked DH to nemal ale mne by t mohlo pomoct
} Data;

typedef struct item {
    char *key;      //identifikator (kluc)              ///toto bude nazov definicie funkcie/premennej?
    Data data;      //data o polozke
    struct item *next;  //ukazatel na dalsiu polozku
} htabItem;

typedef htabItem *htab[MAX_HTAB_SIZE];  //samotna tabulka symbolov
/*
 * Rozptylovacia funkcia, prevzata z predmetu IJC z letneho semestra 2019
 */
unsigned int htab_hash_function(const char *str);

/*
 * Funkcia v ktorej sa inicializuju jednotlive polozky tabulky
 */
void htabInit(htab *table);

/*
 * Funkcia na uvolnenie poloziek tabulky
 */
void htabFree(htab *table);

/*
 * Funkcia ktora zabezpeci pridanie symbolu do tabulky
 */
Data *htabAddSymbol(htab *table, const char *key, bool *allocErr);

/*
 * Funkcia na vyhladanie urcitej polozky v tabulke
 */
Data *htabSearch(htab *table, const char *key);

/*
 * Funkcia na priradenie typu parametru polozke v tabulke
 *
 * vracia true, ak sa podari pridanie, inak false
 */
bool htabAddParam(Data *data, unsigned int DataType);

/*
 * Funkcia na odstranenie urcitej polozky z tabulky
 *
 * vracia true ked sa odstranenie podari, inak false
 */
bool htabDelete(htab *table, const char *key);


#endif //SYMTABLE_H
