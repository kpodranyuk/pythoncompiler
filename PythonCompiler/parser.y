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
%token INDENT	// Начало блока кода
%token DEDENT	// Конец блока кода
%token '='		// Оператор присвоения
%left '+'		// Оператор сложения
%left '-'		// Оператор вычитания
%left POW		// Оператор возведения в степень
%left MUL		// Оператор умножения
%left DIV		// Оператор деления
%left MOD		// Оператор остатка от деления
%left INT		// Оператор целой части от деления (возможна замена одним символом)
%token '<'		// Оператор меньше
%token '>'		// Оператор больше
%token LESS_OR_EQUAL		// Оператор меньше либо равно
%token GREATER_OR_EQUAL		// Оператор больше либо равно
%token EQUAL		// Оператор равенства
%token NOT_EQUAL	// Оператор неравенства
%token TRUE		// Оператор логической истины
%token FALSE	// Оператор false
%token NOT		// Оператор "не"
%token AND		// Оператор и
%token OR		// Оператор или
%token WHILE	// Оператор цикла пока
%token FOR		// Оператор цикла для
%token IN		// Оператор in
%token CONTINUE	// Оператор продолжения цикла
%token BREAK	// Оператор прерывания
%token IF		// Оператор условия if
%token ELIF		// Оператор условия elif
%token ELSE		// Оператор условия else 
%token RETURN	// Оператор возврата значения
%token DEF		// Оператор объявления функции
%token DEL		// Оператор удаления переменной
%token ':'		// Двоеточие
%token ','		// Запятая
%token '['		// Открывающая квадратная скобка
%token ']'		// Закрывающая квадратная скобка
%nonassoc '('		// Открывающая круглая скобка
%nonassoc ')'		// Закрывающая круглая скобка
%token <StringVal>OPERAND	// Операнд
%token <StringVal>STRING	// Строка
%left <Int>DIGIT			// Число
%start fullroot		// Стартовый символ
%%
fullroot: fullroot '+' fullroot {$$=$1+$3; root = $$;}
| fullroot '-' fullroot {$$=$1-$3; root = $$;}
| fullroot POW fullroot {$$=(int)pow((double)$1,$3); root = $$;}
| DIGIT {printf("BISON:\tfound DIGIT:\t%d\n",$1); fprintf(logFile,"BISON:\tfound DIGIT:\t%d\n",$1);}//{$$=$1; root = $$;}
| '(' fullroot ')' {$$=$2;}
;
%%