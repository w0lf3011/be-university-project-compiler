/* lsd12.y
 *
 */

%{
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "sym.h"
#include "check.h"
#include "pcode.h"

extern int num_lines;
extern char* yytext;
  
// to avoid 'implicit definition'
int yylex(void);
int yyerror(char *str);

ASTTREE root;
%}


// définition du type des variables $x
%union{
  int ival;
  char *sval;
  ASTTREE tval;
}

%token PRINT READ WRITE 

%token PLUS MINUS TIMES ADD AND NOT LT EQUAL OR LE 

%token TO FUNCTION REF REMOVE FROM PROG FORWARD 

%token LP RP COMMA AFFECT CARDINAL TWOPOINT

%token IF FI THEN ELSE WHILE DO OD RETURN BEG END FIN 

%token <sval> VAR
// token des valeurs de variables
%token <ival> NB BOOLEAN 
// token des types
%token <ival> TYPE

%left AND
%left OR 
%left NOT
%left LT EQUAL LE 
%left PLUS MINUS
%left TIMES DIVISE

%type <tval> Lsd12 BlocDecla Decla Var Funct HeadFunct Corps Implement ExprD Instruction InstructionList InstructionIF ListParam DeclaParam FunctParam

// indiquer le non-terminal de commencement
%start Lsd12

%%

Lsd12: PROG VAR FIN Funct END FIN
        { root = createNode(AT_ROOT, VAL_NOTYPE, 0, 0, 0, $2, $4, NULL);}
;

Funct: FUNCTION VAR HeadFunct TWOPOINT TYPE FIN Corps
	{ $$ = createNode(AT_FUNCT, $5, 0, 0, 0, $2, $7, $3);}
;

Corps: BlocDecla BEG Implement END FIN
	{ $$ = createNode(AT_CORPS, VAL_NOTYPE, 0, 0, 0, NULL, $1, $3);}
       | FORWARD FIN
	{ $$ = createNode(AT_FORWARD, VAL_NOTYPE, 0, 0, 0, NULL, NULL, NULL);}
;

Implement: InstructionList 
	{ $$ = createNode(AT_IMPLEMENT, VAL_NOTYPE, 0, 0, 0, NULL, $1, NULL);}
;

HeadFunct: LP RP
	{ $$ = createNode(AT_HEADFUNCT, VAL_NOTYPE, 0, 0, 0, NULL, NULL, NULL);}
	| LP ListParam RP
	{ $$ = createNode(AT_HEADFUNCT, VAL_NOTYPE, 0, 0, 0, NULL, $2, NULL);}
;

ListParam: DeclaParam 
	{ $$ = createNode(AT_LISTPARAM, VAL_NOTYPE, 0, 0, 0, NULL, $1, NULL);}
	| DeclaParam COMMA ListParam
	{ $$ = createNode(AT_LISTPARAM, VAL_NOTYPE, 0, 0, 0, NULL, $1, $3);}
;

DeclaParam : REF VAR TWOPOINT TYPE
	{ $$ = createNode(AT_ARG, $4, 1, 0, 0, $2, NULL, NULL);}
	| VAR TWOPOINT TYPE
	{ $$ = createNode(AT_ARG, $3, 0, 0, 0, $1, NULL, NULL);}
;

BlocDecla: REF Decla 
	{ $$ = createNode(AT_BLOCDECLA, VAL_NOTYPE, 0, 0, 0, NULL, $2, NULL);}
	| REF  
	{ $$ = createNode(AT_BLOCDECLA, VAL_NOTYPE, 0, 0, 0, NULL, NULL, NULL);}

;

Decla:  Var TYPE FIN 
	{ $$ = createNode(AT_DECLA, $2, 0, 0, 0, NULL, NULL, $1);}
	| Var TYPE FIN Decla
	{ $$ = createNode(AT_DECLA, $2, 0, 0, 0, NULL, $4, $1);}
	| Funct 
	{ $$ = createNode(AT_DECLA, VAL_NOTYPE, 0, 0, 0, NULL, NULL, $1);}
	| Funct Decla
	{ $$ = createNode(AT_DECLA, VAL_NOTYPE, 0, 0, 0, NULL, $2, $1);}
;

InstructionList : { $$ = NULL;}
		| FIN 
   	        { $$ = createNode(AT_INSTRUCTION, VAL_NOTYPE, 0, 0, 0, NULL, NULL, NULL);}
		| FIN Instruction FIN InstructionList
   	        { $$ = createNode(AT_INSTRUCTION, VAL_NOTYPE, 0, 0, 0, NULL, $2, $4);}
                | Instruction FIN InstructionList
                { $$ = createNode(AT_INSTRUCTION, VAL_NOTYPE, 0, 0, 0, NULL, $1, $3);}
;

Instruction : ExprD
              { $$ = createNode(AT_EXPRD, VAL_NOTYPE, 0, 0, 0, NULL, $1, NULL);}
	      | Var AFFECT ExprD
              { $$ = createNode(AT_AFFECT, VAL_NOTYPE, 0, 0, 0, NULL, $1, $3);}
	      | READ ExprD
	      { $$ = createNode(AT_READ, VAL_NOTYPE, 0, 0, 0, NULL, NULL, $2);}
	      | WRITE ExprD
	      { $$ = createNode(AT_WRITE, VAL_NOTYPE, 0, 0, 0, NULL, NULL, $2);}
	      | RETURN ExprD
	      { $$ = createNode(AT_RETURN, VAL_NOTYPE, 0, 0, 0, NULL, NULL, $2);}
	      | IF LP ExprD RP THEN InstructionIF FI 
	      { $$ = createNode(AT_IF, VAL_NOTYPE, 0, 0, 0, NULL, $3, $6);}
	      | WHILE LP ExprD RP DO InstructionList OD 
	      { $$ = createNode(AT_WHILE, VAL_NOTYPE, 0, 0, 0, NULL, $3, $6);}	      	      
;

FunctParam: { $$ = NULL;}
	   | ExprD
           { $$ = createNode(AT_FUNCTPARAM, VAL_NOTYPE, 0, 0, 0, NULL, NULL, $1);}
	   | ExprD COMMA FunctParam
           { $$ = createNode(AT_FUNCTPARAM, VAL_NOTYPE, 0, 0, 0, NULL, $3, $1);}
;

InstructionIF : InstructionList
	        { $$ = createNode(AT_InstructionIF, VAL_NOTYPE, 0, 0, 0, NULL, $1, NULL);}
		| InstructionList ELSE InstructionList
		{ $$ = createNode(AT_InstructionIFELSE, VAL_NOTYPE, 0, 0, 0, NULL, $1, $3);}	
;

// Expressions Droites 
ExprD : Var  { $$ = $1;}
	| NB
        { $$ = createNode(AT_NB, VAL_INT, 0, 0, yylval.ival, NULL, NULL, NULL);}
	| ExprD TIMES ExprD
        { $$ = createNode(AT_FOIS, VAL_INT, 0, 0, 0, NULL, $1, $3);}
   	| ExprD DIVISE ExprD
        { $$ = createNode(AT_DIVISE, VAL_INT, 0, 0, 0, NULL, $1, $3);}
   	| ExprD PLUS ExprD
        { $$ = createNode(AT_PLUS, VAL_INT, 0, 0, 0, NULL, $1, $3);}
   	| ExprD MINUS ExprD
        { $$ = createNode(AT_MOINS, VAL_INT, 0, 0, 0, NULL, $1, $3);}
   	| MINUS ExprD
        { $$ = createNode(AT_NEG, VAL_INT, 0, 0, 0, NULL, NULL, $2);}
	| ExprD EQUAL ExprD
        { $$ = createNode(AT_EQUAL, VAL_BOOL, 0, 0, 0, NULL, $1, $3);}
	| ExprD LT ExprD
        { $$ = createNode(AT_LT, VAL_BOOL, 0, 0, 0, NULL, $1, $3);}
   	| ExprD LE ExprD
        { $$ = createNode(AT_LE, VAL_BOOL, 0, 0, 0, NULL, $1, $3);}        
	| BOOLEAN
        { $$ = createNode(AT_BOOL, VAL_BOOL, 0, 0, yylval.ival, NULL, NULL, NULL);}
	| NOT ExprD
        { $$ = createNode(AT_NOT, VAL_BOOL, 0, 0, 0, NULL, NULL, $2);}
	| ExprD AND ExprD
        { $$ = createNode(AT_AND, VAL_BOOL, 0, 0, 0, NULL, $1, $3);}
   	| ExprD OR ExprD
        { $$ = createNode(AT_OR, VAL_BOOL, 0, 0, 0, NULL, $1, $3);}
   	| LP ExprD RP
        { $$ = $2;}
	| VAR LP FunctParam RP
        { $$ = createNode(AT_APPELF, VAL_NOTYPE, 0, 0, 0, $1, NULL, $3);}
;


Var: VAR { $$ = createNode(AT_VAR, VAL_NOTYPE, 0, 0, 0, yylval.sval, NULL, NULL);}

;

%%

int yyerror(char *str)
{
        fprintf(stderr,"KO\n");
        printf("ERROR '%s' AT LINE  %d : UNRECOGNISED '%s'\n", str,num_lines, yytext);
        exit(0);
}

// Programme principal
int main() { 

  SYMTABLE sym;
  
  printf("; *** Compiler ***\n");
  printf(";\n");
  
  printf(";*** BEGIN yyparse() ***\n");
  yyparse();
  printf(";*** END yyparse() ***\n");

  /*
    printf(";*** BEGIN printTree(..) ***\n");
    printTree(root);
    printf(";*** END printTree(..) ***\n");
  */
 
  printf(";*** BEGIN SymbolTable ***\n");
  sym = creaNode();
  fillTable(root, sym, -1);
  printf(";*** END SymbolTable ***\n");

  /*
    printf(";*** BEGIN printTreeGraphViz(..) ***\n");
    printTreeGraphViz(root);
    printf(";*** END printTreeGraphViz(..) ***\n");

    printf(";*** BEGIN printSymbolTableGraphViz(..)  ***\n");
    printSymbolTableGraphViz(sym);
    printf(";*** END printSymbolTableGraphViz(..)  ***\n");
  */ 

  printf("; * Verification de la specification LSD12 :\n");
  if (check(root,sym) != 1) {
    freeTree(root);
    freeSymbolTable(sym);
    fprintf(stderr,"KO\n");
    exit(1);
  }
  printf("; * Fin de la verification de la specification LSD12!\n");
  
  printf(";*** BEGIN computeLocations(..) ***\n");
  computeLocations(sym);
  printf(";*** END computeLocations(..) ***\n");

  /*  
  printf(";*** BEGIN SymbolTable ***\n");
  printSymbolTable(sym);
  printf(";*** END SymbolTable ***\n");
  */
  
  printf(";*** BEGIN PCodeGeneration ***\n");
  pcodeGenValue(root, sym);
  printf(";*** END PCodeGeneration ***\n");
 

  printf(";*** BEGIN Cleaning ***\n");
  freeTree(root);
  freeSymbolTable(sym);
  printf(";*** END Cleaning ***\n");
  
  fprintf(stderr,"OK\n");
 
  return 0;

}
