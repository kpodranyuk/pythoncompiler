%{
#include <stdio.h>
#include <string>
#include <conio.h>
#include <locale.h>
#include "parser.tab.h"
#define INDNT 4
#pragma warning(disable : 4996)
extern FILE* logFile;
extern FILE* logFileB;
/* globals to track current indentation */
int current_line_indent = 0;   /* indentation of the current line */
int indent_level = 0;          /* indentation level passed to the parser */
void yyerror(const char* s);
extern int yylex();
int stringIndex=1, charIndex=0;			/**<Перменные, хранящие идексы лексемы*/
float fff;
double ddd;
int step;
/*!
* Функция для определения координат лексемы
* \param[in] index1 Начальное положение лексемы в строке
* \param[in] index2 Индекс строки
* \return Обновленное положение лексемы в строке
*/
int findLexemLocation(int index1, int index2);
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
	static int eof_count=0;
	char bufStr [1000] ="";
	if(eof_count==1&&dedents_count>0)
	{
		//return EOF_TOKEN;
		while(dedents_count>0)
		{
			dedents_count--;
			if(indent_level>0)
			{indent_level--;}
		   /*current_line_indent+=INDNT;*/
		   fprintf(logFile,"FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");//printf("FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");		
		   return DEDENT;
		}
		/*puts("FLEX:\tВернул конец файла");
		fprintf(logFile,"FLEX:\tВернул конец файла\n"); */
		eof_count=0;
		//return EOF_TOKEN;
		yyterminate();
	}
	else if (eof_count==0)
	{
		while(dedents_count>0)
		{
			dedents_count--;
			if(indent_level>0)
			{indent_level--;}
		   /*current_line_indent+=INDNT;*/
		   fprintf(logFile,"FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");//printf("FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");		
		   return DEDENT;
		}
	}
	else if (eof_count==1&&dedents_count==0)
	{
		yyterminate();
	}
%}
#[^\n]*/\n	{/*printf("FLEX:\tКомментарий\n");*/	fprintf(logFile,"FLEX:\tКомментарий\n"); }
<INDENT_>" "      	{ charIndex+=1; current_line_indent++; }
<INDENT_>"\t"     	{ charIndex+=4; current_line_indent+=4;}
<INDENT_>"\n"     	{ charIndex=0; stringIndex++; current_line_indent = 0; /*ignoring blank line */ }
<INDENT_>#[^\n]*  	{ charIndex=findLexemLocation(charIndex,stringIndex); current_line_indent = 0; /*ignoring blank line */ }
<INDENT_>[^#]  		{
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
						   fprintf(logFile,"FLEX:\tУВЕЛИЧЕНИЕ ОТСТУПА\n");//printf("FLEX:\tУВЕЛИЧЕНИЕ ОТСТУПА\n");		
						   return INDENT;
						}
						else if (current_line_indent < indent_level*INDNT)
						{
						   indent_level--;
						   dedents_count=(indent_level*INDNT-current_line_indent)/INDNT;
						   BEGIN(INITIAL);
						   /*current_line_indent+=INDNT;*/
						   fprintf(logFile,"FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");//printf("FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");		
						   return DEDENT;
						} 
						else
						{
						   BEGIN(INITIAL);
						}
					}
[*]{2}			{
					charIndex=findLexemLocation(charIndex,stringIndex);  
					//printf("FLEX:\t%s - операция\n", yytext);		
					fprintf(logFile,"FLEX:\t%s - операция\n", yytext);	
					return POW;
				} 
\*	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\t%s - операция\n", yytext);		
		fprintf(logFile,"FLEX:\t%s - операция\n", yytext);	
		return MUL;
	} 
\%	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("%s - операция\n", yytext);		
		fprintf(logFile,"%s - операция\n", yytext);	
		return MOD;
	} 
[/]	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\t%s - операция\n", yytext);		
		fprintf(logFile,"FLEX:\t%s - операция\n", yytext);	
		return DIV;
	} 
[/]{2}	{
			charIndex=findLexemLocation(charIndex,stringIndex);
			//printf("FLEX:\t%s - операция\n", yytext);		
			fprintf(logFile,"FLEX:\t%s - операция\n", yytext);	
			return IINT;
		} 
\+ 		{
			charIndex=findLexemLocation(charIndex,stringIndex);
			//printf("FLEX:\t%s - операция\n", yytext);		
			fprintf(logFile,"FLEX:\t%s - операция\n", yytext); 
			return '+';
		}
-		{
			charIndex=findLexemLocation(charIndex,stringIndex);
			//printf("FLEX:\t%s - операция\n", yytext);		
			fprintf(logFile,"FLEX:\t%s - операция\n", yytext);	
			return '-';
		}
\>	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		fprintf(logFile,"FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		return '>';
	}
\<	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		fprintf(logFile,"FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		return '<';
	}
\<=	{	
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		fprintf(logFile,"FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		return LESS_OR_EQUAL;
	}
\>=	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		fprintf(logFile,"FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		return GREATER_OR_EQUAL;
	}
==	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		fprintf(logFile,"FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		return EQUAL;
	}
!=	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		fprintf(logFile,"FLEX:\tНашел оператор сравнения: %s\n", yytext);	
		return NOT_EQUAL;
	}
(\"){3}				{charIndex=findLexemLocation(charIndex,stringIndex); /*printf("Нашел многострочный текст через двойную кавычку\n");	fprintf(logFile,"Нашел многострочный текст через двойную кавычку\n"); strcpy(bufStr,"");*/ BEGIN(MULTILINE_DOUBLE_QUOTED_STRING);}
<MULTILINE_DOUBLE_QUOTED_STRING,MULTILINE_SINGLE_QUOTED_STRING>[^\\\n\"']+	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,yytext);}
<MULTILINE_DOUBLE_QUOTED_STRING,MULTILINE_SINGLE_QUOTED_STRING>\'|(\\\')	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,"\'");}
<MULTILINE_DOUBLE_QUOTED_STRING,MULTILINE_SINGLE_QUOTED_STRING>\n	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,"\n");}
<MULTILINE_DOUBLE_QUOTED_STRING,MULTILINE_SINGLE_QUOTED_STRING>\\t	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,"\t");}
<MULTILINE_DOUBLE_QUOTED_STRING,MULTILINE_SINGLE_QUOTED_STRING>\"|(\\\")	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,"\"");}
<MULTILINE_DOUBLE_QUOTED_STRING,MULTILINE_SINGLE_QUOTED_STRING>\\\\	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,"\\");}
<MULTILINE_DOUBLE_QUOTED_STRING>(\"){3}	{charIndex=findLexemLocation(charIndex,stringIndex); /*printf("FLEX:\tВывел многострочный текст через двойную кавычку: %s\n", bufStr);*/	fprintf(logFile,"FLEX:\tВывел многострочный текст через двойную кавычку: %s\n", bufStr); yylval.StringVal = (char*)malloc(strlen(bufStr)+1); strcpy(yylval.StringVal,bufStr); strcpy(bufStr,""); BEGIN(INITIAL); return STRING;}
<MULTILINE_DOUBLE_QUOTED_STRING,MULTILINE_SINGLE_QUOTED_STRING><<EOF>>	{printf("FLEX:\tНашел незакрытую кавычку. Конец парсинга.\n");	fprintf(logFile,"FLEX:\tНашел незакрытую кавычку. Конец парсинга.\n"); return -1;}
(\'){3}				{charIndex=findLexemLocation(charIndex,stringIndex); /*printf("Нашел многострочный текст через одинарную кавычку\n");	fprintf(logFile,"Нашел многострочный текст через одинарную кавычку\n"); strcpy(bufStr,"");*/ BEGIN(MULTILINE_SINGLE_QUOTED_STRING);}
<MULTILINE_SINGLE_QUOTED_STRING>(\'){3}	{charIndex=findLexemLocation(charIndex,stringIndex); /*printf("FLEX:\tВывел многострочный текст через одинарную кавычку: %s\n", bufStr);*/	fprintf(logFile,"FLEX:\tВывел многострочный текст через одинарную кавычку: %s\n", bufStr); yylval.StringVal = (char*)malloc(strlen(bufStr)+1); strcpy(yylval.StringVal,bufStr); strcpy(bufStr,""); BEGIN(INITIAL); return STRING;}
\'				{charIndex=findLexemLocation(charIndex,stringIndex); /*printf("Нашел одинарную кавычку\n");	fprintf(logFile,"Нашел одинарную кавычку\n"); strcpy(bufStr,"");*/ BEGIN(SINGLE_QUOTED_STRING);}
<SINGLE_QUOTED_STRING>\"|(\\\")	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,"\"");}
<SINGLE_QUOTED_STRING>\\\'	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,"\'");}
<SINGLE_QUOTED_STRING>\'	{charIndex=findLexemLocation(charIndex,stringIndex); /*printf("FLEX:\tВывел строку в одинарных кавычках: %s\n", bufStr);*/	fprintf(logFile,"FLEX:\tВывел строку в одинарных кавычках: %s\n", bufStr); yylval.StringVal = (char*)malloc(strlen(bufStr)+1); strcpy(yylval.StringVal,bufStr); strcpy(bufStr,""); BEGIN(INITIAL); return STRING;}
\"				{charIndex=findLexemLocation(charIndex,stringIndex); /*printf("Нашел кавычку\n");	fprintf(logFile,"Нашел кавычку\n"); strcpy(bufStr,"");*/ BEGIN(DOUBLE_QUOTED_STRING);}
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>[^\\\n\"']+	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,yytext);}
<DOUBLE_QUOTED_STRING>\'|(\\\')	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,"\'");}
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\n	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,"\n");}
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\t	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,"\t");}
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\\	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,"\\");}
<DOUBLE_QUOTED_STRING>\\\"	{charIndex=findLexemLocation(charIndex,stringIndex); strcat(bufStr,"\"");}
<DOUBLE_QUOTED_STRING>\"	{ charIndex=findLexemLocation(charIndex,stringIndex); /*printf("FLEX:\tВывел строку в двойных кавычках: %s\n", bufStr);*/	fprintf(logFile,"FLEX:\tВывел строку в двойных кавычках: %s\n", bufStr); yylval.StringVal = (char*)malloc(strlen(bufStr)+1); strcpy(yylval.StringVal,bufStr); strcpy(bufStr,""); BEGIN(INITIAL); return STRING;}
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING><<EOF>>	{printf("FLEX:\tНашел незакрытую кавычку. Конец парсинга.\n");	fprintf(logFile,"FLEX:\tНашел незакрытую кавычку. Конец парсинга.\n"); return -1;}
<<EOF>> {
			//puts("FLEX:\tНашел конец файла");				
			fprintf(logFile,"FLEX:\tНашел конец файла\n"); 
			if(indent_level!=0)
			{
				dedents_count=indent_level;//(indent_level*INDNT-current_line_indent)/INDNT; 
				indent_level=0;
				/*while(dedents_count>0)
				{
					dedents_count--;
				   /*current_line_indent+=INDNT;*/
				   /*printf("FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");		fprintf(logFile,"FLEX:\tУМЕНЬШЕНИЕ ОТСТУПА\n");
				   return DEDENT;
				}*/
			}
			eof_count=1;
			return EOF_TOKEN;
			
		}
=		{
			charIndex=findLexemLocation(charIndex,stringIndex);
			//printf("FLEX:\t%s - операция\n", yytext);		
			fprintf(logFile,"FLEX:\t%s - операция\n", yytext);	
			return '=';
		}/*([%]=)|([/]{1,2}[=])|([-][=])|([+][=])|([*]{1,2}[=])	{
				charIndex=findLexemLocation(charIndex,stringIndex);
				printf("%s - операция\n", yytext);		
				fprintf(logFile,"%s - операция\n", yytext);
			}*/
True		{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return TTRUE;
			}
False		{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);	
				fprintf(logFile,"%FLEX:\ts - ключевое слово\n", yytext);	
				return FFALSE;
			}
not			{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return NOT;
			}
and			{	
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);		
				return AND;
			}
or			{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
			
				return OR;
			}
while		{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return WHILE;
			} 
for			{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return FOR;
			}
in			{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return IIN;
			}
continue	{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return CONTINUE;
			} 
break		{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return BREAK;
			} 
if			{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return IF;
			} 
elif		{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return ELIF;
			} 
else		{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return ELSE;
			} 
"return"	{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - ключевое слово\n", yytext);		
				fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
				return RETURN;
			}
:	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\t%s - ключевое слово\n", yytext);		
		fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
		return ':';
	}
def	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\t%s - ключевое слово\n", yytext);	
		fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
		return DEF;
	}
del	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\t%s - ключевое слово\n", yytext);	
		fprintf(logFile,"FLEX:\t%s - ключевое слово\n", yytext);	
		return DEL;
	}
\[ 	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\t%s - открывающаяся квадратная скобка\n", yytext);	
		fprintf(logFile,"FLEX:\t%s - открывающаяся квадратная скобка\n", yytext);	
		return '[';
	}
\] 	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\t%s - закрывающаяся квадратная скобка\n", yytext);	
		fprintf(logFile,"FLEX:\t%s - закрывающаяся квадратная скобка\n", yytext);	
		return ']';
	}
\( 	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\t%s - открывающаяся круглая скобка\n", yytext);	
		fprintf(logFile,"FLEX:\t%s - открывающаяся круглая скобка\n", yytext);	
		return '(';
	}
\) 	{
		charIndex=findLexemLocation(charIndex,stringIndex);
		//printf("FLEX:\t%s - закрывающаяся круглая скобка\n", yytext);	
		fprintf(logFile,"FLEX:\t%s - закрывающаяся круглая скобка\n", yytext);	
		return ')';
	}
,			{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - запятая\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - запятая\n", yytext);	
				return ',';
			}
\. 			{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\t%s - точка\n", yytext);	
				fprintf(logFile,"FLEX:\t%s - точка\n", yytext);	
				return '.';
			}
{VAR}		{
				//printf("FLEX:\tНашел идентификатор: %s\n", yytext);	
				fprintf(logFile,"FLEX:\tНашел идентификатор: %s\n", yytext);	
				yylval.StringVal=(char*)memset(malloc(strlen(yytext)+1),'\0',strlen(yytext)+1);	
				strcpy(yylval.StringVal, yytext); 
				charIndex=findLexemLocation(charIndex,stringIndex);
				return OPERAND;
			}
{DIGIT}+((\.{DIGIT}+)|(e\-?{DIGIT}+)|(\.{DIGIT}+e\-?{DIGIT}+)) {
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\tНашел число: %s\n", yytext);			
				fprintf(logFile,"FLEX:\tНашел число: %s\n", yytext); 
				fff=atof(yytext);
				ddd=strtod (yytext,NULL);
				yylval.Float=atof(yytext); 
				if(strchr(yytext,'.')!=NULL&&strchr(yytext,'e')!=NULL)
				{
					strcpy(bufStr,"");
					strncpy(bufStr,yytext,strchr(yytext,'e')-yytext);
					bufStr[strchr(yytext,'e')-yytext]='\0';
					yylval.Float=atof(bufStr); 
					fff=atof(strchr(bufStr,'.')+1);
					step=strlen(bufStr)-(strchr(bufStr,'.')-bufStr)-1;
					while(step!=0)
					{
						fff/=10;
						step--;
					}
					yylval.Float+=fff;
					strcpy(bufStr,"");
					strcpy(bufStr,strchr(yytext,'e')+1);
					step=atoi(bufStr);
					while(step!=0)
					{
						if(step<0)
						{
							yylval.Float/=10;
							step++;
						}
						else if(step>0)
						{
							yylval.Float*=10;
							step--;
						}
						
					}
					//bufStr[strchr(yytext,'e')-yytext]='\0';

				}
				else if(strchr(yytext,'.')!=NULL&&strchr(yytext,'e')==NULL)
				{
					yylval.Float=atof(yytext);
					fff=atof(strchr(yytext,'.')+1);
					step=strlen(yytext)-(strchr(yytext,'.')-yytext)-1;
					while(step!=0)
					{
						fff/=10;
						step--;
					}
					yylval.Float+=fff;
				}
				
				return FFLOAT;	
			}
{DIGIT}+	{
				charIndex=findLexemLocation(charIndex,stringIndex);
				//printf("FLEX:\tНашел число: %s\n", yytext);			
				fprintf(logFile,"FLEX:\tНашел число: %s\n", yytext); 
				yylval.Int=atoi(yytext); 
				return NUMBER;
			}
{WHITE_SPACE}+	{charIndex=findLexemLocation(charIndex,stringIndex); /*puts("Нашел белый разделитель");		fprintf(logFile,"Нашел белый разделитель\n");*/}
\n			{
				charIndex=0; 
				stringIndex++;
				//puts("FLEX:\tНовая строка");					
				fprintf(logFile,"FLEX:\tНовая строка\n"); 
				current_line_indent = 0; 
				BEGIN(INDENT_);
				return NEWLINE;
			}
%%

/*void lyyerror(YYLTYPE lexeml, char* s) 
{ 
	printf("Error! Message: %s\nLocation of token, that could led to the error(%d.%d-%d.%d)",s,lexeml.first_line,lexeml.first_column,lexeml.last_line,lexeml.last_column); 
	exit(-1);
}*/

int findLexemLocation(int index1, int index2)
{
	index1++;
	yylloc.first_column=index1;
	index1+=(strlen(yytext)-1);
	yylloc.last_column=index1;
	yylloc.first_line=index2;
	yylloc.last_line=index2;
	return index1;
}

void yyerror(const char* s) {
	printf("Parse error! Message: %s",s);
	// might as well halt now:
	//exit(-1);
}