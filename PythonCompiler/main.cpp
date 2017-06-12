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
		printf("������������ ���������� ��� ������ ���������");
		return -1;
	}
	else if(argc>3)
	{
		printf("������� ����� ���������� ��� ������ ���������");
		return -1;
	}
	else
	{*/
		// set flex to read from it instead of defaulting to STDIN:
		yyin = fopen(argv[1], "rt");//("test1.py","rt");//
		if(yyin==NULL)
		{
			//printf("������! ���������� ������� ���� %s �� ������!", argv[1]);
			printf("������! ���������� ������� ���� �� ������!");
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
		printStatementList(root,&nodeCount,dotTree,false);
		dotTree.push_back("}");
		FILE* dotFile = fopen("dotTree.txt","wt");
		for each (std::string curStr in dotTree)
		{
			fprintf(dotFile,"%s\n",curStr.c_str());
		}
		fclose(dotFile);
		puts("tree was created");
		// ���� ������ ������
		spawnl(_P_WAIT,".\\dot\\dot.exe","dot","-O","-Tpng","dotTree.txt",NULL);
		// ������� ��������� � ���, ��� ������ ����������
		puts("tree was printed");
		TreeTraversal* treeWalker = new TreeTraversal();
		try{
			treeWalker->fixTree(root);
			puts("Tree fixed successfully");
			treeWalker->makeTables(root);
			puts("Tables made successfully");
		}
		catch (char* message)
		{
			printf("\nMessage while traversing the tree: %s\nPlease, check syntax of python file.",message);
			_getch();
			return 0;
		}

		/*����� ������������� ������*/
		nodeCount=0;
		dotTree.clear();
		dotTree.push_back("graph tree{");
		printStatementList(root,&nodeCount,dotTree,true);
		dotTree.push_back("}");
		FILE* dotFileChange = fopen("dotTreeChanged.txt","wt");
		for each (std::string curStr in dotTree)
		{
			fprintf(dotFileChange,"%s\n",curStr.c_str());
		}
		fclose(dotFileChange);
		puts("\ntree changed was created");
		// ���� ������ ������
		spawnl(_P_WAIT,".\\dot\\dot.exe","dot","-O","-Tpng","dotTreeChanged.txt",NULL);
		// ������� ��������� � ���, ��� ������ ����������
		puts("tree changed was printed");
	/*}*/
	_getch();
	return 0;
}