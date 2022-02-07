%{
	#include <bits/stdc++.h>
	using namespace std;
	
	enum { t_real, t_char, t_boolean, t_integer, t_string, t_list  } tip;
	enum { op_le, op_ge, op_ne, op_eq, op_greater, op_less } op;
	
	extern int line_num;
	int yylex();
	void yyerror(const char*);
	void my_error ( const char *s );
	
	
	int cont = 0;
	typedef pair <int,int> tip_t;
	map <string,tip_t> sym;
	string to_string ( char * );
	string get_str_type ( int );
	string get_str_op_comp ( int );
	int label = 0;
	char *get_label();
	char *to_cad(int);
%}                 

%union {
	struct data {
		int ival;
		char *sval;
		double rval;
		char cval;
		bool bval;	
		char *str;				
		
		int tipo, subtipo;
		char *code;
		
		vector < char* > *vstr;
	}	info;
};

%define parse.error verbose
%define parse.lac full


%token PROGRAM VAR REAL CHAR BOOLEAN INTEGER STRING LIST OF BEG END OP_ASIGNACION DIV MOD IF THEN ELSE WHILE DO AND OR READ WRITE LEN APPEND
%token <info> ID
%token <info> LE GE NE LESS GREATER EQ

%token <info> LITERAL_INT      
%token <info> LITERAL_REAL
%token <info> LITERAL_CHAR
%token <info> LITERAL_BOOL
%token <info> LITERAL_STRING


%left AND OR
%left LE GE NE LESS GREATER EQ
%left '+' '-'
%left '*' DIV MOD
%left '('

%type <info> programa cabecera zona_declaracion variables declaracion_variables t_dato bloque sentencia sentencias asignacion variable elemento_lista expresion literal
%type <info> exp_unaria estructura_control alternativa condicional op_comparacion iterativa nombres_variables lista_variables lista_expresiones

%%
programa: 
								cabecera	zona_declaracion	bloque	'.'	{ 
																				char* code = new char[ strlen( $2.code ) + strlen ( $3.code ) + 10];
																				strcpy ( code, $2.code );
																				strcat ( code, $3.code );
																				cout << code;				
																			}
							;

			  
cabecera:
								PROGRAM	ID	';'		{ /*Do nothing*/ }
							;


zona_declaracion:	
								VAR variables	{ $$ = $2; }
							;
					
variables:
								declaracion_variables ';' { $$ = $1; }										
							|	variables	declaracion_variables	';'	{
																			$$.code = new char[ strlen($1.code) + strlen($2.code) + 10 ];
																			strcpy ( $$.code, $1.code );
																			strcat ( $$.code, $2.code );
																		}
							;


declaracion_variables:	
								nombres_variables ':' t_dato	{ 
																	string code;
																	for ( auto i : *$1.vstr ) {
																		string x = to_string(i);
																		if ( sym.count(x) ) {
																			char *msg = new char[100];
																			strcpy ( msg, "Error. Redeclaracion de la variable " );
																			strcat ( msg, x.c_str() );
																			my_error ( msg );
																		}
																		
																		tip_t t = make_pair ( $3.tipo, ( $3.tipo == t_list )? $3.subtipo : -1 );
																		sym[ x ] = t;
																		
																		code += "VAR ";
																		code += x;
																		code += "\n";
																	}
																	
																	code += "DECL_TYPE "; 
																	code += get_str_type ( $3.tipo );
																	if ( $3.tipo == t_list )
																		code += " " + get_str_type ( $3.subtipo );
																	code += "\n";
									
																	$$.code = new char[ code.length() + 10 ];
																	strcpy ( $$.code, code.c_str() );
																}
							;

							
nombres_variables:	
								ID	{ 
										$$.vstr = new vector<char*> ();
										$$.vstr->push_back ( $1.sval );
									}
							|	nombres_variables	','	ID 	{
																$$ = $1;
																$$.vstr->push_back ( $3.sval );
															}
							;

			
t_dato:							
								REAL	{ $$.tipo = t_real; }
							|	CHAR	{ $$.tipo = t_char; }
							|	BOOLEAN	{ $$.tipo = t_boolean; }
							|	INTEGER	{ $$.tipo = t_integer; }
							|	STRING	{ $$.tipo = t_string; }
							|	LIST OF	t_dato	{ 
													$$.tipo = t_list;
													if ( $3.tipo == t_list )
														my_error ("Error de tipo. No se admiten listas de mas de una dimensión"); 
													$$.subtipo = $3.tipo;
												}
							;


bloque:					
								BEG sentencias END	{ $$ = $2; } 
							;
							

sentencias:					
								sentencias	sentencia	{ 
															
															
															$$.code = new char[ strlen($1.code) + strlen($2.code) + 10 ];
															strcpy ( $$.code, $1.code );
															strcat ( $$.code, $2.code );
														}
							|	{ 
									
									$$.code = new char[2];
									$$.code[0] = '\0';
								}
							;
							

sentencia:					
								asignacion ';'	{ $$ = $1; }
							|	bloque	{ $$ = $1; }
							| 	estructura_control	{ $$ = $1; }
							|	READ	'('	lista_variables	')'	';'	{ $$ = $3; }
							|	WRITE	'('	lista_expresiones	')'	';'	{
																			$$.code = new char[ strlen($3.code) + 20 ];
																			strcpy ( $$.code, $3.code );
																			strcat ( $$.code, "WRITE_ENDL\n" );
																		}	
							|	ID	'.'	APPEND	'('	expresion	')'	';'	{
																			string tmp = to_string ( $1.sval );
																			if ( !sym.count(tmp) ) {
																				 char *msg = new char[100];
																				 strcpy ( msg, "Error. No ha sido declarada la variable ");
																				 strcat ( msg, tmp.c_str() );
																				 my_error( msg );
																			}
																			if ( sym[tmp].first != t_list ) {
																				 char *msg = new char[100];
																				 strcpy ( msg, "Error. La variable " );
																				 strcat ( msg, tmp.c_str() );
																				 strcat ( msg, " no es de tipo list" );
																				 my_error ( msg );
																			}
																			if ( sym[tmp].second != $5.tipo ) {
																				
																				char *msg = new char[100];
																				strcpy ( msg, "Error. No se puede insertar un elemento de tipo " );
																				strcat ( msg, get_str_type($5.tipo).c_str() );
																				strcat ( msg, " en una lista de tipo " );
																				strcat ( msg, get_str_type(sym[tmp].second).c_str() );
																				my_error ( msg );
																			}
																			$$.code = new char[ strlen($1.sval) + strlen($5.code) + 40 ];
																			strcpy ( $$.code, "PUSH_STR " );
																			strcat ( $$.code, $1.sval );
																			strcat ( $$.code, "\nCARGA\n" );
																			strcat ( $$.code, $5.code );
																			strcat ( $$.code, "APPEND\n" );
																		}
							;
								
								
lista_variables:
								variable	{ 
												if ( $1.tipo == t_list )
													 my_error ( "Error. No se puede leer una variable de tipo list"); 
												$$.code = new char[ strlen($1.code) + 20 ];
												strcpy ( $$.code, $1.code );
												strcat ( $$.code, "READ\n" );								
											}
							|	lista_variables	','	variable	{
																	if ( $3.tipo == t_list )
																		my_error ( "Error. No se puede leer una variable de tipo list"); 
																	$$.code = new char[ strlen($1.code) + strlen($3.code) + 20 ];
																	strcpy ( $$.code, $1.code );
																	strcat ( $$.code, $3.code );
																	strcat ( $$.code, "READ\n" );
																}
							;
							

lista_expresiones:			
								expresion	{
												if ( $1.tipo == t_list ) 
													my_error ( "Error. No se puede escribir una variable de tipo list" );
									
												$$.code = new char[strlen($1.code) + 20];
												strcpy ( $$.code, $1.code );
												strcat ( $$.code, "WRITE\n" );
											}
							|	lista_expresiones	','	expresion	{
																		if ( $3.tipo == t_list ) 
																			my_error ( "Error. No se puede escribir una variable de tipo list" );		
																		$$.code = new char[strlen($1.code) + strlen($3.code) + 40];
																		strcpy ( $$.code, $1.code );
																		strcat ( $$.code, "WRITE_SPACE\n" );
																		strcat ( $$.code, $3.code );
																		strcat ( $$.code, "WRITE\n" ); 
																		
																	}
							;
						

asignacion:			
								variable	OP_ASIGNACION	expresion	{ 
																			if ( $1.tipo != $3.tipo ) {
																				char *msg = new char[100];
																				strcpy ( msg, "No se puede asignar a una variable tipo " );
																				strcat ( msg, get_str_type( $1.tipo ).c_str() );
																				strcat ( msg, " una expresion de tipo ");
																				strcat ( msg, get_str_type( $3.tipo ).c_str() );
																				my_error ( msg ); 																				
																			}
																			$$.code = new char[ strlen($1.code) + strlen($3.code) + 20 ];
																			strcpy ( $$.code, $1.code );
																			strcat ( $$.code, $3.code );
																			strcat ( $$.code, "ASIGNA\n" );												
																		}
							;
	
	
variable:		
								ID	{ 
										string tmp = to_string ( $1.sval );
										if ( !sym.count(tmp) ) {
											 char *msg = new char[100];
											 strcpy ( msg, "Error. La variable " );
											 strcat ( msg, $1.sval );
											 strcat ( msg, " no ha sido declarada" );
											 my_error ( msg );
										}
										
										$$.tipo = sym[tmp].first;
										$$.subtipo = sym[tmp].second;
										$$.code = new char[ strlen($1.sval) + 40 ] ;
										strcpy ( $$.code, "PUSH_STR " );
										strcat ( $$.code, $1.sval );
										strcat ( $$.code, "\n" );
										strcat ( $$.code, "CARGA\n" );
									}
							|	elemento_lista	{ $$ = $1; }
							;
						
						
elemento_lista:					
								ID	'['	expresion	']'	{	
															string tmp = to_string ( $1.sval );
															if ( !sym.count(tmp) ) {
																 char *msg = new char[100];
																 strcpy ( msg, "Error. La variable " );
																 strcat ( msg, $1.sval );
																 strcat ( msg, " no ha sido declarada" );
																 my_error ( msg );
															}
															if ( sym[tmp].first != t_list && sym[tmp].first != t_string ) {
																 char *msg = new char[100];
																 strcpy  ( msg, "Error. La variable " );
																 strcat ( msg, tmp.c_str() );
																 strcat ( msg, " no es de un tipo contenedor");
																 my_error ( msg );
															}
									
															if ( $3.tipo != t_integer )
																my_error ( "Error de tipo: El tipo de un indice debe ser integer" );
															$$.tipo = ( sym[tmp].first == t_list? sym[tmp].second : t_char );
															$$.code = new char[ strlen( $1.sval ) + strlen ($3.code) + 40 ]; 
															strcpy ( $$.code, "PUSH_STR " );
															strcat ( $$.code, $1.sval );
															strcat ( $$.code, "\n" );
															strcat ( $$.code, "CARGA\n" );
															strcat ( $$.code, $3.code );
															strcat ( $$.code, "CARGA_IND\n" );
														}
							;


expresion:	                
								expresion	'+'	expresion	{ 		
																$$.code = new char[ strlen($1.code) + strlen($3.code) + 40 ]; 
																strcpy ( $$.code, $1.code );
																strcat ( $$.code, $3.code ); 
																if ( $1.tipo == t_string && $1.tipo == t_char ) { 
																	strcat ( $$.code, "SUM_STR_CHAR\n" );
																	$$.tipo = t_string;
																}
																else if ( $1.tipo == t_string && $3.tipo == t_string ) {
																	strcat ( $$.code, "SUM_STR\n" );
																	$$.tipo = t_string;
																}
																else if ( $1.tipo == t_integer && $3.tipo == t_integer ) {
																	strcat ( $$.code, "SUM_INT\n" );
																	$$.tipo = t_integer;
																}
																else if ( $1.tipo == t_real && $3.tipo == t_real ) {
																	strcat ( $$.code, "SUM_REAL\n" );
																	$$.tipo = t_real;
																}
																else {
																	char *msg = new char[100];
																	strcpy ( msg, "Error de tipo: No esta definida la operacion de suma para operandos de tipo " );
																	strcat ( msg, get_str_type($1.tipo).c_str() );
																	strcat ( msg, " y " );
																	strcat ( msg, get_str_type($3.tipo).c_str() );
																	my_error ( msg );
																}
															}
															
															
							|	expresion	'-'	expresion	{
																$$.code = new char[ strlen($1.code) + strlen($3.code) + 40 ]; 
																strcpy ( $$.code, $1.code );
																strcat ( $$.code, $3.code ); 
																if ( $1.tipo == t_integer && $3.tipo == t_integer ) {
																	strcat ( $$.code, "SUB_INT\n" );
																	$$.tipo = t_integer;
																}
																else if ( $1.tipo == t_real && $3.tipo == t_real ) {
																	strcat ( $$.code, "SUB_REAL\n" );
																	$$.tipo = t_real;
																}
																else {
																	char *msg = new char[100];
																	strcpy ( msg, "Error de tipo: No esta definida la operacion de resta para operandos de tipo " );
																	strcat ( msg, get_str_type($1.tipo).c_str() );
																	strcat ( msg, " y " );
																	strcat ( msg, get_str_type($3.tipo).c_str() );
																	my_error ( msg );
																}
															}
							|	expresion	'*'	expresion	{
																$$.code = new char[ strlen($1.code) + strlen($3.code) + 20 ]; 
																strcpy ( $$.code, $1.code );
																strcat ( $$.code, $3.code ); 
																if ( $1.tipo == t_integer && $3.tipo == t_integer ) {
																	strcat ( $$.code, "MUL_INT\n" );
																	$$.tipo = t_integer;
																}
																else if ( $1.tipo == t_real && $3.tipo == t_real ) {
																	strcat ( $$.code, "MUL_REAL\n" );
																	$$.tipo = t_real;
																}
																else {
																	char *msg = new char[100];
																	strcpy ( msg, "Error de tipo: No esta definida la operacion de multiplicación para operandos de tipo " );
																	strcat ( msg, get_str_type($1.tipo).c_str() );
																	strcat ( msg, " y " );
																	strcat ( msg, get_str_type($3.tipo).c_str() );
																	my_error ( msg );
																}
															}
							|	expresion	DIV	expresion	{
																$$.code = new char[ strlen($1.code) + strlen($3.code) + 40 ]; 
																strcpy ( $$.code, $1.code );
																strcat ( $$.code, $3.code ); 
																if ( $1.tipo == t_integer && $3.tipo == t_integer ) {
																	strcat ( $$.code, "DIV_INT\n" );
																	$$.tipo = t_integer;
																}
																else if ( $1.tipo == t_real && $3.tipo == t_real ) {
																	strcat ( $$.code, "DIV_REAL\n" );
																	$$.tipo = t_real;
																}
																else {
																	char *msg = new char[100];
																	strcpy ( msg, "Error de tipo: No esta definida la operacion de division para operandos de tipo " );
																	strcat ( msg, get_str_type($1.tipo).c_str() );
																	strcat ( msg, " y " );
																	strcat ( msg, get_str_type($3.tipo).c_str() );
																	my_error ( msg );
																}
															}
							|	expresion	MOD	expresion	{
																$$.code = new char[ strlen($1.code) + strlen($3.code) + 20 ]; 
																strcpy ( $$.code, $1.code );
																strcat ( $$.code, $3.code ); 
																if ( $1.tipo == t_integer && $3.tipo == t_integer ) {
																	strcat ( $$.code, "MOD_INT\n" );
																	$$.tipo = t_integer;
																}
																else {
																	char *msg = new char[100];
																	strcpy ( msg, "Error de tipo: No esta definida la operacion de modulo para operandos de tipo " );
																	strcat ( msg, get_str_type($1.tipo).c_str() );
																	strcat ( msg, " y " );
																	strcat ( msg, get_str_type($3.tipo).c_str() );
																	my_error ( msg );
																}
															}
							| 	'('	expresion	')'	{ $$ = $2; }
							|	literal	{ $$ = $1; }
							|	variable	{ $$ = $1; }
							|	exp_unaria	{ $$ = $1; }
							|	LEN	'('	ID	')'	{
													string tmp = to_string ( $3.sval );
													if ( !sym.count(tmp) ) {
														 char *msg = new char[100];
														 strcpy ( msg, "Error. La variable " );
														 strcat ( msg, $3.sval );
														 strcat ( msg, " no ha sido declarada" );
														 my_error ( msg );
													}
													
													if ( sym[tmp].first != t_list && sym[tmp].first != t_string ) {
														 char *msg = new char[100];
														 strcpy  ( msg, "Error. La variable " );
														 strcat ( msg, tmp.c_str() );
														 strcat ( msg, " no es de un tipo contenedor");
														 my_error ( msg );
													}
													
													$$.tipo = t_integer;
													$$.code = new char[ strlen($3.sval) + 40 ] ;
													strcpy ( $$.code, "PUSH_STR " );
													strcat ( $$.code, $3.sval );
													strcat ( $$.code, "\n" );
													strcat ( $$.code, "CARGA\n" );
													strcat ( $$.code, "LEN\n" );				
												}
							;

literal:				
								LITERAL_INT	{	
												$$.tipo = t_integer;
												$$.code = new char[ strlen($1.sval) + 20 ];
												strcpy ( $$.code, "LIT_INT ");
												strcat ( $$.code, $1.sval );
												strcat ( $$.code, "\n" );
											}
							|	LITERAL_REAL	{
													$$.tipo = t_real;
													$$.code = new char[ strlen($1.sval) + 20 ];
													strcpy ( $$.code, "LIT_REAL ");
													strcat ( $$.code, $1.sval );
													strcat ( $$.code, "\n" );								
												}
							|	LITERAL_CHAR	{
													$$.tipo = t_char;
													$$.code = new char[ strlen($1.sval) + 20 ];
													strcpy ( $$.code, "LIT_CHAR ");
													strcat ( $$.code, $1.sval );
													strcat ( $$.code, "\n" );													
												}
							|	LITERAL_BOOL	{
													$$.tipo = t_boolean;
													$$.code = new char[ strlen($1.sval) + 20 ];
													strcpy ( $$.code, "LIT_BOOL ");
													strcat ( $$.code, $1.sval );
													strcat ( $$.code, "\n" );									
												}
							|	LITERAL_STRING	{
													$$.tipo = t_string;
													$$.code = new char[ strlen($1.sval) + 20 ];
													strcpy ( $$.code, "LIT_STRING ");
													strcat ( $$.code, $1.sval );
													strcat ( $$.code, "\n" );									
												}
							;


exp_unaria:
								'-'	expresion	{ 
													if ( $2.tipo != t_integer && $2.tipo != t_real ) {
														char *msg = new char[100];
														strcpy ( msg, "El operador unario '-' no puede er aplicado a un operando de tipo " );
														strcat ( msg, get_str_type( $2.tipo ).c_str() );
														my_error ( msg );
													}
													$$.code = new char[ strlen($2.code) + 40 ];
													strcpy ( $$.code, $2.code );
													if ( $2.tipo == t_integer )
														strcat ( $$.code, "NEG_INT\n" );
													else
														strcat ( $$.code, "NEG_REAL\n" );
												}
							;
																					  


estructura_control:
								alternativa	{ $$ = $1; }
							|	iterativa	{ $$ = $1; }
							;
							

alternativa:
								IF	condicional	THEN	bloque	{ 
																	char *L1 = get_label();
																	$$.code = new char[ strlen($2.code) + strlen($4.code) + 40 ];
																	strcpy ( $$.code, $2.code );
																	strcat ( $$.code, "JF " );
																	strcat ( $$.code, L1 );
																	strcat ( $$.code, "\n" );
																	strcat ( $$.code, $4.code );
																	strcat ( $$.code, "LABEL " );
																	strcat ( $$.code, L1 );
																	strcat ( $$.code, "\n" ); 
																}
							|	IF	condicional	THEN	bloque	ELSE	bloque	{
																					char *L1 = get_label();
																					char *L2 = get_label();
																					$$.code = new char[ strlen($2.code) + strlen($4.code) + strlen($6.code) + 40 ];
																					strcpy ( $$.code, $2.code );
																					strcat ( $$.code, "JF " );
																					strcat ( $$.code, L1 );
																					strcat ( $$.code, "\n" );
																					strcat ( $$.code, $4.code ); 
																					strcat ( $$.code, "JUMP " );
																					strcat ( $$.code, L2 );
																					strcat ( $$.code, "\n" );
																					strcat ( $$.code, "LABEL " );
																					strcat ( $$.code, L1 );
																					strcat ( $$.code, "\n" ); 
																					strcat ( $$.code, $6.code );
																					strcat ( $$.code, "LABEL " );
																					strcat ( $$.code, L2 );
																					strcat ( $$.code, "\n" ); 								
																				}
							
							
condicional:
								condicional	AND	condicional	{
																$$.code = new char[ strlen($1.code) + strlen($3.code) + 20 ];
																strcpy ( $$.code, $1.code );
																strcat ( $$.code, $3.code );
																strcat ( $$.code, "AND\n" );
															}
							|	condicional	OR	condicional	{
																$$.code = new char[ strlen($1.code) + strlen($3.code) + 20 ];
																strcpy ( $$.code, $1.code );
																strcat ( $$.code, $3.code );
																strcat ( $$.code, "OR\n" );
															}
							|	'('	condicional	')'	{ $$ = $2; }
							|	expresion	op_comparacion	expresion	{
																			if ( $1.tipo != $3.tipo ) {
																				char *msg = new char[100];
																				strcpy ( msg, "Error. No se puede comparar un elemento de tipo ");
																				strcat ( msg, get_str_type($1.tipo).c_str() );
																				strcat ( msg, " con un elemento de tipo ");
																				strcat ( msg, get_str_type($3.tipo).c_str() );
																				my_error ( msg ); 																				
																			}
																			
																			if ( $2.tipo != op_eq && $2.tipo != op_ne && $1.tipo != t_integer && $1.tipo != t_real ) {
																				char *msg = new char[100];
																				strcpy ( msg, "Error. El operador de comparacion ");
																				strcat ( msg, get_str_op_comp($2.tipo).c_str() );
																				strcat ( msg, " no esta definido para expresiones de tipo ");
																				strcat ( msg, get_str_type($1.tipo).c_str() );
																				my_error ( msg );																				
																			}
																			
																			$$.code = new char[ strlen($1.code) + strlen($3.code) + 40 ];
																			strcpy ( $$.code, $1.code );
																			strcat ( $$.code, $3.code );
																			switch ( $2.tipo ) {
																				case op_less: strcat ( $$.code, "LESS\n" );
																						   break;
																				case op_greater:	strcat ( $$.code, "GREATER\n" );
																								break;
																				case op_eq:	strcat ( $$.code, "EQ\n" );
																							break;
																				case op_le:	strcat ( $$.code, "LE\n" );
																							break;
																				case op_ge:	strcat ( $$.code, "GE\n" );
																							break;
																				default:	strcat ( $$.code, "NE\n" );
																					break;
																			}
																		}
							;
							
							

op_comparacion:	
								LESS	{ $$.tipo = op_less; }
							|	GREATER	{ $$.tipo = op_greater;}
							|	EQ	{ $$.tipo = op_eq; }
							|	LE	{ $$.tipo = op_le; }                                                               
							|	GE	{ $$.tipo = op_ge; }
							|	NE	{ $$.tipo = op_ne; }
							;
							

iterativa:
								WHILE	condicional	DO	bloque	{
									
																	
																	
																	char *L1 = get_label();
																	char *L2 = get_label();
																	$$.code = new char[ strlen($2.code) + strlen($4.code) + 80 ];
																	
																	strcpy ( $$.code, "LABEL ");
																	strcat ( $$.code, L1 );
																	strcat ( $$.code, "\n" );
																	strcat ( $$.code, $2.code );
																	strcat ( $$.code, "JF " );
																	strcat ( $$.code, L2 );
																	
																	strcat ( $$.code, "\n");
																	strcat ( $$.code, $4.code );
																	strcat ( $$.code, "JUMP " );
																	strcat ( $$.code, L1 );
																	strcat ( $$.code, "\n" );
																	strcat ( $$.code, "LABEL ");
																	strcat ( $$.code, L2 );
																	strcat ( $$.code, "\n" );
																}
							;
%%


void my_error ( const char *s ) {
	cerr << "At line " << line_num << ": " << s << endl;
	exit(1);
} 

void yyerror ( const char *s ) {	
	cerr << "At line " << line_num << ":  " << s << endl;
	exit(1);
}			

char* to_cad ( int x ) {
	int cdig = (x == 0);
	int tmp = x;
	while ( tmp ) {
		 cdig ++;
		 tmp /= 10;
	}
	
	bool neg = (x<0);
	char *ret = new char[cdig+1 + neg];
	if ( neg )
		 ret[0] = '-';
	for ( int i = cdig-1+neg; i >= neg; i-- ) {
		 ret[i] = abs(x%10) + '0';
		 x /= 10;
	}
	ret[cdig+neg] = '\0';
	return ret;
} 			

char *get_label () {
	char *s_label = to_cad ( label++ );
	char *ret = new char[ 2 + strlen(s_label) ];
	strcpy ( ret, "L" );
	strcat ( ret, s_label );
	return ret;
}
				
string to_string ( char *s ) {
	string ret;
	int l = strlen ( s );
	for ( int i = 0; i < l; i ++ )
		ret += s[i];
	return ret;
}

string get_str_op_comp ( int oper ) {
	switch ( oper ) {
		case op_less: return "<";
		case op_greater: return ">";
		case op_le: return "<=";
		case op_ge: return ">=";
		case op_eq: return "=";
		default:	return "<>";
	}
	
}

string get_str_type ( int t ) {
	 switch ( t ) {
		case t_boolean: return "boolean";
		case t_char: return "char";
		case t_integer: return "integer";
		case t_list: return "list";
		case t_real: return "real";
		default : return "string";
	 }
}
											   
int main () {
	yyparse();
	return 0;
}
