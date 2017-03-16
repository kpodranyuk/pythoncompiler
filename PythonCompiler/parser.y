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
	char* StringVal;
};
%type <Int> fullroot
%token INDENT	// Начало блока кода
%token DEDENT	// Конец блока кода
%token POW		// Оператор возведения в степень
%token MUL		// Оператор умножения
%token DIV		// Оператор деления
%token MOD		// Оператор остатка от деления
%token INT		// Оператор целой части от деления (возможна замена одним символом)
%left '+'		// Оператор сложения
%token '-'		// Оператор вычитания
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
%token '('		// Открывающая круглая скобка
%token ')'		// Закрывающая круглая скобка
%token <StringVal>OPERAND	// Операнд
%left <Int>DIGIT			// Число
%start fullroot		// Стартовый символ
%%
fullroot: DIGIT {$$=$1; root = $$;}
| fullroot '+' fullroot {$$=$1+$3; root = $$;};
%%