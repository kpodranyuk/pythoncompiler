%{
#include <string.h>
extern int yylex();
extern int yyparse();
extern void yyerror(const char* s);
int root;
%}
%union
{
	int Int;
};
%type <Int> root
%left '+'
%left <Int>OPERAND
%start root
%%
root: OPERAND '+' OPERAND {$$=$1+$3;
root = $$;};
%%