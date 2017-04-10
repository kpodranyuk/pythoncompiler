%{
#include "functions.h"
#include <string.h>
#include <math.h>
#include <process.h>
#include <stdio.h>	
extern int yylex();
extern int yyparse();
extern void yyerror(const char* s);
extern FILE* logFileB;
int root;
%}
%union
{
	int Int;
	char* StringVal;

	struct StmtListInfo * stmtlist;
	struct StmtInfo * stmtinfo;
	struct IfStmtInfo * ifstmt;
	struct ForStmtInfo * forstmt;
	struct WhileStmtInfo * whilestmt;
};
%type <stmtlist> fullroot
%type <ifstmt> if_stmt
%type <Int> elif_list
%type <forstmt> for_stmt
%type <whilestmt> while_stmt
%type <Int> var_val
%type <stmtinfo> stmt
%type <stmtlist> stmt_list
%type <Int> expr
%type <Int> param_list
%type <Int> param_listE
%type <Int> func_def
%type <Int> func_call
%type <Int> line_sep
%type <Int> def_param_list
%type <Int> def_param_listE

%nonassoc INDENT NEWLINE// Начало и конец блока кода
%left DEDENT// Начало и конец блока кода
%token ':'		// Двоеточие
%token EOF_TOKEN		// Конец файла
%token TRUE	FALSE	// Оператор логической истины
%token WHILE	// Оператор цикла пока
%token FOR		// Оператор цикла для
%token CONTINUE	// Оператор продолжения цикла
%token BREAK	// Оператор прерывания
%token IF		// Оператор условия if
%token ELIF		// Оператор условия elif
%token ELSE		// Оператор условия else 
%token RETURN	// Оператор возврата значения
%token DEF		// Оператор объявления функции
%token DEL		// Оператор удаления переменной
%token ','		// Запятая
%nonassoc '.'		// Точка
%token IN 										// Оператор in
%right '='										// Оператор присвоения
%left AND OR NOT								// Оператор и или не
%left EQUAL NOT_EQUAL							// Оператор равенства и неравенства
%left LESS_OR_EQUAL '<' GREATER_OR_EQUAL '>'	// Оператор меньше либо равно, больше либо равно, меньше, больше
%left '+'	'-'									// Оператор сложения и вычитания
%left MUL DIV MOD INT							// Оператор умножения, деления, остатка от деления, целой части от деления (возможна замена одним символом)
%left POW										// Оператор возведения в степень
%nonassoc '['		// Открывающая квадратная скобка
%nonassoc ']'		// Закрывающая квадратная скобка
%nonassoc '('		// Открывающая круглая скобка
%nonassoc ')'		// Закрывающая круглая скобка
%token <StringVal>OPERAND	// Операнд
%left <StringVal>STRING	// Строка
%left <Int>NUMBER			// Число
%start fullroot		// Стартовый символ		//fullroot NEWLINE INDENT {printf("BISON:\tfound NEWLINE INDENT fullroot\n"); fprintf(logFileB,"BISON:\tfound NEWLINE INDENT fullroot\n");}//{$$=$2;}
%%
fullroot: stmt_list {printf("BISON:\tconcatenated 2 strings\n"); fprintf(logFileB,"BISON:\tconcatenated 2 strings\n");}//| fullroot NEWLINE {printf("BISON:\tconcatenated NEWLINE\n"); fprintf(logFileB,"BISON:\tconcatenated NEWLINE\n");}
;

if_stmt: IF expr ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound IF_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound IF_STMT:\t\n");}
| IF expr ':' NEWLINE INDENT stmt_list DEDENT ELSE ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound IF_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound IF_STMT:\t\n");}
| IF expr ':' NEWLINE INDENT stmt_list DEDENT elif_list {printf("BISON:\tfound IF_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound IF_STMT:\t\n");}
| IF expr ':' NEWLINE INDENT stmt_list DEDENT elif_list ELSE ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound IF_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound IF_STMT:\t\n");}
;

elif_list: ELIF expr ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound ELIF_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound ELIF_STMT:\t\n");}
| elif_list ELIF expr ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound ELIF_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound ELIF_STMT:\t\n");}
;

for_stmt: FOR OPERAND IN expr ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound FOR_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound FOR_STMT:\t\n");}
| FOR OPERAND IN expr ':' NEWLINE INDENT stmt_list DEDENT ELSE ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound FOR_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound FOR_STMT:\t\n");}
;

while_stmt: WHILE expr ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound WHILE_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound WHILE_STMT:\t\n");}
|  WHILE expr ':' NEWLINE INDENT stmt_list DEDENT ELSE ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound WHILE_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound WHILE_STMT:\t\n");}
;

stmt_list: stmt_list line_sep {$$ = createStatementList(NULL, $1); printf("BISON:\tconcatenated 2 strings\n"); fprintf(logFileB,"BISON:\tconcatenated 2 strings\n");}//| fullroot NEWLINE {printf("BISON:\tconcatenated NEWLINE\n"); fprintf(logFileB,"BISON:\tconcatenated NEWLINE\n");}
| stmt {$$ = createStatementList($1, NULL); printf("BISON:\tfound stmt_list:\t\n"); fprintf(logFileB,"BISON:\tfound stmt_list:\t\n");}
| stmt_list stmt {$$ = createStatementList($2, $1); printf("BISON:\tfound stmt_list:\t\n"); fprintf(logFileB,"BISON:\tfound stmt_list:\t\n");}
;

stmt: expr line_sep {printf("BISON:\tfound stmt stmt:\t\n"); fprintf(logFileB,"BISON:\tfound expr_stmt:\t\n");}
| CONTINUE line_sep {printf("BISON:\tfound stmt continue:\t\n"); fprintf(logFileB,"BISON:\tfound stmt_continue:\t\n");}
| BREAK line_sep {printf("BISON:\tfound stmt break:\t\n"); fprintf(logFileB,"BISON:\tfound stmt_break:\t\n");}
| RETURN expr line_sep {printf("BISON:\tfound stmt return:\t\n"); fprintf(logFileB,"BISON:\tfound stmt_return:\t\n");}
| DEL OPERAND line_sep {printf("BISON:\tfound expr: DEL_stmt\n"); fprintf(logFileB,"BISON:\tfound expr: DEL_stmt\n");}
| if_stmt {$$ = createFromIfStatement(__IF, $1); printf("BISON:\tfound if_stmt:\t\n"); fprintf(logFileB,"BISON:\tfound if_stmt:\t\n");}
| func_def {printf("BISON:\tfound func_def:\t\n"); fprintf(logFileB,"BISON:\tfound func_def:\t\n");}
| for_stmt {$$ = createFromForStatement(__FOR, $1) ;printf("BISON:\tfound for_stmt:\t\n"); fprintf(logFileB,"BISON:\tfound for_stmt:\t\n");}
| while_stmt {$$ = createFromWhileStatement(__WHILE, $1) ; printf("BISON:\tfound while_stmt:\t\n"); fprintf(logFileB,"BISON:\tfound while_stmt:\t\n");}
;

var_val: TRUE	{printf("BISON:\tfound var_val: TRUE\n"); fprintf(logFileB,"BISON:\tfound var_val: TRUE\n");}
| FALSE		{printf("BISON:\tfound var_val: FALSE\n"); fprintf(logFileB,"BISON:\tfound var_val: FALSE\n");}
| STRING	{printf("BISON:\tfound var_val: STRING \t %s\n", $1); fprintf(logFileB,"BISON:\tfound var_val: STRING \t %s\n", $1);}
| NUMBER		{printf("BISON:\tfound var_val: NUMBER\n"); fprintf(logFileB,"BISON:\tfound var_val: NUMBER\n");}
;
expr: expr OR expr				{printf("BISON:\tfound expr: OR\n"); fprintf(logFileB,"BISON:\tfound expr: OR\n");}
| expr AND expr					{printf("BISON:\tfound expr: AND\n"); fprintf(logFileB,"BISON:\tfound expr: AND\n");}
| NOT expr						{printf("BISON:\tfound expr: NOT\n"); fprintf(logFileB,"BISON:\tfound expr: NOT\n");}
| expr NOT_EQUAL expr			{printf("BISON:\tfound expr: NOT_EQUAL\n"); fprintf(logFileB,"BISON:\tfound expr: NOT_EQUAL\n");}
| expr EQUAL expr				{printf("BISON:\tfound expr: EQUAL\n"); fprintf(logFileB,"BISON:\tfound expr: EQUAL\n");}
| expr '>' expr					{printf("BISON:\tfound expr: >\n"); fprintf(logFileB,"BISON:\tfound expr: >\n");}
| expr GREATER_OR_EQUAL expr	{printf("BISON:\tfound expr: GREATER_OR_EQUAL\n"); fprintf(logFileB,"BISON:\tfound expr: GREATER_OR_EQUAL\n");}
| expr '<' expr					{printf("BISON:\tfound expr: <\n"); fprintf(logFileB,"BISON:\tfound expr: <\n");}
| expr LESS_OR_EQUAL expr		{printf("BISON:\tfound expr: LESS_OR_EQUAL\n"); fprintf(logFileB,"BISON:\tfound expr: LESS_OR_EQUAL\n");}
| expr '-' expr					{printf("BISON:\tfound expr: -\n"); fprintf(logFileB,"BISON:\tfound expr: -\n");}
| expr '+' expr					{printf("BISON:\tfound expr: +\n"); fprintf(logFileB,"BISON:\tfound expr: +\n");}
| expr INT expr					{printf("BISON:\tfound expr: INT\n"); fprintf(logFileB,"BISON:\tfound expr: INT\n");}
| expr MOD expr					{printf("BISON:\tfound expr: MOD\n"); fprintf(logFileB,"BISON:\tfound expr: MOD\n");}
| expr DIV expr					{printf("BISON:\tfound expr: DIV\n"); fprintf(logFileB,"BISON:\tfound expr: DIV\n");}
| expr MUL expr					{printf("BISON:\tfound expr: MUL\n"); fprintf(logFileB,"BISON:\tfound expr: MUL\n");}
| expr POW expr					{printf("BISON:\tfound expr: POW\n"); fprintf(logFileB,"BISON:\tfound expr: POW\n");}
| '(' expr ')'					{printf("BISON:\tfound expr: BRACKETS\n"); fprintf(logFileB,"BISON:\tfound expr: BRACKETS\n");}
| expr '=' expr					{printf("BISON:\tfound expr: =\n"); fprintf(logFileB,"BISON:\tfound expr: =\n");}
| expr '[' expr ']'				{printf("BISON:\tfound expr: index\n"); fprintf(logFileB,"BISON:\tfound expr: index\n");}
| '[' param_list ']'			{printf("BISON:\tfound expr: initialize\n"); fprintf(logFileB,"BISON:\tfound expr: initialize\n");}
| expr '.' OPERAND '(' expr ')'  {printf("BISON:\tfound expr: actMas\n"); fprintf(logFileB,"BISON:\tfound expr: actMas\n");}
| var_val					{printf("BISON:\tfound expr: var_val\n"); fprintf(logFileB,"BISON:\tfound expr: var_val\n");}
| OPERAND 					{printf("BISON:\tfound expr: OPERAND\t%s\n",$1); fprintf(logFileB,"BISON:\tfound expr: OPERAND\t%s\n",$1);}
| func_call {printf("BISON:\tfound expr: FUNC_CALL\t%s\n",$1); fprintf(logFileB,"BISON:\tfound expr: FUNC_CALL\t%s\n",$1);}
;
param_list:	/*empty*/	{printf("BISON:\tfound param_list: EMPTY\n"); fprintf(logFileB,"BISON:\tfound param_list: EMPTY\n");}
| param_listE	{printf("BISON:\tfound param_list: param_listE\n"); fprintf(logFileB,"BISON:\tfound param_list: param_listE\n");}
;
param_listE: expr	{printf("BISON:\tfound param_listE: expr\n"); fprintf(logFileB,"BISON:\tfound param_listE: expr\n");}
| param_listE ',' expr	{printf("BISON:\tfound param_listE: ,\n"); fprintf(logFileB,"BISON:\tfound param_listE: ,\n");}
;

def_param_list:	/*empty*/	{printf("BISON:\tfound def_param_list: EMPTY\n"); fprintf(logFileB,"BISON:\tfound def_param_list: EMPTY\n");}
| def_param_listE	{printf("BISON:\tfound def_param_list: def_param_listE\n"); fprintf(logFileB,"BISON:\tfound def_param_list: def_param_listE\n");}
;
def_param_listE: OPERAND	{printf("BISON:\tfound def_param_listE: OPERAND\n"); fprintf(logFileB,"BISON:\tfound def_param_listE: expr\n");}
| def_param_listE ',' OPERAND	{printf("BISON:\tfound def_param_listE: ,\n"); fprintf(logFileB,"BISON:\tfound def_param_listE: ,\n");}
;

func_def: DEF OPERAND '(' def_param_list ')' ':' NEWLINE INDENT stmt_list DEDENT	{printf("BISON:\tfound func_def: %s\n",$2); fprintf(logFileB,"BISON:\tfound func_def: %s\n",$2);}
;
func_call: OPERAND '(' param_list ')' {printf("BISON:\tfound func_call: FUNC_CALL\t%s\n",$1); fprintf(logFileB,"BISON:\tfound func_call: FUNC_CALL\t%s\n",$1);}
;
line_sep:NEWLINE	{printf("BISON:\tfound line_sep\n"); fprintf(logFileB,"BISON:\tfound line_sep\n");}
| EOF_TOKEN	{printf("BISON:\tfound line_sep\n"); fprintf(logFileB,"BISON:\tfound line_sep\n");}
// NEWLINE EOF_TOKEN	%prec POW{printf("BISON:\tfound line_sep\n"); fprintf(logFileB,"BISON:\tfound line_sep\n");}
;
%%