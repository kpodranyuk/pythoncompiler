%{
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
%x INDENT_
%%
%{
	static int dedents_count=0;
	char bufStr [1000] ="";
	while(dedents_count>0)
	{
		dedents_count--;
	   /*current_line_indent+=INDNT;*/
	   printf("FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");		fprintf(logFile,"FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");
	   return DEDENT;
	}
%}
#[^\n]*/\n	{printf("FLEX:\tКомментарий\n");	fprintf(logFile,"FLEX:\tКомментарий\n"); }
<INDENT_>" "      { current_line_indent++; }
<INDENT_>"\t"     { current_line_indent+=4;}
<INDENT_>"\n"     { current_line_indent = 0; /*ignoring blank line */ }
<INDENT_>.        {
					unput(*yytext);
					if (current_line_indent%INDNT!=0) 
				    {
                       printf("FLEX:\tОшибка: НЕВЕРНОЕ КОЛИЧЕСТВО ОТСТУПОВ. Завершение работы.\n");		
					   fprintf(logFile,"FLEX:\tОшибка: НЕВЕРНОЕ КОЛИЧЕСТВО ОТСТУПОВ. Завершение работы.\n");
					   return -1;
                    }
                    else if (current_line_indent > indent_level*INDNT) 
				    {
                       indent_level++;
					   /*current_line_indent-=INDNT;*/
					   BEGIN(INITIAL);
					   printf("FLEX:\tУВЕЛИЧЕНИЕ ОТСТУПА\n");		fprintf(logFile,"FLEX:\tУВЕЛИЧЕНИЕ ОТСТУПА\n");
					   return INDENT;
                    }
				    else if (current_line_indent < indent_level*INDNT)
				    {
                       indent_level--;
					   dedents_count=(indent_level*INDNT-current_line_indent)/INDNT;
					   /*current_line_indent+=INDNT;*/
                       printf("FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");		fprintf(logFile,"FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");
					   return DEDENT;
                    } 
				    else
				    {
                       BEGIN(INITIAL);
                    }
                 }
,	{return ',';}
[*]{2}			{
					printf("FLEX:\t%s - операция\n", yytext);		
					fprintf(logFile,"FLEX:\t%s - операция\n", yytext);	
					return POW;
				} 
\*	{
		printf("FLEX:\t%s - операция\n", yytext);		
		fprintf(logFile,"FLEX:\t%s - операция\n", yytext);	
		return MUL;
	} 
\%	{
		printf("%s - операция\n", yytext);		
		fprintf(logFile,"%s - операция\n", yytext);	
		return MOD;
	} 
[/]	{
		printf("FLEX:\t%s - операция\n", yytext);		
		fprintf(logFile,"FLEX:\t%s - операция\n", yytext);	
		return DIV;
	} 
[/]{2}	{
			printf("FLEX:\t%s - операция\n", yytext);		
			fprintf(logFile,"FLEX:\t%s - операция\n", yytext);	
			return INT;
		} 
\+ 		{
			printf("FLEX:\t%s - операция\n", yytext);		
			fprintf(logFile,"FLEX:\t%s - операция\n", yytext); 
			return '+';
		}
-		{
			printf("FLEX:\t%s - операция\n", yytext);		
			fprintf(logFile,"FLEX:\t%s - операция\n", yytext);	
			return '-';
		}
\>	{
		printf("FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		fprintf(logFile,"FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		return '>';
	}
\<	{
		printf("FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		fprintf(logFile,"FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		return '<';
	}
\<=	{	
		printf("FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		fprintf(logFile,"FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		return LESS_OR_EQUAL;
	}
\>=	{
		printf("FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		fprintf(logFile,"FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		return GREATER_OR_EQUAL;
	}
==	{
		printf("FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		fprintf(logFile,"FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		return EQUAL;
	}
!=	{
		printf("FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		fprintf(logFile,"FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		return NOT_EQUAL;
	}
(\"){3}				{ /*printf("Нашел многострочный текст через двойную кавычку\n");	fprintf(logFile,"Нашел многострочный текст через двойную кавычку\n"); strcpy(bufStr,"");*/ BEGIN(MULTILINE_DOUBLE_QUOTED_STRING);}
<MULTILINE_DOUBLE_QUOTED_STRING>[^\\\n\"']+	strcat(bufStr,yytext);
<MULTILINE_DOUBLE_QUOTED_STRING>\'|(\\\')	strcat(bufStr,"\'");
<MULTILINE_DOUBLE_QUOTED_STRING>\n	strcat(bufStr,"\n");
<MULTILINE_DOUBLE_QUOTED_STRING>\"|(\\\")	strcat(bufStr,"\"");
<MULTILINE_DOUBLE_QUOTED_STRING>\\\\	strcat(bufStr,"\\");
<MULTILINE_DOUBLE_QUOTED_STRING>(\"){3}	{ printf("FLEX:\tВывел многострочный текст через двойную кавычку: %s\n", bufStr);	fprintf(logFile,"FLEX:\tВывел многострочный текст через двойную кавычку: %s\n", bufStr); strcpy(bufStr,""); BEGIN(INITIAL);}
<MULTILINE_DOUBLE_QUOTED_STRING><<EOF>>	{printf("FLEX:\tНашел незакрытую кавычку. Конец парсинга.\n");	fprintf(logFile,"FLEX:\tНашел незакрытую кавычку. Конец парсинга.\n"); return -1;}
(\'){3}				{ /*printf("Нашел многострочный текст через одинарную кавычку\n");	fprintf(logFile,"Нашел многострочный текст через одинарную кавычку\n"); strcpy(bufStr,"");*/ BEGIN(MULTILINE_SINGLE_QUOTED_STRING);}
<MULTILINE_SINGLE_QUOTED_STRING>[^\\\n\"']+	strcat(bufStr,yytext);
<MULTILINE_SINGLE_QUOTED_STRING>\'|(\\\')	strcat(bufStr,"\'");
<MULTILINE_SINGLE_QUOTED_STRING>\n	strcat(bufStr,"\n");
<MULTILINE_SINGLE_QUOTED_STRING>\"|(\\\")	strcat(bufStr,"\"");
<MULTILINE_SINGLE_QUOTED_STRING>\\\\	strcat(bufStr,"\\");
<MULTILINE_SINGLE_QUOTED_STRING>(\'){3}	{ printf("FLEX:\tВывел многострочный текст через одинарную кавычку: %s\n", bufStr);	fprintf(logFile,"FLEX:\tВывел многострочный текст через одинарную кавычку: %s\n", bufStr); strcpy(bufStr,""); BEGIN(INITIAL);}
<MULTILINE_SINGLE_QUOTED_STRING><<EOF>>	{printf("FLEX:\tНашел незакрытую кавычку. Конец парсинга.\n");	fprintf(logFile,"FLEX:\tНашел незакрытую кавычку. Конец парсинга.\n"); return -1;}
\'				{ /*printf("Нашел одинарную кавычку\n");	fprintf(logFile,"Нашел одинарную кавычку\n"); strcpy(bufStr,"");*/ BEGIN(SINGLE_QUOTED_STRING);}
<SINGLE_QUOTED_STRING>[^\\\n\'"]+	strcat(bufStr,yytext);
<SINGLE_QUOTED_STRING>\\n	strcat(bufStr,"\n");
<SINGLE_QUOTED_STRING>\"|(\\\")	strcat(bufStr,"\"");
<SINGLE_QUOTED_STRING>\\\\	strcat(bufStr,"\\");
<SINGLE_QUOTED_STRING>\\\'	strcat(bufStr,"\'");
<SINGLE_QUOTED_STRING>\'	{ printf("FLEX:\tВывел строку в одинарных кавычках: %s\n", bufStr);	fprintf(logFile,"FLEX:\tВывел строку в одинарных кавычках: %s\n", bufStr); strcpy(bufStr,""); BEGIN(INITIAL);}
<SINGLE_QUOTED_STRING><<EOF>>	{printf("FLEX:\tНашел незакрытую кавычку. Конец парсинга.\n");	fprintf(logFile,"FLEX:\tНашел незакрытую кавычку. Конец парсинга.\n"); return -1;}
\"				{ /*printf("Нашел кавычку\n");	fprintf(logFile,"Нашел кавычку\n"); strcpy(bufStr,"");*/ BEGIN(DOUBLE_QUOTED_STRING);}
<DOUBLE_QUOTED_STRING>[^\\\n\"']+	strcat(bufStr,yytext);
<DOUBLE_QUOTED_STRING>\'|(\\\')	strcat(bufStr,"\'");
<DOUBLE_QUOTED_STRING>\\n	strcat(bufStr,"\n");
<DOUBLE_QUOTED_STRING>\\\\	strcat(bufStr,"\\");
<DOUBLE_QUOTED_STRING>\\\"	strcat(bufStr,"\"");
<DOUBLE_QUOTED_STRING>\"	{ printf("FLEX:\tВывел строку в двойных кавычках: %s\n", bufStr);	fprintf(logFile,"FLEX:\tВывел строку в двойных кавычках: %s\n", bufStr); strcpy(bufStr,""); BEGIN(INITIAL);}
<DOUBLE_QUOTED_STRING><<EOF>>	{printf("FLEX:\tНашел незакрытую двойную кавычку. Конец парсинга.\n");	fprintf(logFile,"FLEX:\tНашел незакрытую двойную кавычку. Конец парсинга.\n"); return -1;}
<<EOF>> {
			puts("FLEX:\tНашел конец файла");				
			fprintf(logFile,"FLEX:\tНашел конец файла\n"); 
			if(indent_level!=0)
			{
				dedents_count=indent_level;//(indent_level*INDNT-current_line_indent)/INDNT; 
				//unput(*yytext);
				indent_level=0;
				while(dedents_count>0)
				{
					dedents_count--;
				   /*current_line_indent+=INDNT;*/
				   printf("FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");		fprintf(logFile,"FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");
				   return DEDENT;
				}
			}
			return 0;
			
		}
=		{
			printf("FLEX:\t%s - операция\n", yytext);		
			fprintf(logFile,"FLEX:\t%s - операция\n", yytext);	
			return '=';
		}
([%]=)|([/]{1,2}[=])|([-][=])|([+][=])|([*]{1,2}[=])			{printf("%s - операция\n", yytext);		fprintf(logFile,"%s - операция\n", yytext);}
True		{
				printf("FLEX:\t%s - ключевое слово\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return TRUE;
			}
False		{
				printf("FLEX:\t%s - ключевое слово\n", yytext);	
				fprintf(logFile,"%FLEX:\ts - ключевое слово\n", yytext);	
				return FALSE;
			}
not			{
				printf("FLEX:\t%s - ключевое слово\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return NOT;
			}
and			{	
				printf("FLEX:\t%s - ключевое слово\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);		
				return AND;
			}
or			{
				printf("FLEX:\t%s - ключевое слово\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
			
				return OR;
			}
while		{
				printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return WHILE;
			} 
for			{	
				printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return FOR;
			}
in			{
				printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return IN;
			}
continue	{
				printf("FLEX:\t%s - ключевое слово\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return CONTINUE;
			} 
break		{
				printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return BREAK;
			} 
if			{
				printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return IF;
			} 
elif		{
				printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return ELIF;
			} 
else		{
				printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return ELSE;
			} 
"return"	{
				printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return RETURN;
			}
:	{
		printf("FLEX:\t%s - ключевое слово\n", yytext);		
		fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
		return ':';
	}
def	{
		printf("FLEX:\t%s - ключевое слово\n", yytext);	
		fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
		return DEF;
	}
del	{
		printf("FLEX:\t%s - ключевое слово\n", yytext);	
		fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
		return DEL;
	}
\[ 	{
		printf("FLEX:\t%s - открывающаяся квадратная скобка\n", yytext);	
		fprintf(logFile,"FLEX:\t%s - открывающаяся квадратная скобка\n", yytext);	
		return '[';
	}
\] 	{
		printf("FLEX:\t%s - закрывающаяся квадратная скобка\n", yytext);	
		fprintf(logFile,"FLEX:\t%s - закрывающаяся квадратная скобка\n", yytext);	
		return ']';
	}
\( 	{
		printf("FLEX:\t%s - открывающаяся круглая скобка\n", yytext);	
		fprintf(logFile,"FLEX:\t%s - открывающаяся круглая скобка\n", yytext);	
		return '(';
	}
\) 	{
		printf("FLEX:\t%s - закрывающаяся круглая скобка\n", yytext);	
		fprintf(logFile,"FLEX:\t%s - закрывающаяся круглая скобка\n", yytext);	
		return ')';
	}
,			{
				printf("FLEX:\t%s - запятая\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - запятая\n", yytext);	
				return ',';
			}
{VAR}		{
				printf("FLEX:\tНашел идентификатор: %s\n", yytext);	
				fprintf(logFile,"FLEX:\tНашел идентификатор: %s\n", yytext);	
				yylval.StringVal=(char*)memset(malloc(strlen(yytext)+1),'\0',strlen(yytext)+1);	
				strcpy(yylval.StringVal, yytext); 
				return OPERAND;
			}
{DIGIT}+	{
				printf("FLEX:\tНашел число: %s\n", yytext);			
				fprintf(logFile,"FLEX:\tНашел число: %s\n", yytext); 
				yylval.Int=atoi(yytext); 
				return DIGIT;
			}
{WHITE_SPACE}+	{/*puts("Нашел белый разделитель");		fprintf(logFile,"Нашел белый разделитель\n");*/}
\n			{
				puts("FLEX:\tНовая строка");					
				fprintf(logFile,"FLEX:\tНовая строка\n"); 
				current_line_indent = 0; 
				BEGIN(INDENT_);
				return NEWLINE;
			}
%%
int main(int argc, char** argv) {
	setlocale(LC_ALL, "RUS");
	/*if(argc<2)
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
	{*/
		// set flex to read from it instead of defaulting to STDIN:
		yyin = fopen("test1.py","rt");//(argv[1], "rt");
		if(yyin==NULL)
		{
			//printf("Ошибка! Невозможно открыть файл %s на чтение!", argv[1]);
			printf("Ошибка! Невозможно открыть файл на чтение!");
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
		//printf("result is %d", root);
	/*}*/
	_getch();
	return 0;
}

void yyerror(const char* s) {
	printf("Parse error! Message: %s",s);
	// might as well halt now:
	//exit(-1);
}