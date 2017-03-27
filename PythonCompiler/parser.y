%{
#include <string.h>
#include <math.h>
#include <process.h>
#include <stdio.h>	
extern int yylex();
extern int yyparse();
extern void yyerror(const char* s);
extern FILE* logFile;
int root;
%}
%union
{
	int Int;
	char* StringVal;
};
%type <Int> fullroot
%type <Int> if_stmt
%type <Int> for_stmt
%type <Int> while_stmt
%type <Int> var_val
%type <Int> stmt
%type <Int> stmt_list
%type <Int> expr
%nonassoc INDENT DEDENT	NEWLINE// Начало и конец блока кода
%token ':'		// Двоеточие
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
%token '['		// Открывающая квадратная скобка
%token ']'		// Закрывающая квадратная скобка
%token AND OR NOT								// Оператор и или не
%token IN 										// Оператор in
%left '='										// Оператор присвоения
%left EQUAL NOT_EQUAL							// Оператор равенства и неравенства
%left LESS_OR_EQUAL '<' GREATER_OR_EQUAL '>'	// Оператор меньше либо равно, больше либо равно, меньше, больше
%left '+'	'-'									// Оператор сложения и вычитания
%left MUL DIV MOD INT							// Оператор умножения, деления, остатка от деления, целой части от деления (возможна замена одним символом)
%left POW										// Оператор возведения в степень
%nonassoc '('		// Открывающая круглая скобка
%nonassoc ')'		// Закрывающая круглая скобка
%token <StringVal>OPERAND	// Операнд
%left <StringVal>STRING	// Строка
%left <Int>DIGIT			// Число
%start fullroot		// Стартовый символ		//fullroot NEWLINE INDENT {printf("BISON:\tfound NEWLINE INDENT fullroot\n"); fprintf(logFile,"BISON:\tfound NEWLINE INDENT fullroot\n");}//{$$=$2;}
%%
fullroot: fullroot NEWLINE fullroot{printf("BISON:\tconcatenated 2 strings\n"); fprintf(logFile,"BISON:\tconcatenated 2 strings\n");}//| fullroot NEWLINE {printf("BISON:\tconcatenated NEWLINE\n"); fprintf(logFile,"BISON:\tconcatenated NEWLINE\n");}
| if_stmt {printf("BISON:\tfound IF_STMT: INDENT\t\n"); fprintf(logFile,"BISON:\tfound IF_STMT INDENT:\t\n");}//{$$=$1; root = $$;}
| expr
;
if_stmt: IF expr ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound IF_STMT:\t\n"); fprintf(logFile,"BISON:\tfound IF_STMT:\t\n");}
| IF expr ':' NEWLINE INDENT stmt_list DEDENT NEWLINE ELSE NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound IF_STMT:\t\n"); fprintf(logFile,"BISON:\tfound IF_STMT:\t\n");}
| ELIF expr ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound IF_STMT:\t\n"); fprintf(logFile,"BISON:\tfound IF_STMT:\t\n");}
| ELIF expr ':' NEWLINE INDENT stmt_list DEDENT NEWLINE ELSE NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound IF_STMT:\t\n"); fprintf(logFile,"BISON:\tfound IF_STMT:\t\n");}
;

for_stmt: FOR OPERAND IN OPERAND ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound FOR_STMT:\t\n"); fprintf(logFile,"BISON:\tfound FOR_STMT:\t\n");}
| FOR OPERAND IN OPERAND ':' NEWLINE INDENT stmt_list DEDENT NEWLINE ELSE NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound FOR_STMT:\t\n"); fprintf(logFile,"BISON:\tfound FOR_STMT:\t\n");}
;

while_stmt: WHILE expr ':' NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound WHILE_STMT:\t\n"); fprintf(logFile,"BISON:\tfound WHILE_STMT:\t\n");}
|  WHILE expr ':' NEWLINE INDENT stmt_list DEDENT NEWLINE ELSE NEWLINE INDENT stmt_list DEDENT {printf("BISON:\tfound WHILE_STMT:\t\n"); fprintf(logFile,"BISON:\tfound WHILE_STMT:\t\n");}
;

stmt_list: stmt {printf("BISON:\tfound stmt_list:\t\n"); fprintf(logFile,"BISON:\tfound stmt_list:\t\n");}
| stmt_list stmt {printf("BISON:\tfound stmt_list:\t\n"); fprintf(logFile,"BISON:\tfound stmt_list:\t\n");}
;

stmt: expr NEWLINE {printf("BISON:\tfound expr stmt:\t\n"); fprintf(logFile,"BISON:\tfound expr stmt:\t\n");}
| if_stmt NEWLINE {printf("BISON:\tfound if_stmt:\t\n"); fprintf(logFile,"BISON:\tfound if_stmt:\t\n");}
| for_stmt NEWLINE {printf("BISON:\tfound for_stmt:\t\n"); fprintf(logFile,"BISON:\tfound for_stmt:\t\n");}
| while_stmt NEWLINE {printf("BISON:\tfound while_stmt:\t\n"); fprintf(logFile,"BISON:\tfound while_stmt:\t\n");}
;

var_val: TRUE	{printf("BISON:\tfound var_val: TRUE\n"); fprintf(logFile,"BISON:\tfound var_val: TRUE\n");}
| FALSE		{printf("BISON:\tfound var_val: FALSE\n"); fprintf(logFile,"BISON:\tfound var_val: FALSE\n");}
| STRING	{printf("BISON:\tfound var_val: STRING\n"); fprintf(logFile,"BISON:\tfound var_val: STRING\n");}
| DIGIT		{printf("BISON:\tfound var_val: DIGIT\n"); fprintf(logFile,"BISON:\tfound var_val: DIGIT\n");}
;
expr: expr NOT_EQUAL expr			{printf("BISON:\tfound expr: NOT_EQUAL\n"); fprintf(logFile,"BISON:\tfound expr: NOT_EQUAL\n");}
| expr EQUAL expr				{printf("BISON:\tfound expr: EQUAL\n"); fprintf(logFile,"BISON:\tfound expr: EQUAL\n");}
| expr '>' expr					{printf("BISON:\tfound expr: >\n"); fprintf(logFile,"BISON:\tfound expr: >\n");}
| expr GREATER_OR_EQUAL expr	{printf("BISON:\tfound expr: GREATER_OR_EQUAL\n"); fprintf(logFile,"BISON:\tfound expr: GREATER_OR_EQUAL\n");}
| expr '<' expr					{printf("BISON:\tfound expr: <\n"); fprintf(logFile,"BISON:\tfound expr: <\n");}
| expr LESS_OR_EQUAL expr		{printf("BISON:\tfound expr: LESS_OR_EQUAL\n"); fprintf(logFile,"BISON:\tfound expr: LESS_OR_EQUAL\n");}
| expr '-' expr					{printf("BISON:\tfound expr: -\n"); fprintf(logFile,"BISON:\tfound expr: -\n");}
| expr '+' expr					{printf("BISON:\tfound expr: +\n"); fprintf(logFile,"BISON:\tfound expr: +\n");}
| expr INT expr					{printf("BISON:\tfound expr: INT\n"); fprintf(logFile,"BISON:\tfound expr: INT\n");}
| expr MOD expr					{printf("BISON:\tfound expr: MOD\n"); fprintf(logFile,"BISON:\tfound expr: MOD\n");}
| expr DIV expr					{printf("BISON:\tfound expr: DIV\n"); fprintf(logFile,"BISON:\tfound expr: DIV\n");}
| expr MUL expr					{printf("BISON:\tfound expr: MUL\n"); fprintf(logFile,"BISON:\tfound expr: MUL\n");}
| expr POW expr					{printf("BISON:\tfound expr: POW\n"); fprintf(logFile,"BISON:\tfound expr: POW\n");}
| expr '=' expr					{printf("BISON:\tfound expr: =\n"); fprintf(logFile,"BISON:\tfound expr: =\n");}
| var_val					{printf("BISON:\tfound expr: var_val\n"); fprintf(logFile,"BISON:\tfound expr: var_val\n");}
| OPERAND					{printf("BISON:\tfound expr: OPERAND\n"); fprintf(logFile,"BISON:\tfound expr: OPERAND\n");}
| DEL OPERAND				{printf("BISON:\tfound expr: DEL\n"); fprintf(logFile,"BISON:\tfound expr: DEL\n");}
;
%%