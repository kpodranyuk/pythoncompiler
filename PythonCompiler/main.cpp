#include <stdio.h>
#include <conio.h>
#include <locale.h>
#include <iostream>
#include "tree_structs.h"
#include "parser.tab.h"

extern FILE* yyin;
extern int yyparse();
extern "C" int yylex();

FILE* logFile;
FILE* logFileB;

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
		logFileB = fopen("logB.txt", "wt");
		do{
			yyparse();//yylex();	
		}while(!feof(yyin));
		fclose(yyin);
		fclose(logFile);
		fclose(logFileB);
		//printf("result is %d", root);
	/*}*/
	_getch();
	return 0;
}