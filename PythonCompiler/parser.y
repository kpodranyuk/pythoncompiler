%{
#include "stdafx.h"
#include <string.h>
extern int yylex();
extern int yyparse();
%}
%start root
%%
root: ')'
%%