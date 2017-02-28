%{
#include <process.h>
#include <conio.h>	
#include <locale.h>
%}
%option noyywrap 
%option never-interactive
DIGIT [0-9]
LETTER [A-Za-zА-Яа-я]
VAR [A-Za-zА-Яа-я_][A-Za-zА-Яа-я_0-9]*
VARVAL [A-Za-zА-Яа-я0-9]
WHITE_SPACE[ \t]
%%
%{
	char bufStr [100] ="";
%}
<<EOF>> {puts("Нашел конец файла"); return 0;}
\" { printf("Нашел кавычку\n");}
while {printf("%s - ключевое слово\n", yytext);} 
for {printf("%s - ключевое слово\n", yytext);}
in {printf("%s - ключевое слово\n", yytext);}
continue {printf("%s - ключевое слово\n", yytext);} 
break {printf("%s - ключевое слово\n", yytext);} 
if {printf("%s - ключевое слово\n", yytext);} 
elif {printf("%s - ключевое слово\n", yytext);} 
else {printf("%s - ключевое слово\n", yytext);} 
: {printf("%s - ключевое слово\n", yytext);}
not {printf("%s - ключевое слово\n", yytext);}
and {printf("%s - ключевое слово\n", yytext);}
or {printf("%s - ключевое слово\n", yytext);}
>|<|<=|>= {printf("Нашел оператор сравнения: %s\n", yytext);}
\[ {printf("%s - открывающаяся квадратная скобка\n", yytext);}
\] {printf("%s - закрывающаяся квадратная скобка\n", yytext);}
\( {printf("%s - открывающаяся круглая скобка\n", yytext);}
\) {printf("%s - закрывающаяся круглая скобка\n", yytext);}
, {printf("%s - запятая\n", yytext);}
{VAR}{WHITE_SPACE}*/= {strcpy(bufStr,""); 
			   strncpy(bufStr,yytext,strcspn(yytext," \t")); 
			   strcat(bufStr,"\0"); 
			   printf("Нашел имя переменной: %s\n", bufStr);}
= { puts("Нашел присвоение");}
{VAR} {printf("Нашел вызов переменной: %s\n", yytext);}
{DIGIT}+ {printf("Нашел число: %s\n", yytext);}
{WHITE_SPACE}+ {puts("Нашел белый разделитель");}
\n {puts("Новая строка");}
%%
int main(int argc, char** argv) {
	setlocale(LC_ALL, "RUS");
	if(argc<2)
	{
		printf("Недостаточно параметров для вызова программы");
		return -1;
	}
	else if(argc>3)
	{
		printf("Слишком много параметров для вызова программы");
		return -1;
	}
	else
	{
		// set flex to read from it instead of defaulting to STDIN:
		yyin = fopen(argv[1], "rt");
		if(yyin==NULL)
		{
			printf("Ошибка! Невозможно открыть файл %s на чтение!", argv[1]);
			exit(-1);
		}
		// parse through the input until there is no more:
		//rewind(yyin);
		do{
			yylex();	
		}while(!feof(yyin));
		fclose(yyin);
	}
	_getch();
	return 0;
}