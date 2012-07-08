/* sym.h
 *
 */

#ifndef SYM_H_
#define SYM_H_

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"


struct SYMBOLETABLE {

  char * id;     // Nom variable
  int address;   // Position de la variable en mémoire
  int varType;   // Type de variable : entier, boolean
  int levelNode; // Niveau d'imbrication
  int state;     // Etat :  0 variable, 1 function, 2 argument, 3 (tout sauf fonction)
  int ref;       // arg par copie (0) ou par reference (1)
  int fnctId;    // suffixe du nom de la fonction -> 0 par defaut et 1 si au moins une fonction créée
  int fnctFor; // Boolean si fonction est une fonction forward (1) ou pas (0)
  
  int UId;
	
  struct SYMBOLETABLE* next;     // Pointeur vers le prochain symbole
  struct SYMBOLETABLE* previous; // Pointeur vers le symbole précedent
  struct SYMBOLETABLE* up;       // Pointeur vers le symbole parent
  struct SYMBOLETABLE* down;     // Pointeur vers le symbole enfant

};

// Definition des structures
typedef struct SYMBOLETABLE NOEUD;
typedef NOEUD * SYMTABLE;

// int schFnct;	 utilisé pour rechercher une fonction en fonction de sa surcharge. 0 -> pas rechercher, 1 -> chercher

// Declaration et prototypes des fonctions
SYMTABLE creaNode();

void printSymbolTableGraphViz(SYMTABLE s);

SYMTABLE checkSymbolLevel(SYMTABLE s, char* name, int state, int fnctId, int schFnct, int fnctFor);

SYMTABLE alreadyIsSymbolLevel(SYMTABLE s, char* name,int state, int fnctId, int schFnct, int fnctFor);

SYMTABLE alreadyIsSymbol(SYMTABLE s, char* name, int state, int fnctId, int schFnct, int fnctFor);

SYMTABLE addToSymbolTable(SYMTABLE s, char* name, int state, int type, int fnctId, int schFnct, int fnctFor);

int checkNbFunctSymbol(SYMTABLE s, char* name, int state, int fnctId, int schFnct, int fnctFor);

SYMTABLE returnFunctSymbol(SYMTABLE s, char* name, int state, int fnctId, int schFnct, int fnctFor);

void freeSymbolTable(SYMTABLE s);

void computeLocations(SYMTABLE s);

int getMaxMemoryUsage(SYMTABLE s);

void printSymbolTable(SYMTABLE s);

int fillTable (ASTTREE tree, SYMTABLE s, int currentType);

#endif
