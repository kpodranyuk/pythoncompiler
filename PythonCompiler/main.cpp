#include <stdio.h>
#include <conio.h>
#include <locale.h>
#include <iostream>
#include <process.h>
#include "tree_print.h"
#include "bypass_semant_tree.h"
#include "parser.tab.h"

extern FILE* yyin;
extern int yyparse();
extern "C" int yylex();

FILE* logFile;
FILE* logFileB;
extern StmtListInfo* root;

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
		int nodeCount=0;
		std::vector<std::string> dotTree;
		dotTree.push_back("graph tree{");
		printStatementList(root,&nodeCount,dotTree);
		dotTree.push_back("}");
		FILE* dotFile = fopen("dotTree.txt","wt");
		for each (std::string curStr in dotTree)
		{
			fprintf(dotFile,"%s\n",curStr.c_str());
		}
		fclose(dotFile);
		printf("tree was created\n");
		// Ждем печать дерева
		spawnl(_P_WAIT,".\\dot\\dot.exe","dot","-O","-Tpng","dotTree.txt",NULL);
		// Выводим сообщение о том, что дерево напечатано
		printf("tree was printed");
		TreeTraversal* treeWalker = new TreeTraversal();
		try{
			//treeWalker->fixTree(root);
			printf("\nTree fixed successfully\n");
			treeWalker->makeTables(root);
			printf("\nTables made successfully\n");
		}
		catch (char* message)
		{
			printf("\nMessage while traversing the tree: %s\nPlease, check syntax of python file.",message);
		}
	/*}*/
	_getch();
	return 0;
}