#include <bits/stdc++.h>

using namespace std;

typedef long long ll;

const int maxn = 1005;
const ll mod = 998244353;

struct info {
	int *ival;
	double *rval;
	char *cval;
	string *sval;
	int *bval;

	vector<int> *vint;
	vector<double> *vreal;
	vector<char> *vchar;
	vector<string> *vstr;
	vector<int> *vbool;

	int tipo, subtipo;
};

map <string, info*> sym;
map <string, int> label, pr;

vector<info*> s;
vector < vector<string> > code;

enum { t_real = 1, t_char, t_boolean, t_integer, t_string, t_list  } ;
enum { VAR = 1, DECL_TYPE, CARGA, CARGA_IND, LIT_INT, LIT_REAL, LIT_CHAR, LIT_BOOL, LIT_STRING, PUSH_STR, ASIGNA, LESS, GREATER, EQ, LE, GE, NE,
	   SUM_STR_CHAR, SUM_STR, SUM_INT, SUM_REAL, SUB_INT, SUB_REAL, MUL_INT, MUL_REAL, LEN, NEG_REAL, NEG_INT, LABEL, JF, JUMP, WRITE_ENDL, APPEND,
	   READ, WRITE, WRITE_SPACE, DIV_INT, DIV_REAL, MOD_INT, AND, OR };

int pt;

void my_error ( string msg ) {
	cout << "Error de ejecucion en linea " << pt+1 << " de la forma interna: " << endl;
	cout << msg << endl;
	exit(0);
}


int main()
{

	ifstream in ( "forma_interna.txt", ios::in );

	pr = { {"VAR", VAR}, {"DECL_TYPE", DECL_TYPE}, {"CARGA", CARGA}, {"CARGA_IND", CARGA_IND}, {"LIT_INT", LIT_INT}, {"LIT_REAL", LIT_REAL},
		   {"LIT_CHAR", LIT_CHAR}, {"LIT_BOOL", LIT_BOOL}, {"LIT_STRING", LIT_STRING}, {"PUSH_STR", PUSH_STR}, {"ASIGNA", ASIGNA}, {"LESS", LESS},
		   {"GREATER", GREATER}, {"EQ", EQ}, {"LE", LE}, {"GE", GE}, {"NE", NE}, {"SUM_STR_CHAR", SUM_STR_CHAR}, {"SUM_STR", SUM_STR},
		   {"SUM_INT", SUM_INT}, {"SUM_REAL", SUM_REAL}, {"SUB_INT", SUB_INT}, {"SUB_REAL", SUB_REAL}, {"MUL_INT", MUL_INT}, {"MUL_REAL", MUL_REAL},
		   {"LEN", LEN}, {"NEG_REAL", NEG_REAL}, {"NEG_INT", NEG_INT}, {"LABEL", LABEL}, {"JF", JF}, {"JUMP", JUMP}, {"WRITE_ENDL", WRITE_ENDL},
		   {"APPEND", APPEND}, {"READ", READ}, {"WRITE", WRITE}, {"WRITE_SPACE", WRITE_SPACE}, {"DIV_INT", DIV_INT}, {"DIV_REAL", DIV_REAL},
		   {"MOD_INT", MOD_INT}, {"AND", AND}, {"OR", OR} };

	char *buff = new char[10000];
	while ( in.getline ( buff, 1000 ) ) {
		int len = strlen(buff);

		code.push_back ( vector<string> () );
		string tmp;
		for ( int i = 0; i < len; i ++ ) {
			if ( buff[i] == ' ' ) {
				code.back().push_back( tmp );
				tmp = "";
			} else
				tmp += buff[i];
		}
		code.back().push_back ( tmp );
	}


	int sz_code = code.size();
	for ( int i = 0; i < sz_code; i ++ )
		if ( code[i][0] == "LABEL" ) {
			label[ code[i][1] ] = i;
		}

	vector <string> vars;
	int top = -1;
	for ( pt = 0; pt < sz_code ; pt ++ ) {
		switch ( pr[ code[pt][0]  ] ) {
		case VAR: {
				   	   vars.push_back ( code[pt][1] );
				   	   break;
				  }
		case DECL_TYPE: {
						 	 for (string i: vars ) {
						 		 sym[i] = new info();
						 		 info *tmp = sym[i];
			             		 if ( code[pt][1] == "integer" ) {
			             			 tmp->ival = new int();
			             			 tmp->tipo = t_integer;
			             		 }
			             		 if ( code[pt][1] == "real" ) {
			             			 tmp->rval = new double();
			             			 tmp->tipo = t_real;
			             		 }
			             		 if ( code[pt][1] == "boolean" ) {
			             			 tmp->bval = new int();
			             			 tmp->tipo = t_boolean;
			             		 }
			             		 if ( code[pt][1] == "char" ) {
			             			 tmp->cval = new char();
			             			 tmp->tipo = t_char;
			             		 }
			             		 if ( code[pt][1] == "string" ) {
			             			 tmp->sval = new string();
			             			 tmp->tipo = t_string;
			             		 }
			             		 if ( code[pt][1] == "list" ) {
			             			 tmp->tipo = t_list;
			             			 if ( code[pt][2] == "integer" ) {
			             				 tmp->vint = new vector<int> ();
			             				 tmp->subtipo = t_integer;
			             			 }
			             			 if ( code[pt][2] == "real" ) {
			             				 tmp->vreal = new vector<double> ();
			             				 tmp->subtipo = t_real;
			             			 }
			             			 if ( code[pt][2] == "boolean" ) {
			             				 tmp->vbool = new vector<int> ();
			             				 tmp->subtipo = t_boolean;
			             			 }
			             			 if ( code[pt][2] == "char" ) {
			             				 tmp->vchar = new vector<char> ();
			             				 tmp->subtipo = t_char;
			             			 }
									 if ( code[pt][2] == "string" ) {
										 tmp->vstr = new vector<string> ();
										 tmp->subtipo = t_string;
									 }
			             		 }
			             	 }
						 	 vars.clear();
						 	 break;
						}
		case CARGA: {
						string name = *( s.back()->sval );
						s.back() = sym[ name ];
						break;
					}
		case CARGA_IND: {
							info *tmp = new info();
							int i = *(s[top]->ival);
							if ( s[top-1]->tipo == t_list ) {
								tmp->tipo = s[top-1]->subtipo;
								if ( tmp->tipo == t_integer ) {
									if ( i >= (int)s[top-1]->vint->size() || i < 0 )
										my_error( "Indice fuera de rango" );
									tmp->ival = &( *( s[top-1]->vint ) )[i] ;
								}
								if ( tmp->tipo == t_real ) {
									if ( i >= (int)s[top-1]->vreal->size() || i < 0 )
										my_error( "Indice fuera de rango" );
									tmp->rval = & ( * (s[top-1]->vreal ) )[i];
								}
								if ( tmp->tipo == t_char ) {
									if ( i >= (int)s[top-1]->vchar->size() || i < 0 )
										my_error( "Indice fuera de rango" );
									tmp->cval = &( * (s[top-1]->vchar ) )[i];
								}
								if ( tmp->tipo == t_boolean ) {
									if ( i >= (int)s[top-1]->vbool->size() || i < 0 )
										my_error( "Indice fuera de rango" );
									tmp->bval = &( * (s[top-1]->vbool) )[i];
								}
								if ( tmp->tipo == t_string ) {
									if ( i >= (int)s[top-1]->vstr->size() || i < 0 )
										my_error( "Indice fuera de rango" );
									tmp->sval = &( * (s[top-1]->vstr) )[i] ;
								}
							}
							if ( s[top-1]->tipo == t_string ) {
								tmp->tipo = t_char;
								if ( i >= (int)s[top-1]->sval->size() || i < 0 )
									my_error( "Indice fuera de rango" );
								tmp->cval = new char( (*(s[top-1]->sval))[i] );
							}
							s[top-1] = tmp;
							s.pop_back();
							top--;
							break;
						}
		case LIT_INT: {
							info *tmp = new info();
							int x = atoi ( code[pt][1].c_str() );
							tmp->ival = new int( x );
							tmp->tipo = t_integer;
							s.push_back ( tmp );
							top ++;
							break;
					  }
		case LIT_BOOL: {
							info *tmp = new info();
							int x = ( code[pt][1] == "false" )? 0 : 1;
							tmp->bval = new int( x );
							tmp->tipo = t_boolean;
							s.push_back ( tmp );
							top ++;
							break;
					   }
		case LIT_REAL: {
							info *tmp = new info();
							double x = atof ( code[pt][1].c_str() );
							tmp->rval = new double( x );
							tmp->tipo = t_real;
							s.push_back ( tmp );
							top ++;
							break;
					   }
		case LIT_CHAR: {
							info *tmp = new info();
							char x = code[pt][1][0];
							tmp->cval = new char ( x );
							tmp->tipo = t_char;
							s.push_back ( tmp );
							top ++;
							break;
					   }
		case LIT_STRING: {
							info *tmp = new info();
							string x = code[pt][1];
							for ( int i = 2; i < (int)code[pt].size(); i ++ )
								x = x + ' ' + code[pt][i];

							tmp->sval = new string ( x );
							tmp->tipo = t_string;
							s.push_back ( tmp );
							top ++;
							break;
					    }
		case PUSH_STR: {
							info *tmp = new info();
							string x = code[pt][1];
							tmp->sval = new string ( x );
							s.push_back ( tmp );
							top ++;
							break;
					   }
		case ASIGNA: {
							info *l = s[top-1], *r = s[top];
							if ( l->tipo == t_integer )
								*(l->ival) = *(r->ival);
							if ( l->tipo == t_real )
								*(l->rval) = *(r->rval);
							if ( l->tipo == t_char )
								*(l->cval) = *(r->cval);
							if ( l->tipo == t_boolean )
								*(l->bval) = *(r->bval);
							if ( l->tipo == t_string )
								*(l->sval) = *(r->sval);
							if ( l->tipo == t_list ) {
								if ( l->subtipo == t_integer )
									*(l->vint) = *(r->vint);
								if ( l->subtipo == t_real )
									*(l->vreal) = *(r->vreal);
								if ( l->subtipo == t_char )
									*(l->vchar) = *(r->vchar);
								if ( l->subtipo == t_boolean )
									*(l->vbool) = *(r->vbool);
								if ( l->subtipo == t_string )
									*(l->vstr) = *(r->vstr);
							}

							s.pop_back();
							s.pop_back();
							top -= 2;
							break;
					}
		case LESS: {
						info *l = s[top-1], *r = s[top];
						info *tmp = new info();
						tmp->tipo = t_boolean;
						if ( l->tipo == t_integer )
							tmp->bval = new int ( ( *(l->ival) < *(r->ival) ) );
						else
							tmp->bval = new int ( ( *(l->rval) < *(r->rval) ) );
						s[top-1] = tmp;
						s.pop_back();
						top --;
						break;
				   }
		case GREATER: {
							info *l = s[top-1], *r = s[top];
							info *tmp = new info();
							tmp->tipo = t_boolean;
							if ( l->tipo == t_integer )
								tmp->bval = new int ( ( *(l->ival) > *(r->ival) ) );
							else
								tmp->bval = new int ( ( *(l->rval) > *(r->rval) ) );
							s[top-1] = tmp;
							s.pop_back();
							top --;
							break;
					  }
		case LE: {
					info *l = s[top-1], *r = s[top];
					info *tmp = new info();
					tmp->tipo = t_boolean;
					if ( l->tipo == t_integer )
						tmp->bval = new int ( ( *(l->ival) <= *(r->ival) ) );
					else
						tmp->bval = new int ( ( *(l->rval) <= *(r->rval) ) );
					s[top-1] = tmp;
					s.pop_back();
					top --;
					break;
				 }
		case GE: {
					info *l = s[top-1], *r = s[top];
					info *tmp = new info();
					tmp->tipo = t_boolean;
					if ( l->tipo == t_integer )
						tmp->bval = new int ( ( *(l->ival) >= *(r->ival) ) );
					else
						tmp->bval = new int ( ( *(l->rval) >= *(r->rval) ) );
					s[top-1] = tmp;
					s.pop_back();
					top --;
					break;
				 }
		case EQ: {
					info *l = s[top-1], *r = s[top];
					info *tmp = new info();
					tmp->tipo = t_boolean;
					if ( l->tipo == t_integer )
						tmp->bval = new int ( ( *(l->ival) == *(r->ival) ) );
					if ( l->tipo == t_real )
						tmp->bval = new int ( ( *(l->rval) == *(r->rval) ) );
					if ( l->tipo == t_char )
						tmp->bval = new int ( ( *(l->cval) == *(r->cval) ) );
					if ( l->tipo == t_boolean )
						tmp->bval = new int ( ( *(l->bval) == *(r->bval) ) );
					if ( l->tipo == t_string )
						tmp->bval = new int ( ( *(l->sval) == *(r->sval) ) );
					if ( l->tipo == t_list ) {
						if ( l->subtipo == t_integer )
							tmp->bval = new int ( ( *(l->vint) == *(r->vint) ) );
						if ( l->subtipo == t_real )
							tmp->bval = new int ( ( *(l->vreal) == *(r->vreal) ) );
						if ( l->subtipo == t_char )
							tmp->bval = new int ( ( *(l->vchar) == *(r->vchar) ) );
						if ( l->subtipo == t_boolean )
							tmp->bval = new int ( ( *(l->vbool) == *(r->vbool) ) );
						if ( l->subtipo == t_string )
							tmp->bval = new int ( ( *(l->vstr) == *(r->vstr) ) );
					}
					s[top-1] = tmp;
					s.pop_back();
					top --;
					break;
				 }
		case NE: {
					info *l = s[top-1], *r = s[top];
					info *tmp = new info();
					tmp->tipo = t_boolean;
					if ( l->tipo == t_integer )
						tmp->bval = new int ( ( *(l->ival) != *(r->ival) ) );
					if ( l->tipo == t_real )
						tmp->bval = new int ( ( *(l->rval) != *(r->rval) ) );
					if ( l->tipo == t_char )
						tmp->bval = new int ( ( *(l->cval) != *(r->cval) ) );
					if ( l->tipo == t_boolean )
						tmp->bval = new int ( ( *(l->bval) != *(r->bval) ) );
					if ( l->tipo == t_string )
						tmp->bval = new int ( ( *(l->sval) != *(r->sval) ) );
					if ( l->tipo == t_list ) {
						if ( l->subtipo == t_integer )
							tmp->bval = new int ( ( *(l->vint) != *(r->vint) ) );
						if ( l->subtipo == t_real )
							tmp->bval = new int ( ( *(l->vreal) != *(r->vreal) ) );
						if ( l->subtipo == t_char )
							tmp->bval = new int ( ( *(l->vchar) != *(r->vchar) ) );
						if ( l->subtipo == t_boolean )
							tmp->bval = new int ( ( *(l->vbool) != *(r->vbool) ) );
						if ( l->subtipo == t_string )
							tmp->bval = new int ( ( *(l->vstr) != *(r->vstr) ) );
					}
					s[top-1] = tmp;
					s.pop_back();
					top --;
					break;
		  		 }
		case SUM_STR_CHAR: {
								info *l = s[top-1], *r = s[top];
								info *tmp = new info();
								tmp->tipo = t_string;
								tmp->sval = new string ( *(l->sval) );
								*(tmp->sval) += *(r->cval);
								s[top-1] = tmp;
								s.pop_back();
								top --;
								break;
						   }
		case SUM_STR: {
							info *l = s[top-1], *r = s[top];
							info *tmp = new info();
							tmp->tipo = t_string;
							tmp->sval = new string( *(l->sval) );
							*(tmp->sval) += *(r->sval);
							s[top-1] = tmp;
							s.pop_back();
							top --;
							break;
					  }

		case SUM_INT: {
							info *l = s[top-1], *r = s[top];
							info *tmp = new info();
							tmp->tipo = t_integer;
							tmp->ival = new int ( *(l->ival) + *(r->ival) );
							s[top-1] = tmp;
							s.pop_back();
							top --;
							break;
					  }
		case SUM_REAL: {
							info *l = s[top-1], *r = s[top];
							info *tmp = new info();
							tmp->tipo = t_real;
							tmp->rval = new double ( *(l->rval) + *(r->rval) );
							s[top-1] = tmp;
							s.pop_back();
							top --;
							break;
		   			   }
		case SUB_INT: {
							info *l = s[top-1], *r = s[top];
							info *tmp = new info();
							tmp->tipo = t_integer;
							tmp->ival = new int ( *(l->ival) - *(r->ival) );
							s[top-1] = tmp;
							s.pop_back();
							top --;
							break;
					  }
		case SUB_REAL: {
							info *l = s[top-1], *r = s[top];
							info *tmp = new info();
							tmp->tipo = t_real;
							tmp->rval = new double ( *(l->rval) - *(r->rval) );
							s[top-1] = tmp;
							s.pop_back();
							top --;
							break;
					   }
		case MUL_INT: {
							info *l = s[top-1], *r = s[top];
							info *tmp = new info();
							tmp->tipo = t_integer;
							tmp->ival = new int ( *(l->ival) * *(r->ival) );
							s[top-1] = tmp;
							s.pop_back();
							top --;
							break;
					  }
		case MUL_REAL: {
							info *l = s[top-1], *r = s[top];
							info *tmp = new info();
							tmp->tipo = t_real;
							tmp->rval = new double ( *(l->rval) * *(r->rval) );
							s[top-1] = tmp;
							s.pop_back();
							top --;
							break;
					   }
		case DIV_INT: {
							info *l = s[top-1], *r = s[top];
							info *tmp = new info();
							tmp->tipo = t_integer;
							tmp->ival = new int ( *(l->ival) / *(r->ival) );
							s[top-1] = tmp;
							s.pop_back();
							top --;
							break;
					  }
		case DIV_REAL: {
							info *l = s[top-1], *r = s[top];
							info *tmp = new info();
							tmp->tipo = t_real;
							tmp->rval = new double ( *(l->rval) / *(r->rval) );
							s[top-1] = tmp;
							s.pop_back();
							top --;
							break;
					   }
		case MOD_INT: {
							info *l = s[top-1], *r = s[top];
							info *tmp = new info();
							tmp->tipo = t_integer;
							tmp->ival = new int ( *(l->ival) % *(r->ival) );
							s[top-1] = tmp;
							s.pop_back();
							top --;
							break;
					  }
		case NEG_INT: {
							info *u = s[top];
							info *tmp = new info();
							tmp->tipo = t_integer;
							tmp->ival = new int ( *(u->ival) * -1 );
							s[top] = tmp;
							break;
					  }
		case NEG_REAL: {
							info *u = s[top];
							info *tmp = new info();
							tmp->tipo = t_real;
							tmp->rval = new double ( *(u->rval) * -1 );
							s[top] = tmp;
							break;
					   }
		case LEN: {
						info *u = s[top];
						info *tmp = new info();
						tmp->tipo = t_integer;
						tmp->ival = new int();
						if ( u->tipo == t_string )
							*(tmp->ival) = u->sval->size();
						if ( u->tipo == t_list ) {
							if ( u->subtipo == t_integer )
								*(tmp->ival) = u->vint->size();
							if ( u->subtipo == t_real )
								*(tmp->ival) = u->vreal->size();
							if ( u->subtipo == t_char )
								*(tmp->ival) = u->vchar->size();
							if ( u->subtipo == t_boolean )
								*(tmp->ival) = u->vbool->size();
							if ( u->subtipo == t_string )
								*(tmp->ival) = u->vstr->size();
						}
						s[top] = tmp;
						break;
				  }
		case LABEL: {
						break;
					}
		case JF: {
					info *u = s[top];
					if ( *(u->bval) == false )
						pt = label[ code[pt][1] ];
					s.pop_back();
					top--;
					break;
				 }
		case JUMP: {
						pt = label[ code[pt][1] ];
						break;
				   }
		case WRITE_ENDL: {
							cout << endl;
							break;
						 }
		case WRITE_SPACE: {
								cout << ' ';
								break;
						  }
		case WRITE: {
						info *u = s[top];
						if ( u->tipo == t_integer )
							cout << *(u->ival);
						if ( u->tipo == t_real )
							cout << *(u->rval);
						if ( u->tipo == t_char )
							cout << *(u->cval);
						if ( u->tipo == t_boolean )
							cout << *(u->bval);
						if ( u->tipo == t_string )
							cout << *(u->sval);
						s.pop_back();
						top--;
						break;
					}
		case APPEND: {
						info *l = s[top-1], *r = s[top];
						if ( r->tipo == t_integer )
							l->vint->push_back ( *(r->ival) );
						if ( r->tipo == t_real )
							l->vreal->push_back ( *(r->rval) );
						if ( r->tipo == t_char )
							l->vchar->push_back ( *(r->cval) );
						if ( r->tipo == t_boolean )
							l->vbool->push_back ( *(r->bval) );
						if ( r->tipo == t_string )
							l->vstr->push_back ( *(r->sval) );

						s.pop_back();
						s.pop_back();
						top -= 2;
						break;
					 }
		case READ: {
						info *u = s[top];
						if ( u->tipo == t_integer ) {
							int x; cin >> x;
							*(u->ival) = x;
						}
						if ( u->tipo == t_real ) {
							double x; cin >> x;
							*(u->rval) = x;
						}
						if ( u->tipo == t_char ) {
							char x; cin >> x;
							*(u->cval) = x;
						}
						if ( u->tipo == t_string ) {
							string x; cin >> x;
							*(u->sval) = x;
						}
						if ( u->tipo == t_boolean ) {
							int x; cin >> x;
							*(u->bval) = x;
						}
						s.pop_back();
						top --;
						break;
				   }
		case AND: {
						info *l = s[top-1], *r = s[top];
						info *tmp = new info();
						tmp->tipo = t_boolean;
						tmp->bval = new int ( ( *(l->bval) && *(r->bval) ) );
						s[top-1] = tmp;
						s.pop_back();
						top --;
						break;
				  }
		case OR: {
						info *l = s[top-1], *r = s[top];
						info *tmp = new info();
						tmp->tipo = t_boolean;
						tmp->bval = new int ( ( *(l->bval) || *(r->bval) ) );
						s[top-1] = tmp;
						s.pop_back();
						top --;
						break;

				 }
		default: assert(false);

		}
	}

    return 0;
}
