/* lsd12.l
 *
 * 
 *  
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "y.tab.h"

int num_lines=1;
%}

%option noyywrap
%x comment

var       [A-Za-z]([A-Za-z]|[0-9])*
nbr       (0|[1-9][0-9]*)
boolean   ("true"|"false")
blank     [ \t]+
type	  ("int"|"bool"|"iset"|"void")

%%


"{"	{BEGIN(comment);}
<comment>"}"   {BEGIN(INITIAL);}
<comment>"{"   {
                  fprintf(stderr,"KO\n");
                  printf("ERROR : invalid '%s' in line %d\n",yytext, num_lines);
                  exit(0);
                  }
<comment>"\n"	{++num_lines;}
<comment>.	{/*On passe*/}


"program"	{return PROG;}

"return" 	{return RETURN;}	/* RETURN variable/value */
"read"   	{return READ;}		/* scanf */
"write"  	{return WRITE;}		/* printf */

"function"     	{return FUNCTION;}
"forward"	{return FORWARD;}	/* Postpose implementation*/
"var" 		{return REF;}		/* Variable passee par reference */
"begin" 	{return BEG;}		/* Ending of declaration bloc (if exist), Begin of implementation */
"end" 		{return END;}		/* Ending of implementation */

"if" 		{return IF;}		/* IF (e) THEN i1 ELSE i2 FI */
"then" 		{return THEN;}
"else" 		{return ELSE;}
"fi" 		{return FI;}

"while" 	{return WHILE;}		/* WHILE (e) DO i OD */
"do" 		{return DO;}
"od" 		{return OD;}

"add"     	{return ADD;}		/* Ex: ADD a TO ensA */
"to"     	{return TO;}

"remove" 	{return REMOVE;}	/* Ex: REMOVE a FROM ensA */
"from"  	{return FROM;}

":="	{return AFFECT;}	/* Ex int b := 1 */

"+"     {return PLUS;}		/* Logical oparators */
"-"     {return MINUS;}
"*"     {return TIMES;}
"("     {return LP;}
")"     {return RP;}
","     {return COMMA;}
"||"	{return OR;}
"="	{return EQUAL;}
"!"	{return NOT;}

":"	{return TWOPOINT;}
"<"	{return LT;}
"<="	{return LE;}
"&&"	{return AND;}

"/"	{return DIVISE;}

"#"	{return CARDINAL;}
";" 	{return FIN;}		/* End of instruction */



{type} {
	if(strcmp(yytext, "void")==0) 
		{yylval.ival=5;} // avant -> 1
	else
	{
		if(strcmp(yytext, "int")==0) {yylval.ival=2;}
		else
		{
			if(strcmp(yytext, "bool")==0)
				{yylval.ival=3;}
			else
				{yylval.ival=4;}				
		}
	}
	return TYPE;
}

{boolean} {
	if(strcmp(yytext, "true")==0) 
		{yylval.ival=1;}
	else 
		{yylval.ival=0;}
	return BOOLEAN;
}

{var} {
        yylval.sval=(char*)calloc(strlen(yytext)+1,sizeof(char));
	strcpy(yylval.sval,yytext);
	return VAR;
}
			 
{nbr} {
        yylval.ival=atoi(yytext);
	return NB;
}

"\n"	{++num_lines;}

{blank} {/*On passe*/}

. {
        fprintf(stderr,"KO\n");
        printf("ERROR : invalid '%s' in line %d\n",yytext, num_lines);
        exit(0);
}

%%

