%{
#include <process.h>
#include <conio.h>	
#include <locale.h>
#include "parser.tab.h"
#define INDNT 4
#pragma warning(disable : 4996)
FILE* logFile;
/* globals to track current indentation */
int current_line_indent = 0;   /* indentation of the current line */
int indent_level = 0;          /* indentation level passed to the parser */
extern int root;
void yyerror(const char* s);
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
%x INDENT
%%
%{
	char bufStr [1000] ="";
%}
#[^\n]*/\n	{printf("Комментарий\n");	fprintf(logFile,"Комментарий\n"); }
<INDENT>" "      { current_line_indent++; }
<INDENT>"\t"     { current_line_indent+=4;}
<INDENT>"\n"     { current_line_indent = 0; /*ignoring blank line */ }
<INDENT>.        {
					unput(*yytext);
					if (current_line_indent%INDNT!=0) 
				   {
                       printf("Ошибка: НЕВЕРНОЕ КОЛИЧЕСТВО ОТСТУПОВ. Завершение работы.\n");		
					   fprintf(logFile,"Ошибка: НЕВЕРНОЕ КОЛИЧЕСТВО ОТСТУПОВ. Завершение работы.\n");
					   return -1;
                   }
                   else if (current_line_indent > indent_level*INDNT) 
				   {
                       indent_level++;
					   /*current_line_indent-=INDNT;*/
                       printf("УВЕЛИЧЕНИЕ ОТСТУПА\n");		fprintf(logFile,"УВЕЛИЧЕНИЕ ОТСТУПА\n");
                   }
				   else if (current_line_indent < indent_level*INDNT)
				   {
                       indent_level--;
					   /*current_line_indent+=INDNT;*/
                       printf("УМЕНЬШЕНИЕ ОТСТУПА\n");		fprintf(logFile,"УМЕНЬШЕНИЕ ОТСТУПА\n");
                   } 
				   else
				   {
                       BEGIN(INITIAL);
                   }
                 }
[*]{2}			{printf("%s - операция\n", yytext);		fprintf(logFile,"%s - операция\n", yytext);} 
\*|\%|([/]{1,2})	{printf("%s - операция\n", yytext);		fprintf(logFile,"%s - операция\n", yytext);} 
\+ {return '+';}
-			{printf("%s - операция\n", yytext);		fprintf(logFile,"%s - операция\n", yytext);}
>|<|(<=)|(>=)	{printf("Нашел оператор сравнения: %s\n", yytext);	fprintf(logFile,"Нашел оператор сравнения: %s\n", yytext);}
(==)|(!=)		{printf("Нашел оператор сравнения: %s\n", yytext);	fprintf(logFile,"Нашел оператор сравнения: %s\n", yytext);}
(\"){3}				{ /*printf("Нашел многострочный текст через двойную кавычку\n");	fprintf(logFile,"Нашел многострочный текст через двойную кавычку\n"); strcpy(bufStr,"");*/ BEGIN(MULTILINE_DOUBLE_QUOTED_STRING);}
<MULTILINE_DOUBLE_QUOTED_STRING>[^\\\n\"']+	strcat(bufStr,yytext);
<MULTILINE_DOUBLE_QUOTED_STRING>\'|(\\\')	strcat(bufStr,"\'");
<MULTILINE_DOUBLE_QUOTED_STRING>\n	strcat(bufStr,"\n");
<MULTILINE_DOUBLE_QUOTED_STRING>\"|(\\\")	strcat(bufStr,"\"");
<MULTILINE_DOUBLE_QUOTED_STRING>\\\\	strcat(bufStr,"\\");
<MULTILINE_DOUBLE_QUOTED_STRING>(\"){3}	{ printf("Вывел многострочный текст через двойную кавычку: %s\n", bufStr);	fprintf(logFile,"Вывел многострочный текст через двойную кавычку: %s\n", bufStr); strcpy(bufStr,""); BEGIN(INITIAL);}
<MULTILINE_DOUBLE_QUOTED_STRING><<EOF>>	{printf("Нашел незакрытую кавычку. Конец парсинга.\n");	fprintf(logFile,"Нашел незакрытую кавычку. Конец парсинга.\n"); return -1;}
(\'){3}				{ /*printf("Нашел многострочный текст через одинарную кавычку\n");	fprintf(logFile,"Нашел многострочный текст через одинарную кавычку\n"); strcpy(bufStr,"");*/ BEGIN(MULTILINE_SINGLE_QUOTED_STRING);}
<MULTILINE_SINGLE_QUOTED_STRING>[^\\\n\"']+	strcat(bufStr,yytext);
<MULTILINE_SINGLE_QUOTED_STRING>\'|(\\\')	strcat(bufStr,"\'");
<MULTILINE_SINGLE_QUOTED_STRING>\n	strcat(bufStr,"\n");
<MULTILINE_SINGLE_QUOTED_STRING>\"|(\\\")	strcat(bufStr,"\"");
<MULTILINE_SINGLE_QUOTED_STRING>\\\\	strcat(bufStr,"\\");
<MULTILINE_SINGLE_QUOTED_STRING>(\'){3}	{ printf("Вывел многострочный текст через одинарную кавычку: %s\n", bufStr);	fprintf(logFile,"Вывел многострочный текст через одинарную кавычку: %s\n", bufStr); strcpy(bufStr,""); BEGIN(INITIAL);}
<MULTILINE_SINGLE_QUOTED_STRING><<EOF>>	{printf("Нашел незакрытую кавычку. Конец парсинга.\n");	fprintf(logFile,"Нашел незакрытую кавычку. Конец парсинга.\n"); return -1;}
\'				{ /*printf("Нашел одинарную кавычку\n");	fprintf(logFile,"Нашел одинарную кавычку\n"); strcpy(bufStr,"");*/ BEGIN(SINGLE_QUOTED_STRING);}
<SINGLE_QUOTED_STRING>[^\\\n\'"]+	strcat(bufStr,yytext);
<SINGLE_QUOTED_STRING>\\n	strcat(bufStr,"\n");
<SINGLE_QUOTED_STRING>\"|(\\\")	strcat(bufStr,"\"");
<SINGLE_QUOTED_STRING>\\\\	strcat(bufStr,"\\");
<SINGLE_QUOTED_STRING>\\\'	strcat(bufStr,"\'");
<SINGLE_QUOTED_STRING>\'	{ printf("Вывел строку в одинарных кавычках: %s\n", bufStr);	fprintf(logFile,"Вывел строку в одинарных кавычках: %s\n", bufStr); strcpy(bufStr,""); BEGIN(INITIAL);}
<SINGLE_QUOTED_STRING><<EOF>>	{printf("Нашел незакрытую кавычку. Конец парсинга.\n");	fprintf(logFile,"Нашел незакрытую кавычку. Конец парсинга.\n"); return -1;}
\"				{ /*printf("Нашел кавычку\n");	fprintf(logFile,"Нашел кавычку\n"); strcpy(bufStr,"");*/ BEGIN(DOUBLE_QUOTED_STRING);}
<DOUBLE_QUOTED_STRING>[^\\\n\"']+	strcat(bufStr,yytext);
<DOUBLE_QUOTED_STRING>\'|(\\\')	strcat(bufStr,"\'");
<DOUBLE_QUOTED_STRING>\\n	strcat(bufStr,"\n");
<DOUBLE_QUOTED_STRING>\\\\	strcat(bufStr,"\\");
<DOUBLE_QUOTED_STRING>\\\"	strcat(bufStr,"\"");
<DOUBLE_QUOTED_STRING>\"	{ printf("Вывел строку в двойных кавычках: %s\n", bufStr);	fprintf(logFile,"Вывел строку в двойных кавычках: %s\n", bufStr); strcpy(bufStr,""); BEGIN(INITIAL);}
<DOUBLE_QUOTED_STRING><<EOF>>	{printf("Нашел незакрытую двойную кавычку. Конец парсинга.\n");	fprintf(logFile,"Нашел незакрытую двойную кавычку. Конец парсинга.\n"); return -1;}
<<EOF>> {puts("Нашел конец файла");				fprintf(logFile,"Нашел конец файла"); return 0;}
=|([%]=)|([/]{1,2}[=])|([-][=])|([+][=])|([*]{1,2}[=])			{printf("%s - операция\n", yytext);		fprintf(logFile,"%s - операция\n", yytext);}
True			{printf("%s - ключевое слово\n", yytext);	fprintf(logFile,"%s - ключевое слово\n", yytext);}
False			{printf("%s - ключевое слово\n", yytext);	fprintf(logFile,"%s - ключевое слово\n", yytext);}
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
"return"		{printf("%s - ключевое слово\n", yytext);		fprintf(logFile,"%s - ключевое слово\n", yytext);}
:	{printf("%s - ключевое слово\n", yytext);		fprintf(logFile,"%s - ключевое слово\n", yytext);}
def {printf("%s - ключевое слово\n", yytext);	fprintf(logFile,"%s - ключевое слово\n", yytext);}
del {printf("%s - ключевое слово\n", yytext);	fprintf(logFile,"%s - ключевое слово\n", yytext);}
\[ {printf("%s - открывающаяся квадратная скобка\n", yytext);	fprintf(logFile,"%s - открывающаяся квадратная скобка\n", yytext);}
\] {printf("%s - закрывающаяся квадратная скобка\n", yytext);	fprintf(logFile,"%s - закрывающаяся квадратная скобка\n", yytext);}
\( {printf("%s - открывающаяся круглая скобка\n", yytext);	fprintf(logFile,"%s - открывающаяся круглая скобка\n", yytext);}
\) {printf("%s - закрывающаяся круглая скобка\n", yytext);	fprintf(logFile,"%s - закрывающаяся круглая скобка\n", yytext);}
,			{printf("%s - запятая\n", yytext);	fprintf(logFile,"%s - запятая\n", yytext);}
{VAR}		{printf("Нашел идентификатор: %s\n", yytext);	fprintf(logFile,"Нашел идентификатор: %s\n", yytext);}
{DIGIT}+	{printf("Нашел число: %s\n", yytext);			fprintf(logFile,"Нашел число: %s\n", yytext); yylval.Int=atoi(yytext); return DIGIT;}
{WHITE_SPACE}+	{/*puts("Нашел белый разделитель");		fprintf(logFile,"Нашел белый разделитель\n");*/}
\n				{puts("Новая строка");					fprintf(logFile,"Новая строка\n"); current_line_indent = 0; BEGIN(INDENT);}
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
			yyparse();//yylex();	
		}while(!feof(yyin));
		fclose(yyin);
		fclose(logFile);
		printf("result is %d", root);
	}
	_getch();
	return 0;
}

void yyerror(const char* s) {
	//printf("Parse error! Message: %s",s);
	// might as well halt now:
	//exit(-1);
}