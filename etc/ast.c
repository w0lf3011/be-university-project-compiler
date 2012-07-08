/* ast.c
*/

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

ASTTREE createNode(int id,
		   int type,
		   int varRef,
		   int fnctId,
		   int ival,
		   char* sval,
		   ASTTREE left,
		   ASTTREE right)
{
  ASTTREE node = (ASTTREE) malloc(sizeof(ASTNODE));
  if (node == NULL)
    {
      fprintf(stderr,"KO\n");
      printf("ERROR : malloc failed in createNode(..)\n");
      exit(1);
    }
  else
    {
      node->id = id;
      node->type = type;
      node->varRef = varRef;
      node->fnctId = fnctId;
      node->ival = ival;
      node->sval = sval;
      node->left = left;
      node->right = right;      
      //node->level = -1;

      return node;
    }
}

void freeNode(ASTTREE node)
{
  if (node != NULL)
    {
      if (node->sval != NULL) free(node->sval);
      if (node->left != NULL) freeNode(node->left);
      if (node->right != NULL) freeNode(node->right);

      free(node);
    }
}


void freeTree(ASTTREE tree) // idem above but top root is static
{
  if (tree != NULL)
    {
      if (tree->sval != NULL) free(tree->sval);
      if (tree->left != NULL) freeNode(tree->left);
      if (tree->right != NULL) freeNode(tree->right);
    } 
}


char* humanReadableNodeType(int id)
{
  switch(id) {

  case AT_FUNCTPARAM:      return "AT_FUNCTPARAM"; break;
  case AT_APPELF:      return "AT_APPELF"; break;
  case AT_ARG:      return "AT_ARG"; break;
  case AT_LISTPARAM:      return "AT_LISTPARAM"; break;
  case AT_WHILE:      return "AT_WHILE"; break;
  case AT_InstructionIFELSE:      return "AT_InstructionIFELSE"; break;
  case AT_InstructionIF:      return "AT_InstructionIF"; break;
  case AT_IF:      return "AT_IF"; break;
  case AT_WRITE:      return "AT_WRITE"; break;
  case AT_READ:      return "AT_READ"; break;
  case AT_RETURN:      return "AT_RETURN"; break;
  case AT_AFFECT:      return "AT_AFFECT"; break;
  case AT_NOT:      return "AT_NOT"; break;
  case AT_EQUAL:      return "AT_EQUAL"; break;
  case AT_GE:      return "AT_GE"; break;
  case AT_GT:      return "AT_GT"; break;
  case AT_LE:      return "AT_LE"; break;
  case AT_LT:      return "AT_LT"; break;
  case AT_OR:      return "AT_OR"; break;
  case AT_AND:      return "AT_AND"; break;
  case AT_NEG:      return "AT_NEG"; break;
  case AT_MODULO:      return "AT_MODULO"; break;
  case AT_DIVISE:      return "AT_DIVISE"; break;
  case AT_FOIS:      return "AT_FOIS"; break;
  case AT_MOINS:      return "AT_MOINS"; break;
  case AT_EXPRD:      return "AT_EXPRD"; break;
  case AT_INSTRUCTION:      return "AT_INSTRUCTION"; break;
  case AT_NB:      return "AT_NB"; break;
  case AT_BOOL:      return "AT_BOOL"; break;
  case AT_PLUS:      return "AT_PLUS"; break;
  case AT_VAR:      return "AT_VAR"; break;
  case AT_CORPS:   return "AT_CORPS"; break; 
  case AT_IMPLEMENT:  return "AT_IMPLEMENT"; break;
  case AT_FORWARD: return "AT_FORWARD"; break;
  case AT_HEADFUNCT:      return "AT_HEADFUNCT"; break;
  case AT_FUNCT:      return "AT_FUNCT"; break;
  case AT_BLOCDECLA: return "AT_BLOCDECLA"; break;
  case AT_DECLA:    return "AT_DECLA"; break;
  case AT_ROOT:     return "AT_ROOT"; break;
  default :         return "??";
  }
}

void printTree(ASTTREE tree)
{
  if (tree != NULL) 
    {
      printf("; [%p] id=%s, type=%d, ival=%d, sval=", tree, humanReadableNodeType(tree->id), tree->type, tree->ival);
      if (tree->sval == NULL) printf("NULL");
      else printf("'%s'", tree->sval);
      printf(", left=%p, right=%p\n", tree->left, tree->right);

      printTree(tree->left);
      printTree(tree->right);
    }
}

void printTreeGraphViz(ASTTREE tree)
{

	if (tree != NULL) 
	{
		if(strcmp(humanReadableNodeType(tree->id), "AT_ROOT")==0)
		{
			printf("\"%p\";\n",tree);
		}
		// Affichage du numero du noeud
		//Affichage de IVAL
		if (tree->ival == -1) // si pas de variable numérique
		{
			if (tree->sval == NULL) // si pas de variable numérique
			{
			printf("\"%p\"[shape=box, color=\"#e6e8f2\", fontcolor=\"#000000\",label=\"%s \\n\"];\n", tree, humanReadableNodeType(tree->id));
			}
			else
			{
			printf("\"%p\"[shape=box, color=\"#e6e8f2\", fontcolor=\"#000000\",label=\"%s \\n Sval = %s \\n fnctId = %d\" ];\n", tree, humanReadableNodeType(tree->id), tree->sval, tree->fnctId);	
			}
		}
		else
		{
			if (tree->sval == NULL) // si pas de variable numérique
			{
			printf("\"%p\"[shape=box, color=\"#e6e8f2\", fontcolor=\"#000000\",label=\"%s \\n Ival = %d\"];\n", tree, humanReadableNodeType(tree->id),tree->ival);
			}
			else
			{
			printf("\"%p\"[shape=box, color=\"#e6e8f2\", fontcolor=\"#000000\",label=\"%s \\n Ival = %d\\n Sval = %s \\n  fnctId=%d \"];\n", tree, humanReadableNodeType(tree->id),tree->ival, tree->sval, tree->fnctId);
			}
		}

		if(tree->left != NULL)
		{
			printf("\"%p\" -> \"%p\";\n", tree, tree->left);
		}

		if(tree->right != NULL)
		{
			printf("\"%p\" -> \"%p\";\n", tree, tree->right);
		}

		printTreeGraphViz(tree->left);
		printTreeGraphViz(tree->right);
	}
}

