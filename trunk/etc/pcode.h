/* pcode.h
 *
 */

#ifndef PCODE_H_
#define PCODE_H_

#include "ast.h"
#include "sym.h"

// Declaration fonctions

void pcodeGenAddress(ASTTREE tree, SYMTABLE s, SYMTABLE function);
/*
	objets utilises : tree = arbre genere par le compilateur, s = variable recherchee, function = la fonction courante
	PRE : s ne doit pas être NULL et function ne doit pas être NULL.
	POST : Génère le p-code qui récupère l'adresse de la variable.
*/

void pcodeGenValue(ASTTREE tree, SYMTABLE s);
/*
	objets utilises : tree = arbre genere par le compilateur, s = element de l'environnement courant
	PRE : /
	POST : Génère le p-code associé au noeud en train d'être analysé
*/
void nPara( ASTTREE tree, int * n );
#endif /*PCODE_H_*/
