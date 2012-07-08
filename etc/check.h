/* check.h
 *
 */

#ifndef CHECK_H_
#define CHECK_H_

#include <stdio.h>
#include "ast.h"
#include "sym.h"
#define OK 1
#define KO 0


int check (ASTTREE tree, SYMTABLE tds);

int checkType (ASTTREE tree, int type, SYMTABLE tds);

#endif 
