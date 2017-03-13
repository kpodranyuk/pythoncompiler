%{
#include <process.h>
#include <conio.h>	
#include <locale.h>
FILE* logFile;
%}
%option noyywrap 
%option never-interactive
DIGIT [0-9]
LETTER [A-Za-zА-Яа-я]
VAR [A-Za-zА-Яа-я_][A-Za-zА-Яа-я_0-9]*
VARVAL [A-Za-zА-Яа-я0-9]
WHITE_SPACE[ \t]
%x DOUBLE_QUOTED_STRING
%x SINGLE_QUOTED_STRING
%x MULTILINE_DOUBLE_QUOTED_STRING
%x MULTILINE_SINGLE_QUOTED_STRING
%%
%{
	char bufStr [1000] ="";
%}
<<EOF>> {puts("Нашел конец файла");				fprintf(logFile,"Нашел конец файла\n"); return 0;}
#[^\n]*\n	{printf("Комментарий\n");	fprintf(logFile,"Комментарий\n");}
**			{printf("%s - операция\n", yytext);		fprintf(logFile,"%s - операция\n", yytext);} 
*|\%|(//)	{printf("%s - операция\n", yytext);		fprintf(logFile,"%s - операция\n", yytext);} 
+|-			{printf("%s - операция\n", yytext);		fprintf(logFile,"%s - операция\n", yytext);}
>|<|(<=)|(>=)	{printf("Нашел оператор сравнения: %s\n", yytext);	fprintf(logFile,"Нашел оператор сравнения: %s\n", yytext);}
(==)|(!=)		{printf("Нашел оператор сравнения: %s\n", yytext);	fprintf(logFile,"Нашел оператор сравнения: %s\n", yytext);}
\"				{ printf("Нашел кавычку\n");	fprintf(logFile,"Нашел кавычку\n"); strcpy(bufStr,""); BEGIN(DOUBLE_QUOTED_STRING);}
<DOUBLE_QUOTED_STRING>[^\\\n\"]+	strcat(bufStr,yytext);
<DOUBLE_QUOTED_STRING>\\n	strcat(bufStr,"\n");
<DOUBLE_QUOTED_STRING>\\\\	strcat(bufStr,"\\");
<DOUBLE_QUOTED_STRING>\\\"	strcat(bufStr,"\"");
<DOUBLE_QUOTED_STRING>\"	{ printf("Нашел строку в двойных кавычках: %s\n", bufStr);	fprintf(logFile,"Нашел строку в двойных кавычках: %s\n", bufStr); strcpy(bufStr,""); BEGIN(INITIAL);}
<DOUBLE_QUOTED_STRING><<EOF>>	{printf("Нашел незакрытую двойную кавычку. Конец парсинга.\n");	fprintf(logFile,"Нашел незакрытую двойную кавычку. Конец парсинга.\n"); return -1;}
=|(\%=)|(/=)|(//=)|(-=)|(+=)|(*=)|(**=)			{printf("%s - операция\n", yytext);		fprintf(logFile,"%s - операция\n", yytext);}
not			{printf("%s - ключевое слово\n", yytext);	fprintf(logFile,"%s - ключевое слово\n", yytext);}
and			{printf("%s - ключевое слово\n", yytext);	fprintf(logFile,"%s - ключевое слово\n", yytext);}
or			{printf("%s - ключевое слово\n", yytext);	fprintf(logFile,"%s - ключевое слово\n", yytext);}
while		{printf("%s - ключевое слово\n", yytext);		fprintf(logFile,"%s - ключевое слово\n", yytext);} 
for			{printf("%s - ключевое слово\n", yytext);		fprintf(logFile,"%s - ключевое слово\n", yytext);}
in			{printf("%s - ключевое слово\n", yytext);		fprintf(logFile,"%s - ключевое слово\n", yytext);}
continue	{printf("%s - ключевое слово\n", yytext);	fprintf(logFile,"%s - ключевое слово\n", yytext);} 
break		{printf("%s - ключевое слово\n", yytext);		fprintf(logFile,"%s - ключевое слово\n", yytext);} 
if			{printf("%s - ключевое слово\n", yytext);		fprintf(logFile,"%s - ключевое слово\n", yytext);} 
elif		{printf("%s - ключевое слово\n", yytext);		fprintf(logFile,"%s - ключевое слово\n", yytext);} 
else		{printf("%s - ключевое слово\n", yytext);		fprintf(logFile,"%s - ключевое слово\n", yytext);} 
:	{printf("%s - ключевое слово\n", yytext);		fprintf(logFile,"%s - ключевое слово\n", yytext);}
def {printf("%s - ключевое слово\n", yytext);	fprintf(logFile,"%s - ключевое слово\n", yytext);}
del {printf("%s - ключевое слово\n", yytext);	fprintf(logFile,"%s - ключевое слово\n", yytext);}
>|<|<=|>= {printf("Нашел оператор сравнения: %s\n", yytext);	fprintf(logFile,"Нашел оператор сравнения: %s\n", yytext);}
\[ {printf("%s - открывающаяся квадратная скобка\n", yytext);	fprintf(logFile,"%s - открывающаяся квадратная скобка\n", yytext);}
\] {printf("%s - закрывающаяся квадратная скобка\n", yytext);	fprintf(logFile,"%s - закрывающаяся квадратная скобка\n", yytext);}
\( {printf("%s - открывающаяся круглая скобка\n", yytext);	fprintf(logFile,"%s - открывающаяся круглая скобка\n", yytext);}
\) {printf("%s - закрывающаяся круглая скобка\n", yytext);	fprintf(logFile,"%s - закрывающаяся круглая скобка\n", yytext);}
,			{printf("%s - запятая\n", yytext);	fprintf(logFile,"%s - запятая\n", yytext);}
{VAR}		{printf("Нашел вызов переменной: %s\n", yytext);	fprintf(logFile,"Нашел вызов переменной: %s\n", yytext);}
{DIGIT}+	{printf("Нашел число: %s\n", yytext);			fprintf(logFile,"Нашел число: %s\n", yytext);}
{WHITE_SPACE}+	{puts("Нашел белый разделитель");		fprintf(logFile,"Нашел белый разделитель\n");}
\n				{puts("Новая строка");								fprintf(logFile,"Новая строка\n");}
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
		logFile = fopen("log.txt", "wt");
		do{
			yylex();	
		}while(!feof(yyin));
		fclose(yyin);
	}
	_getch();
	return 0;
}