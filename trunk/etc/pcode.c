/* pcode.c
 * jojo
 */

#include <stdio.h>
#include <stdlib.h>
#include "pcode.h"

void pcodeGenAddress(ASTTREE tree, SYMTABLE s, SYMTABLE function) // function = fonction courante
{ 

  SYMTABLE node;
  int location;
  int niveau;
  int tmp;
  int upBound;
  
  if (tree == NULL) {
    return;
  }
  
  switch (tree->id) 
    {
    case AT_ARG :
    case AT_VAR :
      node = alreadyIsSymbol(s, tree->sval,0, -1, 0, 0);
   
      // calcul l'écart d'imbrication entre la fonction et la variable   
      niveau = s->levelNode - 1;
      if(niveau <= function->levelNode) 	
	niveau = function->levelNode - niveau;
      else{
	niveau = niveau - function->levelNode;
      }
      
      tmp = 0;
     
      if(node->ref != 1) {   // argument ou variable passee par valeur (avant s)
	if(node->varType == VAL_INT) {
	  printf("lda i %d %d\n",niveau,node->address);
	}
	if(node->varType == VAL_BOOL) {
	  printf("lda b %d %d\n",niveau,node->address);
	}
      }
      
      if(node->ref == 1) {   // argument passe par reference	

	printf(";reference pcodegenadress\n");
	printf(";s->levelNode %d s->id %s %s niveau = %d state %d type %d \n",s->levelNode, s->id, node->id, function->levelNode, node->state, node->varType );
	if( function->levelNode == 1 ) {
	  tmp = - 5;
	}

	upBound = getMaxMemoryUsage(function->up);
	if ( upBound > 0 ) {
	  upBound--;
	}
	
	/*
	if(s->varType == VAL_INT) {
	  //printf("lod a %d %d\n",niveau,(node->address + tmp + 5) ); // ok p4? 
	  printf("lda i %d %d\n",niveau ,upBound - (node->address + tmp)  ); // ok p3 
	}
	else if (s->varType == VAL_BOOL) {
	  printf("lda b %d %d\n",niveau + 1,upBound - (node->address + tmp)  ); // ok p3 
	  //printf("lod a %d %d\n",niveau,(node->address + tmp + 5)  ); // ok p4?
	}
	*/	

	printf("lod a %d %d\n",niveau,(node->address + tmp + 5)  ); // ok p4?

      }
      
      break;
      
    default:
      printf(";ERROR : unrecognized type=%d in pcodeGenAddress(..)\n", tree->type);
      fprintf(stderr,";KO\n");
      exit(1);
    }

}


void pcodeGenValue(ASTTREE tree, SYMTABLE s)
{

  SYMTABLE node, nodeR;
  int location, argument, lvl;
  static int staticlabel = 0;

  int n_par;
  int niveau;

  int id_while;
  int id_if;

  char type_return;

  staticlabel++;

  if (tree != NULL && s != NULL)
    {
	
      switch (tree->id) 
	{
	case AT_ROOT: 
	  
	  node = s;

	  if (tree->left != NULL) {   
	    pcodeGenValue(tree->left,s);
	  }
	  
	  printf("stp\n");	  
	  break;

	case AT_FUNCT : 

	  // trouver la fonction
	  node = alreadyIsSymbol(s, tree->sval, 1, tree->fnctId, 0, 0);
	  if(node != NULL  ) {

	    if (tree->left != NULL) {
	      pcodeGenValue(tree->left,node->down);
	    }
	    if(tree->right != NULL) {
	      pcodeGenValue(tree->right,node->down);
	    }
	      	      
	    // return des fonction void et diffente du main
	    if(node->varType == VAL_NOTYPE && strcmp(s->id, "main") != 0 && node->fnctFor != 1 ) {
	      printf("retp\n");
	    }
	    
	  }
	  
	  break; 

	case AT_HEADFUNCT :  // arguments fonction, entre les () // non utilise
	  
	  printf(";at_headfunc\n");
	  if( tree->left != NULL ) {
	    pcodeGenValue(tree->left,s);
	  }
	  break;

	case AT_LISTPARAM :  // non utilise

	  printf(";entree listparam\n");
	  if( tree->left != NULL && tree->right == NULL ) {
	    pcodeGenValue(tree->left,s->down);	    
	  }
	  else if( tree->right != NULL ) {
	    pcodeGenValue(tree->left,s->down);
	    pcodeGenValue(tree->right,s->down);
	  }
	  break;

	case AT_CORPS :

	  // reservation memoire pour les fonctions autre que main
	  printf("define @%s_%d_%d\n", s->up->id, s->up->levelNode, s->up->fnctId);  

	  if( strcmp(s->up->id, "main") != 0 ) {
	    printf("ssp %d\n",getMaxMemoryUsage(s->up) + 5);
	  }
	  else {
	    printf("ssp %d\n",getMaxMemoryUsage(s->up) + 0);
	  }
	  printf("ujp @%s_body_%d_%d\n", s->up->id, s->up->levelNode, s->up->fnctId);
	  
	  // pcode bloc decla
	  if (tree->left != NULL ){
	    pcodeGenValue(tree->left,s);
	  }
	  // pcode implementation	  
	  if (tree->right != NULL ){
	    printf("define @%s_body_%d_%d\n", s->up->id, s->up->levelNode, s->up->fnctId);
	    pcodeGenValue(tree->right,s);
	  }
	  break;
      
	case AT_IMPLEMENT :

	  if(tree->left != NULL) {
	    pcodeGenValue(tree->left,s);
	  }
	  break;

	case AT_BLOCDECLA :
	  if(tree->left != NULL)
	    pcodeGenValue(tree->left,s);
	  break;
		
	case AT_DECLA : 
	  if(tree->right->id != AT_VAR) {
	      pcodeGenValue(tree->right,s);
	  }
	  if(tree->left != NULL ) {
	    pcodeGenValue(tree->left,s);			
	  }	   
	  break;			
	
	case AT_INSTRUCTION : 	
	  if(tree->right != NULL && tree->left != NULL) {
		pcodeGenValue(tree->left,s);		
		pcodeGenValue(tree->right,s);		
	  }
	  if(tree->right == NULL && tree->left != NULL) {
		pcodeGenValue(tree->left,s);
	  }
	  break;

	case AT_AFFECT : 
	  node = alreadyIsSymbol(s, tree->left->sval, 0, -1, 0, 0);
	  if(node != NULL) {
	    pcodeGenAddress(tree->left, node ,s->up);
	    pcodeGenValue(tree->right, s);
	    if(node->varType == VAL_INT) // affectation d'entier
	      {
		printf("sto i\n");
	      }
	    if(node->varType == VAL_BOOL) // ... ou de booleens
	      {
		printf("sto b\n");
	      }
	  }
	  break;	

	case AT_EXPRD :  
	  if (tree->left != NULL){
	    pcodeGenValue(tree->left,s);
	  }
	  break;

	case AT_WRITE : 
	  pcodeGenValue(tree->right, s);
	  printf("prin\n");		
	  break;

	case AT_READ : 
	  node = alreadyIsSymbol(s, tree->right->sval, 0, -1, 0, 0);
	  pcodeGenAddress(tree->right, node ,s->up);

	  printf("read\n");
	  printf("sto i\n");
	  break;

	case AT_PLUS :
	  pcodeGenValue(tree->left, s);
	  pcodeGenValue(tree->right, s);
	  printf("add i\n");	
	  break;
			
	case AT_MOINS :
	  pcodeGenValue(tree->left, s);
	  pcodeGenValue(tree->right, s);
	  printf("sub i\n");
	  break;
		
	case AT_FOIS : 
	  pcodeGenValue(tree->left, s);
	  pcodeGenValue(tree->right, s);
	  printf("mul i\n");
	  break;
			
	case AT_NEG : 
	  pcodeGenValue(tree->right, s);
	  printf("neg i\n");
	  break;
		    
	case AT_DIVISE : 
	  pcodeGenValue(tree->left, s);
	  pcodeGenValue(tree->right, s);
	  printf("div i\n");
	  break;
			
	case AT_NOT :
	  pcodeGenValue(tree->right, s);
	  printf("not b\n");
	  break;
		
	case AT_AND : 
	  pcodeGenValue(tree->left, s);
	  pcodeGenValue(tree->right, s);
	  printf("and b\n");
	  break;
		
	case AT_OR : 
	  pcodeGenValue(tree->left, s);
	  pcodeGenValue(tree->right, s);
	  printf("or b\n");
	  break;
		
	case AT_LT : 	  
	  pcodeGenValue(tree->left, s);
	  pcodeGenValue(tree->right, s);
	  printf("les b\n");
	  break;
		
	case AT_LE :
	  pcodeGenValue(tree->left, s);
	  pcodeGenValue(tree->right, s);
	  printf("leq b\n");
	  break;
		
	case AT_EQUAL : 
	  pcodeGenValue(tree->left, s);
	  pcodeGenValue(tree->right, s);
	  printf("equ b\n");
	  break;
			
	case AT_NB :
	  printf("ldc i %d\n", tree->ival);
	  break;
		
	case AT_BOOL : 
	  printf("ldc b %d\n",tree->ival);
	  break;
		
	case AT_VAR:
	  
	  node = alreadyIsSymbol(s, tree->sval, 0, -1, 0, 0);
	  pcodeGenAddress(tree, node ,s->up);	  

	  if(node->varType == VAL_INT ) {
	    printf("ind i\n");
	  }
	  if(node->varType == VAL_BOOL) {
	      printf("ind b\n");
	  }
	  	  
	  break;
	  
	// condition bool du if
	case AT_IF :
	  if ( tree->right != NULL ) {
	    pcodeGenValue(tree->left, s);
	    pcodeGenValue(tree->right, s);
	  }
	  break;

	// instruction du then
	case AT_InstructionIF :

	  id_if = staticlabel;

	  printf("fjp @fi_%d\n", id_if);
	  pcodeGenValue(tree->left, s);
	  printf("ujp @fi_%d\n", id_if);
	  printf("define @fi_%d\n", id_if);

	  break;

	// instruction du if then else
	case AT_InstructionIFELSE :
	  
	  id_if = staticlabel;

	  printf("fjp @else_%d\n", id_if);
	  if( tree->left != NULL ) {
	    pcodeGenValue(tree->left, s);	    
	  }
	  printf("ujp @fi_%d\n", id_if);
	  printf("define @else_%d\n", id_if);
	  if( tree->right != NULL ) {
	    pcodeGenValue(tree->right, s);
	  }
	  printf("define @fi_%d\n", id_if);

	  break;

	// boucle while
	case AT_WHILE :
	  id_while = staticlabel;
	  printf("define @while_%d\n", id_while);
	  pcodeGenValue(tree->left, s);
	  
	  printf("fjp @od_%d\n", id_while);
	  
	  if( tree->right != NULL ) {
	    pcodeGenValue(tree->right, s);
	  }
	  
	  printf("ujp @while_%d\n", id_while);
	  printf("define @od_%d\n", id_while);
	  
	  break;
	
	case AT_RETURN :
	  // determiner type de retour: entier ou booleen
	  printf("; type ret = %d\n", s->up->varType);
	  if( s->up->varType == VAL_INT ) {
	    type_return = 'i';
	  }
	  else {
	    type_return = 'b';
	  }
	  
	  // generation pcode retour
	  printf("lda %c 0 0\n", type_return);
	  pcodeGenValue(tree->right, s);
	  printf("sto %c\n", type_return);
	  printf("retf\n");
	  break;


	case AT_FUNCTPARAM :
	  printf(";entree funcparam\n");
	  if( tree->right != NULL && tree->left == NULL ) {
	    pcodeGenValue(tree->right,s);	    
	  }
	  else if( tree->left != NULL ) {
	    pcodeGenValue(tree->left,s);
	    pcodeGenValue(tree->right,s);
	    
	  }
	  break;

	case AT_ARG :
	 
	  pcodeGenAddress(tree, node ,s);
	  
	  break;

	case AT_APPELF :
	  
	  printf(";calcul diff de profondeur d\n");
	  node = alreadyIsSymbol(s, tree->sval, 1, tree->fnctId, 0, 0);
	  SYMTABLE nodeTemp = node;

	  niveau = s->levelNode;
	  if(niveau <= node->levelNode) {
	    niveau = node->levelNode - niveau;
	  }
	  else{
	    niveau = niveau - node->levelNode;
	  }	 
	  printf("mst %d\n", niveau);
 	  
	  printf(";generation nombre et pcode parametres\n");	  	  
	  node = node->down;

	  // a mettre au dessus!
	  ASTTREE treeTmp = tree;
	  int     tmp     = 0;
	  int     upBound = getMaxMemoryUsage(node->up);
	  if( upBound > 0 ) { upBound--; }                 // cause : parcours a l envers
	  SYMTABLE nodeL = NULL;
	  
	  n_par = 0;
	  
	  if (tree->right != NULL) {
	    tree = tree->right;
	    n_par++;
	    if (node->ref == 1) {
	      
	      nodeL = alreadyIsSymbol(s, tree->right->sval, 2, -1, 0, 0);

	      if( node->levelNode == 1 ) {
	        tmp = 5;
	      }
	      
	      if(node->varType == VAL_INT) {
		//printf("lda i %d %d\n",niveau, upBound - (node->address - tmp) );
		
		if(niveau == 0) {
		  printf("lda i %d %d\n",niveau,nodeL->address - tmp );  // ok :)
		}
		else {
		  printf("lod a %d %d\n",0,nodeL->address - tmp );
		}
		
	      }
	      
	      else if (node->varType == VAL_BOOL) {
		//printf("lda b %d %d\n",niveau, upBound - (node->address - tmp) );
		
		if(niveau == 0) {
		  printf("lda b %d %d\n",niveau,nodeL->address - tmp );  // ok :)
		}
		else {
		  printf("lod a %d %d\n",0,nodeL->address - tmp );
		}
		
	      }
	      
	    }
	    else {
	      pcodeGenValue(tree->right, s);
	    }
	    
	    
	    while (tree->left != NULL) {
	      tree = tree->left;
	      node = node->next;
	      n_par++;
	      if (node->ref == 1) {
		
		nodeL = alreadyIsSymbol(s, tree->right->sval, 2, -1, 0, 0);
	
		printf("; **** appel ref 2 pour var %s, tree = %s, level = %d\n", node->id, tree->right->sval, nodeL->levelNode);
	
		if( node->levelNode == 1 ) {
		 tmp = 5;
		}
		
		if(node->varType == VAL_INT) {
		  //		  printf("lda i %d %d\n",niveau,upBound - (nodeL->address - tmp) );
		  
		  if(niveau == 0) {
		    printf("lda i %d %d\n",niveau,nodeL->address - tmp );  // ok :)
		  }
		  else {
		    printf("lod a %d %d\n",0,nodeL->address - tmp );
		  }
		}
		
		else if (node->varType == VAL_BOOL) {
		  //printf("lda b %d %d\n",niveau, upBound - (node->address - tmp) );
		
		  if(niveau == 0) {
		    printf("lda b %d %d\n",niveau,nodeL->address - tmp );  // ok :)
		  }
		  else {
		    printf("lod a %d %d\n",0,nodeL->address - tmp );
		  }

		}
		
		
	      }
	      else {
		pcodeGenValue(tree->right, s);
	      }
	    }
	    
	    
	  }
	  
	  node = node->up;	  
	 
	  printf(";appel de %s_%d_%d\n", nodeTemp->id, nodeTemp->levelNode, nodeTemp->fnctId);  
	  printf("cup %d @%s_%d_%d\n", n_par, nodeTemp->id, nodeTemp->levelNode, nodeTemp->fnctId);

	  break;
	  
	case AT_FORWARD :
	  break;

	default:
	  printf(";ERROR : unrecognized type=%d in pcodeGenValue(..)\n", tree->type);
	  fprintf(stderr,";KO\n");
	  
  	}

    }

}

