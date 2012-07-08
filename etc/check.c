/* check.c
 *
 */

#include "check.h"
	
int check(ASTTREE tree, SYMTABLE tds)
{
	SYMTABLE node; 
	int type;
    	if (tds == NULL) { 
		printf("; Tds Empty \n");
		return KO; }
	if (tree == NULL) {
		printf("; Tree Empty \n");
		return KO; }
	else{
		switch (tree->id)
		{
			case AT_ROOT : 
				
				if(tree->left != NULL) {
					return (check(tree->left, tds));
				}		
				break;

			
			case AT_FUNCT : 
					node = alreadyIsSymbol(tds, tree->sval, 1, tree->fnctId, 1, 0);
					if(node != NULL)
					{
						if(tree->left != NULL) {
							return check(tree->left, node->down); 
						}
						if(tree->right != NULL) {
							return check(tree->right, node->down); 
						}
						
					}else {
						printf(";ERROR : la fonction %s est inexistante \n", tree->sval);
						return KO;
					}
				
				break; 
			case AT_HEADFUNCT : 
				if(tree->left != NULL) {
					return check(tree->left,tds);
				}
				else 
				{
					return OK;
				}
				break;
			case AT_CORPS : 
				if(tree->left != NULL && tree->right != NULL) {
					return check(tree->right, tds) && check(tree->left, tds);
				}
				break;
			case AT_IMPLEMENT : 
				if(tree->left != NULL) {
					return check(tree->left, tds);
				}
				if(tree->right == NULL && tree->left == NULL) {
					return OK;
				}
				break;
			case AT_BLOCDECLA :
				return OK;
				break;
			case AT_INSTRUCTION : 
				if(tree->right != NULL && tree->left != NULL) {
					return check(tree->right, tds) && check(tree->left, tds);
				}
				if(tree->right == NULL && tree->left != NULL) {
					return check(tree->left, tds);
				}
				else
				{
					return OK;
				}
				break;
			case AT_AFFECT : 
				node = alreadyIsSymbol(tds,tree->left->sval,3, tree->fnctId, 1, 0); 
				if (node != NULL)
				{	switch(node->varType)
					{
						
						case VAL_INT : 
							if(checkType(tree->right,VAL_INT,tds) == 2) // Si bien un int -> OK
							{
								return OK;
							}
							
							break;
						
						case VAL_BOOL : 
							if(checkType(tree->right,VAL_BOOL,tds)==3) // Si bien un boolean -> OK
							{
								
								return OK;
							}
							
							break;
						default : printf(";Erreur l'affectation a %s est mal typee\n",tree->left->sval);
							return KO;
					}
					return KO;
				}else{
					
					printf(";Erreur impossible de faire une affectation sur %s car non declaree\n",tree->left->sval);
					return KO;
				}
				
				break;
			case AT_EXPRD :  
				if(tree->left != NULL) {
					if (tree->left->id == 0) { // Si tree->left->id est AT_VAR (type 5 dans l'arbre et autre type dans la table des symboles)
						return OK;
					}
					else {
						type = tree->left->type;
						return checkType(tree->left, type, tds);
					}
				}
				break;

			case AT_WRITE : 
				if(tree->right != NULL) {
					// Verifie si le type du noeud left est VAL_INT
					if(checkType(tree->right, VAL_INT, tds)==2)
					{
						return OK;
					}else{
						printf(";Erreur de typage avec WRITE\n");
						return KO;
					} 
				}
				else 
					return KO;
				break;
			case AT_READ : 
				if(tree->right != NULL) {
					// Verifie si le type du noeud right est VAL_INT
					if(checkType(tree->right, VAL_INT, tds)==2)
					{
						return OK;
					}else{
						printf(";Erreur de typage avec READ\n");
						return KO;
					}  
				}
				else 
					return KO;
				
				break;
			case AT_RETURN : 
				if(tree->right != NULL) {
					type = tds->up->varType;
					return checkType(tree->right,type,tds);
					
				} return OK;						
				break;	
			case AT_IF : 
				// Si "ExprD" est de type VAL_BOOL
				if(checkType(tree->left,VAL_BOOL,tds) == 3)
				{
					// Si instruction vide dans le bloc
					if(tree->right == NULL)
						return OK;
					else
						return check(tree->right,tds);
					}else{	
						printf(";Error IF\n");
					return KO;
				}
				break;
	

			case AT_InstructionIF : 
				if(tree->left != NULL) {
					return check(tree->left, tds);
				}
				else
				{
					return OK;
				}
				break;	
			case AT_InstructionIFELSE : 
				if(tree->left != NULL)
				{
					return check(tree->left,tds);
				}
				if(tree->right != NULL)
				{
					return check(tree->right,tds);
				}
				if(tree->left == NULL && tree->right == NULL)
				{
					return OK;
				}
				break;	
			case AT_WHILE : 
				// Si "ExprD" est de type VAL_BOOL
				if(checkType(tree->left,VAL_BOOL,tds) == 3)
				{
					if(tree->right == NULL)
						return OK;
					else
						return check(tree->right,tds);
				}else{	
					printf(";Error WHILE\n");
					return KO;
				}
				break;			
			case AT_FUNCTPARAM :
				if(tree->left != NULL)
				{
					return check(tree->left,tds);
				}
				if(tree->right != NULL)
				{
					return check(tree->right,tds);
				}	
				break;	
			
			
			
			default :
				printf("; Erreur de typage check\n");
				fprintf(stderr,";KO\n");
				return KO;
		}
	}
}



int checkType(ASTTREE tree, int type, SYMTABLE tds)
{ 

int nbOverLoad;

	SYMTABLE node,function;
	if (tree == NULL)
		return 0;
	else{
		switch (tree->id)
		{

			
			case AT_VAR : 
				node = alreadyIsSymbol(tds, tree->sval, 3, tree->fnctId, 1, 0); 
				if(node != NULL) // si variable existe
				{
					if(node->varType == type)
					{	return type;
						
					}else{
						printf(";ATTENTION ERREUR TYPE ! %d - %d \n", node->varType, type);
						return KO;
					}
				}else{
					printf(";Variable %s n'existe pas\n",tree->sval);
					return KO;
				}
				break;
			
			case AT_PLUS :
				if(type != VAL_INT)
				{
					printf(";Error de typage avec PLUS\n");
					return KO;
				}
				else
				{
					// Verifie si le type de gauche et de droit sont des int
					if(checkType(tree->left,VAL_INT,tds)==2 && checkType(tree->right,VAL_INT,tds)==2) 
						return VAL_INT;
					else {
						printf(";Error : VAL_INT OK but -> AT_PLUS\n");
						return KO; 
					} 
				}	
				break;

			
			case AT_MOINS :
				if(type != VAL_INT)
				{
					printf(";Error de type avec MOINS\n");
					return KO;
				}
				else
				{	// Verifie si le type de gauche et de droit sont des int
					if(checkType(tree->left,VAL_INT,tds)==2 && checkType(tree->right,VAL_INT,tds)==2)
						return VAL_INT;
					else {
						printf(";Error : VAL_INT OK but -> AT_MOINS\n");
						return KO; 
						} 
				}
				break;

			case AT_FOIS : 
				if(type != VAL_INT)
				{
					printf(";Error de typage avec FOIS\n");
					return KO;
				}
				else
				{	// Verifie si le type de gauche et de droit sont des int
					if(checkType(tree->left,VAL_INT,tds)==2 && checkType(tree->right,VAL_INT,tds)==2)
						return VAL_INT;
					else {
						printf(";Error : VAL_INT OK but -> AT_FOIS\n");
						return KO; 
					} 
				}
				break;


			
			case AT_NEG : 
				if(type != VAL_INT)
				{
					printf(";Erreur de typage avec NEG\n");
					return KO;
				}else{
					return checkType(tree->right,VAL_INT,tds);
				}
				break;

		    
			case AT_DIVISE : 
				if(type != VAL_INT)
				{
					printf(";Error de typage avec DIVISE\n");
					return KO;
				}
				else{
					// Verifie si le type de gauche et de droit sont des int
					if(checkType(tree->left,VAL_INT,tds)==2 && checkType(tree->right,VAL_INT,tds)==2)
						return VAL_INT;
					else {
						printf(";Error : VAL_INT OK but -> AT_DIVISE\n");
						return KO; 
						} 
				}
				break;

			
			case AT_NOT : 
				if(type != VAL_BOOL)
				{
					printf(";Error de typage avec NOT\n");
					return KO;
				}
				else{	
					if(checkType(tree->right, VAL_BOOL,tds)==3)
						return VAL_BOOL;
					else 
					{
						printf(";Error : VAL_BOOL OK but -> AT_NOT\n");
						return KO; 
					}
				}
				break;

		
			case AT_AND : 
				if(type != VAL_BOOL)
				{
					printf(";Error de typage avec AND\n");
					return KO;
				}
				else{	// Verifie si le type de gauche et de droit sont des bool
					if(checkType(tree->left,VAL_BOOL,tds)==3 && checkType(tree->right,VAL_BOOL,tds)==3)
						return VAL_BOOL;
					else {
						printf(";Error : VAL_BOOL OK but -> AT_AND\n");
						return KO; }
				}
				break;

		
			case AT_OR : 
				if(type != VAL_BOOL)
				{
					printf(";Error de typage avec OR\n");
					return KO;
				}
				else{
					// Verifie si le type de gauche et de droit sont des bool
					if(checkType(tree->left,VAL_BOOL,tds)==3 && checkType(tree->right,VAL_BOOL,tds)==3)
						return VAL_BOOL;
					else {
						printf(";Error : VAL_BOOL OK but -> AT_OR\n");
						return KO; } 
				}
				break;

		
			case AT_LT : 
				if(type != VAL_BOOL)
				{
					printf(";Erreur de typage pour opération '<'\n");
					return KO;
				}else{
						// Verifie si le type de gauche et de droit sont des int
						if(checkType(tree->left, VAL_INT, tds)==2 && checkType(tree->right,VAL_INT, tds)==2)
						{ 
							return VAL_BOOL;
						}else{
							printf(";Error : VAL_BOOL OK but -> AT_LT\n");
							return KO;
						}
				}
				break;

			
			case AT_LE :
				if(type != VAL_BOOL)
				{
					printf(";Erreur de typage pour opération '<='\n");
					return KO;
				}else{		// Verifie si le type de gauche et de droit sont des int
						if(checkType(tree->left, VAL_INT, tds)==2 && checkType(tree->right,VAL_INT, tds)==2)
						{ 
							return VAL_BOOL;
						}else{
							printf(";Error : VAL_BOOL OK but -> AT_LE\n");
							return KO;
						}
				}
				break;

		
			case AT_EQUAL : 
				if(type != VAL_BOOL)
				{
					printf(";Erreur de typage pour opération '='\n");
					return KO;
				}else{		// Verifie si le type de gauche et de droit sont des int
						if(checkType(tree->left, VAL_INT, tds)==2 && checkType(tree->right,VAL_INT, tds)==2)
						{ 
							return VAL_BOOL;
						}else{
						  printf(";Error : VAL_BOOL OK but -> AT_EQUAL\n");
						  return KO;
						}
				}
				break;

			
			case AT_NB : 
				if (type == VAL_INT)
					return VAL_INT;
				else{
					printf(";ERREUR DE TYPAGE ! \n");
					return KO;
				}
				break;

		
			case AT_BOOL : 
				if (type == VAL_BOOL)
					return VAL_BOOL;
				else{
					printf(";ERREUR DE TYPAGE ! \n");
					return KO;
				}
				break;
	
			case AT_APPELF :
				
				nbOverLoad = checkNbFunctSymbol(tds, tree->sval, 1, -1, 0, 0); // renvoie le nombre d'overload de la fonction

				if(nbOverLoad != -1) // si fonction existe
				{ 
					int cpt = 1; // 1 car elle existe dans la TDS

					while(cpt <= nbOverLoad)
					{

						int check = 0; // boolean pour detecter si erreur quand l'appel de la fonction
						function = alreadyIsSymbol(tds, tree->sval,1, cpt, 1, 0); // renvoie le noeud de la fonction dans le TDS
						
						// si appel de fonction sans arg				
						if(tree->right == NULL) 
						{
							// si la fonction a bien aucun argument dans sa déclaration
							if(function->down->state != 2) 
							{
								if(type == function->varType) 
									return type;
								else{	
									check = 1;
									cpt ++;
									continue;
									// ERROR : La fonction retourne un type incorrect
								}
							}else{
								check = 1;
								cpt ++;
								continue;
								//ERROR Appel de la fonction avec trop peu arguments
								
							}	
						}
						else
						{
							// si appel de fonction AVEC arg
							ASTTREE treeSafe = tree; // Fait une copie du noeud de l'arbre
							node = function->down; // On pointe sur les arguments de la fonction
							treeSafe = treeSafe->right; // Node AT_FUNCTPARAM
							int b = 0; // Boolean pour gerer les erreurs dans la boucle while
							while(treeSafe != NULL){
	
								if(node->state != 2 || node == NULL)
								{
									check = 1;
									cpt ++;
									b = 1;
									break;
									//ERROR Appel de la fonction avec trop arguments
								}
	
								if(checkType(treeSafe->right,node->varType,tds) != node->varType)  
								{
		
									check = 1;
									cpt ++;
									b = 1;
									break;
									//ERROR Argument mal type dans l'appel de fonction 
								}
	
								node = node->next; // Argument suivant dans TDS
								treeSafe = treeSafe->left; // Argument suivant dans Arbre
	
							}
							
							if(b == 1)
							{
								// Erreur donc on passe au la fonction suivante -> cpt +1
								continue;
							}		
							if(node != NULL && node->state == 2)
							{
	
								check = 1;
								cpt ++;
								continue;
								//ERROR Appel de la fonction avec trop peu arguments
							}

							if(type == function->varType) 
							{
								return type;
							}else{
								check = 1;
								cpt ++;
								continue;
								// ERROR : La fonction retourne un type incorrect
								
							}
						}



					}
					// End while
					if(check == 0)
					{
						return OK; 
					}
					else
					{	// Error à l'appel de la fonction
						return KO;
					}

				}else{
					printf("; La fonction %s est inexistante\n",tree->sval);
					return KO;
				}
				break;

			
			default : printf(";Erreur de typage check type\n");
						fprintf(stderr,";KO\n");
						return KO;
		}
	}
}
