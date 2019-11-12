/*
 *	Predmet : Formální jazyky a překladače
 *	Projekt : Implementace překladače imperativního jazyka IFJ19
 *	Súbor	: symtable.c
 *				- modul s hash tabulkou (tabulka symbolov)
 *				- prevzata z predmetu IJC, autor: David Oravec
 *				- modifikovana pre ulahcenie prace v nasom IFJ projekte
 *	Tím		: číslo 10 / varianta II
 *	Autori	: Dominik Boboš (xbobos00)
 *			  Peter Hudeček (xhudec34)
 *			  Timotej Kováčik (xkovac49)
 *			  Dávid Oravec (xorave05)
 *
 */

#include <stdlib.h>
#include "symtable.h"

//rozptylovacia funkcia
unsigned int htab_hash_function(const char *str) {
    uint32_t h=0;     // musí mat 32 bitov
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++){
        h = 65599*h + *p;
    }
    return h % MAX_HTAB_SIZE;
}

//inicializacia tabulky
void htabInit(htab *table){
    if (table == NULL) return;

    for (unsigned int i = 0; i < MAX_HTAB_SIZE; i++){
        (*table)[i] = NULL;
    }
}

Data *htabAddSymbol(htab *table, const char *key, bool *allocErr){
    *allocErr = false;      //bool ktory sa nastavi na true pri chybe alokacii

    if (!table || !key){
        *allocErr = true;
        return NULL;
    }

    unsigned int hash = htab_hash_function(key);

    htabItem *tmp = (*table)[hash];
    htabItem *last = NULL;

    while (tmp){
        if(!strcmp(tmp->key, key)){
            return NULL;
        }
        tmp = tmp->next;
        last = tmp;
    }
    //alokacia pamate na novu polozku v tabulke
    htabItem *newItem = (htabItem *) malloc(sizeof(htabItem));
    if (!newItem){
        *allocErr = true;
        return NULL;
    }
    //alokacia pamata pre kluc v polozke
    if (!(newItem->key = (char *) malloc(sizeof(strlen(key) + 1)))){
        free(newItem);
        *allocErr = true;
        return NULL;
    }
    //alokacia miesta pre parametre v polozke
    if (!(newItem->data.param = (string *) malloc(sizeof(string)))){
        free(newItem->key);
        free(newItem);
        *allocErr = true;
        return NULL;
    }
    //inicializacia parametrov v polozke
    if (!stringInit(newItem->data.param)){
        free(newItem->data.param);
        free(newItem->key);
        free(newItem);
        *allocErr = true;
        return NULL;
    }

    strcpy(newItem->key,key);
    newItem->data.isDefined = false;
    newItem->data.isGlobal = false;
    newItem->data.identifier = newItem->key;
    newItem->data.type = DTYPE_UNDEFINED;
    newItem->next = NULL;

    if (!last){
        (*table)[hash] = newItem;
    } else {
        last->next = newItem;
    }
    return &newItem->data;
}

bool htabAddParam(Data *data, unsigned int DataType){
    if (!data) return NULL;

    if (DataType == DTYPE_INT){
        if (!stringAddChar(data->param,'i')) {
            return false;
        }
    } else if (DataType == DTYPE_DOUBLE){
        if (!stringAddChar(data->param,'d')) {
            return false;
        }
    } else if (DataType == DTYPE_STRING) {
        if (!stringAddChar(data->param,'s')) {
            return false;
        }
    }
    return true;
}

Data *htabSearch(htab *table, const char *key){
    if (!table || !key) return NULL;

    unsigned int hash = htab_hash_function(key);

    htabItem* tmp = (*table)[hash];	//pristup k polozke v tabulke na indexe

    // prechadzam polozky na danom indexe
    while (tmp) {
        if (!strcmp(tmp->key, key)) {	// ak sa zhoduje  s key, vraciam
            return &tmp->data;
        }
        // posuvam ukazatel na dalsi polozku na indexe
        tmp = tmp->next;
    }

    // prvok nebol najdeny, vraciam NULL
    return NULL;
}

void htabFree(htab *table){
    if (!table) return;

    htabItem *tmp, *next;

    for (unsigned int i = 0; i < MAX_HTAB_SIZE; i++){
        for (tmp = (*table)[i]; tmp != NULL; tmp = next){
            next = tmp->next;
            free(tmp->key);

            if (!tmp->data.param){
                stringFree(tmp->data.param);
                free(tmp->data.param);
            }
            free(tmp);
        }
        (*table)[i] = NULL;
    }
}

bool htabDelete(htab *table, const char *key){
    if (!table || !key) return NULL;

    unsigned int hash = htab_hash_function(key);
    htabItem *item = (*table)[hash];
    htabItem *nextItem, *prevItem = NULL;

    while (item){
        nextItem = item->next;
        if (!strcmp(item->key,key)){    //ak sa keys zhoduju
            if (!prevItem){ //a polozka nema pred sebou ziadnu inu
                //nasledujucu polozku ukladam priamo do tabulky na dany index
                (*table)[hash] = nextItem;
            } else {
                //predchadzajuca polozka ma ukazatel na nasledujucu
                prevItem->next = nextItem;
            }
            free(item->key);

            if (!item->data.param){
                stringFree(item->data.param);
                free(item->data.param);
            }
            free(item);
            return true;
        }
        prevItem = item;	//ukladam aktualnu polozku ako predchadzajucu
        item = nextItem;	//aktualna polozka bude dalsou polozkou
    }
    return false;
}