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
%type <Int> block
%nonassoc INDENT DEDENT	// Начало и конец блока кода
%left NEWLINE		// Перенос строки
%token ':'		// Двоеточие


%token 	// Оператор неравенства
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
%token '='										// Оператор присвоения
%token EQUAL NOT_EQUAL							// Оператор равенства и неравенства
%token LESS_OR_EQUAL '<' GREATER_OR_EQUAL '>'	// Оператор меньше либо равно, больше либо равно, меньше, больше
%left '+'	'-'									// Оператор сложения и вычитания
%left MUL DIV MOD INT							// Оператор умножения, деления, остатка от деления, целой части от деления (возможна замена одним символом)
%left POW										// Оператор возведения в степень
%nonassoc '('		// Открывающая круглая скобка
%nonassoc ')'		// Закрывающая круглая скобка
%token <StringVal>OPERAND	// Операнд
%token <StringVal>STRING	// Строка
%left <Int>DIGIT			// Число
%start block		// Стартовый символ		//fullroot NEWLINE INDENT {printf("BISON:\tfound NEWLINE INDENT fullroot\n"); fprintf(logFile,"BISON:\tfound NEWLINE INDENT fullroot\n");}//{$$=$2;}
%%
block:	fullroot	{printf("BISON:\tfound fullroot\n"); fprintf(logFile,"BISON:\tfound fullroot\n");}//| fullroot DEDENT {printf("BISON:\tfound fullroot DEDENT\n"); fprintf(logFile,"BISON:\tfound fullroot DEDENT\n");}//{$$=$2;}
;
fullroot: fullroot NEWLINE {printf("BISON:\tconcatenated 2 strings\n"); fprintf(logFile,"BISON:\tconcatenated 2 strings\n");}//| fullroot NEWLINE {printf("BISON:\tconcatenated NEWLINE\n"); fprintf(logFile,"BISON:\tconcatenated NEWLINE\n");}
| fullroot '+' fullroot {printf("BISON:\tfound +\n"); fprintf(logFile,"BISON:\tfound +\n");}//{$$=$1+$3; root = $$;}
| fullroot '-' fullroot {printf("BISON:\tfound -\n"); fprintf(logFile,"BISON:\tfound -\n");}//{$$=$1-$3; root = $$;}
| fullroot POW fullroot {printf("BISON:\tfound POW\n"); fprintf(logFile,"BISON:\tfound POW\n");}//{$$=(int)pow((double)$1,$3); root = $$;}
| DIGIT {printf("BISON:\tfound DIGIT:\t%d\n",$1); fprintf(logFile,"BISON:\tfound DIGIT:\t%d\n",$1);}//{$$=$1; root = $$;}
| '(' fullroot ')' {printf("BISON:\tfound (fullroot)\n"); fprintf(logFile,"BISON:\tfound (fullroot)\n");}//{$$=$2;}
;
%%