%{
#include <string.h>
extern int yylex();
extern int yyparse();
extern void yyerror(const char* s);
%}
%start root
%%
root: ')'
%%