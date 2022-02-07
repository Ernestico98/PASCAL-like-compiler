%{
	#include <bits/stdc++.h>
	using namespace std;
	#include "y.tab.h"
	
	int line_num = 1;
%}


%%

"program"	{ return PROGRAM;  }
"var" 		{ return VAR;  }
"real" 		{ return REAL;  }
"char"		{ return CHAR;  }
"boolean"	{ return BOOLEAN; }
"integer"	{ return INTEGER; }
"string" 	{ return STRING; }
"list"		{ return LIST; }
"of" 		{ return OF; }
"begin"		{ return BEG; }
"end"		{ return END; }
":="		{ return OP_ASIGNACION; }
"div"		{ return DIV; }
"mod" 		{ return MOD; }
"if"		{ return IF; }
"then"		{ return THEN; }
"else"		{ return ELSE; }
"while"		{ return WHILE; }
"do"		{ return DO; }
"and"		{ return AND; }
"or"		{ return OR; }
"read"		{ return READ; }
"write"		{ return WRITE; }
"len"		{ return LEN; }
"append"	{ return APPEND; }

"<="		{ return LE; }
">="		{ return GE; }
"<>"		{ return NE; }
"<"			{ return LESS; }
">"			{ return GREATER; }
"="			{ return EQ; }

"false"						{
								yylval.info.bval = false;
								yylval.info.sval = new char[yyleng];
								strcpy ( yylval.info.sval, yytext );
								return LITERAL_BOOL;	
							}
							
"true"						{
								yylval.info.bval = true;
								yylval.info.sval = new char[yyleng];
								strcpy ( yylval.info.sval, yytext );
								return LITERAL_BOOL;
							}	


[a-zA-Z_][a-zA-Z0-9_]* 		{ 
								yylval.info.sval = new char[yyleng];
								strcpy ( yylval.info.sval, yytext );
								return ID;
							}


[0-9]+						{ 
								yylval.info.ival = atoi ( yytext );
								yylval.info.sval = new char[yyleng];
								strcpy ( yylval.info.sval, yytext );
								return LITERAL_INT;
							}


[0-9]+\.[0-9]*				{
								yylval.info.rval = atof ( yytext );
								yylval.info.sval = new char[yyleng];
								strcpy ( yylval.info.sval, yytext );
								return LITERAL_REAL;
							}


'.'							{
								yylval.info.cval = yytext[1];
								yylval.info.sval = new char[yyleng];
								strncpy ( yylval.info.sval, yytext+1, 1 );
								return LITERAL_CHAR;								
							}

\"[^\"]*\"						{
								yylval.info.sval = new char[yyleng];
								strcpy ( yylval.info.sval, yytext+1 );
								yylval.info.sval[yyleng-2] = '\0';
								return LITERAL_STRING;	
							}

"//"[^\n]*					;	/*Ignorando los comentarios*/

							
"["			{ return *yytext; }
"]"			{ return *yytext; }
[\)\(\}\{]	{ return *yytext; }
[-*+;:,\.]	{ return *yytext; }
[ \t]+	;
[\n]	{line_num ++;}

. 			{ 
				cerr << "En linea " << line_num << " : caracter invalido " << yytext << endl;
				exit(1);
			}

%%


int yywrap ( ) {
	cerr << "La compilacion ha finalizado con exito" << endl;
	return 1;
} 
