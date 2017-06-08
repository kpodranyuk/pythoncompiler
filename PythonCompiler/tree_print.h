#include "tree_structs.h"
#include <vector>

#pragma warning(disable : 4996)

/** КРАТКАЯ СПРАВКА ПО ЯЗЫКУ DOT
* Сам граф должен объявляться как: graph <Имя> {тело графа}
* Объявление узла: <номер узла> [label=\"строка с текстом в узле\" shape=форма узла];
* Пример: 1 [label=\"{4}\" shape=box];
* Связи между узлами (если граф не направленный): <номер узла> -- <номер узла>;
* Пример: 1 -- 2;
**/

/*---------	ОПЕРАЦИИ - КВАДРАТНЫЕ ФОРМЫ УЗЛОВ, ОПЕРАНДЫ - КРУГЛЫЕ (по умолчанию без тега shape)	---------*/

void printExpr(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree);
void printIfStmt(struct IfStmtInfo * ifstmt, int* nodeCount, std::vector<std::string>& dotTree);
void printWhileStmt(struct WhileStmtInfo * whilestmt, int* nodeCount, std::vector<std::string>& dotTree);
void printForStmt(struct ForStmtInfo * forstmt, int* nodeCount, std::vector<std::string>& dotTree);
void printFuncDefStmt(struct FuncDefInfo * funcdefstmt, int* nodeCount, std::vector<std::string>& dotTree);
void printContinueBreakStmt(enum StmtType type, int* nodeCount, std::vector<std::string>& dotTree);
void printReturnStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree);
void printDelStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree);
void printFuncParams(struct DefFuncParamListInfo* params,int* nodeCount,std::vector<std::string>& dotTree);

char* makeStringForDot(char* string);
char* makeExprNodename(enum ExprType type, char * idName);
void printVarVal(struct ValInfo * val, int* nodeCount, std::vector<std::string>& dotTree);
void printExprList(struct ExprListInfo * exprlist, int* nodeCount, std::vector<std::string>& dotTree);
void printFuncCall(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree);
void addDeclStringToStringList(char* string, int declNumber, std::vector<std::string>& dotTree);
void addLinkToStringList(int firstNode, int secondNode, std::vector<std::string>& dotTree);
/* Обойти дерево и "перевести" для печати на язык dot
* @author Kate
* \param[in] root список корней дерева
* \param[in|out] nodeCount текущий индекс узла
* \param[in|out] dotTree список строк для хранения кода на dot
*/
void printStatementList(struct StmtListInfo* root, int* nodeCount, std::vector<std::string>& dotTree)
{
	// Если список корней дерева null - выходим из функции
	if(root==NULL) return ;
	// Иначе
	// Создаем локальный указатель на элемент входного дерева
	struct StmtInfo* begining;
	// Считаем первый элемент списка начальным
	begining = root->first;
	int node1, node2; // Номер главного узла и номер дочернего узла
	*nodeCount+=1; // Получить номер узла
	std::string nodeDec;
	// Запомнить номер текущего "родительского" узла
	node1=*nodeCount;
	if(node1==1)
		addDeclStringToStringList("[label=\"MAINBLOCK\" shape=invtriangle];",node1,dotTree);
	else
		addDeclStringToStringList("[label=\"CODEBLOCK\" shape=trapezium];",node1,dotTree);
	// Пока текущий элемент списка не последний..
	while(begining!=NULL)
	{
		// В зависимости от типа узла/элемента вызываем соответствующую функцию
		// Запомнить номер текущего дочернего узла
		node2=*nodeCount+1; 
		if(begining->type==_EXPR)
		{
			printExpr(begining->expr,nodeCount,dotTree);
		}
		else if(begining->type==_IF)
		{
			printIfStmt(begining->ifstmt,nodeCount,dotTree);
		}
		else if(begining->type==_FOR)
		{
			printForStmt(begining->forstmt,nodeCount,dotTree);
		}
		else if(begining->type==_WHILE)
		{
			printWhileStmt(begining->whilestmt,nodeCount,dotTree);
		}
		else if(begining->type==_FUNC_DEF)
		{
			printFuncDefStmt(begining->funcdefstmt,nodeCount,dotTree);
		}
		else if(begining->type==_RETURN)
		{
			printReturnStmt(begining->expr,nodeCount,dotTree);
		}
		else if(begining->type==_BREAK)
		{
			printContinueBreakStmt(_BREAK,nodeCount,dotTree);
		}
		else if(begining->type==_CONTINUE)
		{
			printContinueBreakStmt(_CONTINUE,nodeCount,dotTree);
		}
		else if(begining->type==_DEL)
		{
			printDelStmt(begining->expr,nodeCount,dotTree);
		}
		else
		{
			return;
		}
		// Добавить в список связь между дочерним и родительским узлами
		addLinkToStringList(node1,node2,dotTree);
		// Считаем следующий элемент списка новым текущим
		begining = begining->next;
	}
}


void printExpr(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // Номер главного узла и номер дочернего узла
	*nodeCount+=1; // Получить номер узла
	char* curNode;
	std::string nodeDec;
	// В зависимости от типа узла/элемента вызываем соответствующую функцию
	if(expr->type==_NOT)
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"NOT\" shape=box];",node1,dotTree);
		// Запомнить номер текущего дочернего узла
		node2=*nodeCount+1; 
		printExpr(expr->left,nodeCount,dotTree);
		// Добавить в список связь между дочерним и родительским узлами
		addLinkToStringList(node1,node2,dotTree);
	}
	else if(expr->type==_OPERAND)
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		curNode = new char [50+strlen(expr->idName)+1];
		sprintf(curNode,"[label=\"<ID>\n%s\"];",expr->idName);
		addDeclStringToStringList(curNode,node1,dotTree);
	}
	else if(expr->type==_UMINUS)
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"UMINUS\" shape=box];",node1,dotTree);
		// Запомнить номер текущего дочернего узла
		node2=*nodeCount+1; 
		printExpr(expr->left,nodeCount,dotTree);
		// Добавить в список связь между дочерним и родительским узлами
		addLinkToStringList(node1,node2,dotTree);
	}
	else if(expr->type==_VARVAL)
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount-1;
		//addDeclStringToStringList("[label=\"VARVAL\"];",node1,dotTree);
		// Запомнить номер текущего дочернего узла
		node2=*nodeCount; 
		printVarVal(expr->exprVal,nodeCount,dotTree);
		// Добавить в список связь между дочерним и родительским узлами
		//addLinkToStringList(node1,node2,dotTree);
	}
	else if(expr->type==_FUNCCALL)
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"FUNC_CALL\"];",node1,dotTree);
		// ВЫЗОВ
		printFuncCall(expr,nodeCount,dotTree);
	}
	else if(expr->type==_ARRINIT)
	{
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"INITIALIZATION_LIST\"];",node1,dotTree);
		printExprList(expr->arglist,nodeCount,dotTree);
	}
	else
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		addDeclStringToStringList(makeExprNodename(expr->type, expr->idName),node1,dotTree);
		// Запомнить номер текущего дочернего узла
		node2=*nodeCount+1; 
		printExpr(expr->left,nodeCount,dotTree);
		// Добавить в список связь между дочерним и родительским узлами
		addLinkToStringList(node1,node2,dotTree);
		if(expr->type==_ARRID_AND_ASSIGN)
		{
			// Запомнить номер текущего дочернего узла
			node2=*nodeCount+1; 
			printExpr(expr->middle,nodeCount,dotTree);
			// Добавить в список связь между дочерним и родительским узлами
			addLinkToStringList(node1,node2,dotTree);
		}
		// Запомнить номер текущего дочернего узла
		node2=*nodeCount+1; 
		printExpr(expr->right,nodeCount,dotTree);
		// Добавить в список связь между дочерним и родительским узлами
		addLinkToStringList(node1,node2,dotTree);
	}
}

void printVarVal(struct ValInfo * val, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1; // Номер главного узла и номер дочернего узла
	//*nodeCount+=1; // Получить номер узла
	char* curNode;
	// Запомнить номер текущего "родительского" узла
	node1=*nodeCount;
	// В зависимости от типа узла/элемента вызываем соответствующую функцию
	if(val->type==_TRUE)
	{
		addDeclStringToStringList("[label=\"VARVAL\ntrue\"];",node1,dotTree);
	}
	else if(val->type==_FALSE)
	{
		addDeclStringToStringList("[label=\"VARVAL\nfalse\"];",node1,dotTree);
	}
	else if(val->type==_NONE)
	{
		addDeclStringToStringList("[label=\"VARVAL\nNONE\"];",node1,dotTree);
	}
	else if(val->type==_STRING)
	{
		char* buf = makeStringForDot(val->stringVal);
		curNode = new char [50+strlen(buf)+1];
		sprintf(curNode,"[label=\"VARVAL\n%s\"];",buf);
		addDeclStringToStringList(curNode,node1,dotTree);
	}
	else if(val->type==_NUMBER)
	{
		curNode = new char [50];
		sprintf(curNode,"[label=\"VARVAL\n%d\"];",val->intVal);
		addDeclStringToStringList(curNode,node1,dotTree);
	}
	else
		return ;
}

char* makeStringForDot(char* string)
{
	char* finalString = new char [strlen(string)*2+1];
	finalString[0]='\0';
	int index = 0;
	int finalLength = 0;
	while(index<(int)strlen(string))
	{
		if(string[index]=='\"')
		{
			if(strlen(finalString)==0)
				strcpy(finalString,"\\\"");
			else
				strcat(finalString,"\\\"");
			
		}
		else
		{
			
			finalString[finalLength]=string[index];
			finalString[finalLength+1]='\0';
		}
		finalLength = strlen(finalString);
		index++;
	}
	return finalString;
}

void printExprList(struct ExprListInfo * exprlist, int* nodeCount, std::vector<std::string>& dotTree)
{
	// Создаем ExprInfo указатель на элемент входного дерева
	struct ExprInfo* begining;
	// Считаем первый элемент списка начальным
	begining = exprlist->first;
	int node1, node2; // Номер главного узла и номер дочернего узла
	//*nodeCount+=1; // Получить номер узла
	std::string nodeDec;
	// Запомнить номер текущего "родительского" узла
	node1=*nodeCount;
	// Пока текущий элемент списка не последний..
	while(begining!=NULL)
	{
		// Запомнить номер текущего дочернего узла
		node2=*nodeCount+1; 
		printExpr(begining,nodeCount,dotTree);
		// Добавить в список связь между дочерним и родительским узлами
		addLinkToStringList(node1,node2,dotTree);
		// Считаем следующий элемент списка новым текущим
		begining = begining->next;
	}
}

void printIfStmt(struct IfStmtInfo * ifstmt, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // Номер главного узла и номер дочернего узла
	*nodeCount+=1; // Получить номер узла if_stmt
	int if_stmt=*nodeCount;

	node1=*nodeCount;
	addDeclStringToStringList("[label=\"IF_STMT\"];",if_stmt,dotTree);

	// Вывод условного выражения
	*nodeCount+=1;
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"CONDITION\"];",node1,dotTree);
	addLinkToStringList(if_stmt,node1,dotTree);
	node2=*nodeCount+1;
	printExpr(ifstmt->expr,nodeCount,dotTree);
	addLinkToStringList(node1,node2,dotTree);

	// Вывод тела условия
	node1=if_stmt;
	node2=*nodeCount+1;
	printStatementList(ifstmt->stmtlist, nodeCount, dotTree);
	addLinkToStringList(node1,node2,dotTree);

	// Вывод elif-ов
	struct ElifListInfo* begin=ifstmt->eliflist;
	while(begin!=NULL)
	{
		*nodeCount+=1;
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"ELIF\"];",node1,dotTree);
		addLinkToStringList(if_stmt,node1,dotTree);
		int elif_stmt=node1;

		// Вывод условного выражения
		*nodeCount+=1;
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"CONDITION\"];",node1,dotTree);
		addLinkToStringList(elif_stmt,node1,dotTree);
		node2=*nodeCount+1;
		printExpr(begin->expr,nodeCount,dotTree);
		addLinkToStringList(node1,node2,dotTree);

		// Вывод тела
		node1=elif_stmt;
		node2=*nodeCount+1;
		printStatementList(begin->stmtlist, nodeCount, dotTree);
		addLinkToStringList(node1,node2,dotTree);

		begin=begin->next;
	}

	// Вывод блока else
	if(ifstmt->elsestmtlist!=NULL)
	{
		*nodeCount+=1;
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"ELSE BLOCK\"];",node1,dotTree);
		addLinkToStringList(if_stmt,node1,dotTree);
		node2=*nodeCount+1;
		printStatementList(ifstmt->elsestmtlist, nodeCount, dotTree);
		addLinkToStringList(node1,node2,dotTree);
	}
}


void printWhileStmt(struct WhileStmtInfo * whilestmt, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // Номер главного узла и номер дочернего узла
	*nodeCount+=1; // Получить номер узла while_stmt
	int while_stmt=*nodeCount;
	
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"WHILE_STMT\"];",while_stmt,dotTree);

	// Вывод условия продолжения цикла
	*nodeCount+=1;
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"CONDITION\"];",node1,dotTree);
	addLinkToStringList(while_stmt,node1,dotTree);
	node2=*nodeCount+1;
	printExpr(whilestmt->expr,nodeCount,dotTree);
	addLinkToStringList(node1,node2,dotTree);

	// Вывод тела цикла
	node1=while_stmt;
	node2=*nodeCount+1;
	printStatementList(whilestmt->stmtlist, nodeCount, dotTree);
	addLinkToStringList(node1,node2,dotTree);

	// Вывод блока else
	if(whilestmt->elsestmt!=NULL)
	{
		*nodeCount+=1;
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"ELSE BLOCK\"];",node1,dotTree);
		addLinkToStringList(while_stmt,node1,dotTree);
		node2=*nodeCount+1;
		printStatementList(whilestmt->elsestmt, nodeCount, dotTree);
		addLinkToStringList(node1,node2,dotTree);
	}
}

void printForStmt(struct ForStmtInfo * forstmt, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // Номер главного узла и номер дочернего узла
	*nodeCount+=1; // Получить номер узла for_stmt
	int for_stmt=*nodeCount;

	node1=*nodeCount;
	addDeclStringToStringList("[label=\"FOR_STMT\"];",for_stmt,dotTree);

	// Вывод счетчика цикла
	*nodeCount+=1;
	node1=*nodeCount;
	char* curNode = new char [50+strlen(forstmt->counter->idName)+1];
	sprintf(curNode,"[label=\"<COUNTER_ID>\n%s\"];",forstmt->counter->idName);
	addDeclStringToStringList(curNode,node1,dotTree);
	addLinkToStringList(for_stmt, node1, dotTree);

	// Вывод выражения, по чем проходит цикл
	*nodeCount+=1;
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"BYPASS_EXPR\"];",node1,dotTree);
	addLinkToStringList(for_stmt,node1,dotTree);
	node2=*nodeCount+1;
	printExpr(forstmt->expr,nodeCount,dotTree);
	addLinkToStringList(node1,node2,dotTree);

	// Вывод тела цикла
	node1=for_stmt;
	node2=*nodeCount+1;
	printStatementList(forstmt->stmtlist, nodeCount, dotTree);
	addLinkToStringList(node1,node2,dotTree);

	// Вывод блока else
	if(forstmt->elsestmt!=NULL)
	{
		*nodeCount+=1;
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"ELSE BLOCK\"];",node1,dotTree);
		addLinkToStringList(for_stmt,node1,dotTree);
		node2=*nodeCount+1;
		printStatementList(forstmt->elsestmt, nodeCount, dotTree);
		addLinkToStringList(node1,node2,dotTree);
	}
}

void printFuncCall(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
	char* curNode;
	int parent = (*nodeCount)++;
	int name = (*nodeCount)++;
	int args, node2;
	curNode = new char[30+strlen(expr->idName)];
	sprintf(curNode,"[label=\"FUNC\n%s\"];",expr->idName);	
	addDeclStringToStringList(curNode,name,dotTree);
	// Добавить в список связь между дочерним и родительским узлами
	addLinkToStringList(parent,name,dotTree);
	// Запомнить номер текущего дочернего узла
	args = (*nodeCount);
	if(expr->arglist->first==NULL&&expr->arglist->last==NULL)
	{
		addDeclStringToStringList("[label=\"NO_ARGS\"];",args,dotTree);
	}
	else
	{
		addDeclStringToStringList("[label=\"ARGS\"];",args,dotTree);
		// Запомнить номер текущего дочернего узла
		node2=(*nodeCount)+1; 
		printExprList(expr->arglist,nodeCount,dotTree);
	}
		addLinkToStringList(parent,args,dotTree);
}

void printFuncDefStmt(struct FuncDefInfo * funcdefstmt, int* nodeCount, std::vector<std::string>& dotTree)
{
	char* curNode;
	int name = ++(*nodeCount);
	int node2;
	// Объявляем узел с именем функции
	curNode = new char[50+strlen(funcdefstmt->functionName)];
	sprintf(curNode,"[label=\"FUNC_DEF\n%s\"];",funcdefstmt->functionName);	
	addDeclStringToStringList(curNode,name,dotTree);
	// Запомнить номер текущего дочернего узла
	node2=*nodeCount+1; 
	printFuncParams(funcdefstmt->params,nodeCount,dotTree);
	// Добавить в список связь между дочерним и родительским узлами
	addLinkToStringList(name,node2,dotTree);
	// Запомнить номер текущего дочернего узла
	node2=*nodeCount+1; 
	printStatementList(funcdefstmt->body,nodeCount,dotTree);
	// Добавить в список связь между дочерним и родительским узлами
	addLinkToStringList(name,node2,dotTree);

}

void printFuncParams(struct DefFuncParamListInfo* params,int* nodeCount,std::vector<std::string>& dotTree)
{
	if(params->first==NULL&&params->last==NULL)
	{
		addDeclStringToStringList("[label=\"NO_PARAMS\"];",++(*nodeCount),dotTree);
		return ;
	}
	char* curNode;
	// Создаем DefFuncParamInfo указатель на элемент входного дерева
	struct DefFuncParamInfo* begining;
	// Считаем первый элемент списка начальным
	begining = params->first;
	int node1, node2, node3; // Номер главного узла и номер дочернего узла
	// Запомнить номер текущего "родительского" узла
	node1=++(*nodeCount);
	addDeclStringToStringList("[label=\"PARAMS\"];",node1,dotTree);
	// Пока текущий элемент списка не последний..
	while(begining!=NULL)
	{
		// Запомнить номер текущего дочернего узла
		node2=++(*nodeCount); 
		curNode = new char[30+strlen(begining->paramName)];
		sprintf(curNode,"[label=\"%s\"];",begining->paramName);	
		addDeclStringToStringList(curNode,node2,dotTree);
		// Добавить в список связь между дочерним и родительским узлами
		addLinkToStringList(node1,node2,dotTree);
		
		if(begining->paramVal!=NULL)
		{
			node3=node2;
			node2=*nodeCount+1; 
			printVarVal(begining->paramVal,nodeCount,dotTree);
			// Добавить в список связь между дочерним и родительским узлами
			addLinkToStringList(node3,node2,dotTree);
		}

		// Считаем следующий элемент списка новым текущим
		begining = begining->next;
	}
}

void printContinueBreakStmt(enum StmtType type, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1; // Номер главного узла и номер дочернего узла
	*nodeCount+=1;
	node1=*nodeCount;

	if(type==_BREAK)
	{
		addDeclStringToStringList("[label=\"BREAK\" shape=box];",node1,dotTree);
	}
	else if(type==_CONTINUE)
	{
		addDeclStringToStringList("[label=\"CONTINUE\" shape=box];",node1,dotTree);
	}
}

void printReturnStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // Номер главного узла и номер дочернего узла
	*nodeCount+=1; // Получить номер узла
	std::string nodeDec;
	
	// Запомнить номер текущего "родительского" узла
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"RETURN\" shape=box];",node1,dotTree);
	// Запомнить номер текущего дочернего узла
	node2=*nodeCount+1; 
	printExpr(expr,nodeCount,dotTree);
	// Добавить в список связь между дочерним и родительским узлами
	addLinkToStringList(node1,node2,dotTree);
}

void printDelStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // Номер главного узла и номер дочернего узла
	*nodeCount+=1; // Получить номер узла
	std::string nodeDec;
	
	// Запомнить номер текущего "родительского" узла
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"DEL\" shape=box];",node1,dotTree);
	// Запомнить номер текущего дочернего узла
	node2=*nodeCount+1; 
	printExpr(expr,nodeCount,dotTree);
	// Добавить в список связь между дочерним и родительским узлами
	addLinkToStringList(node1,node2,dotTree);
}
/* Добавить в список строк объявление узла на языке dot
* @author Kate
* \param[in] string строка с названием узла и его параметрами
* \param[in] declNumber номер узла
* \param[in|out] dotTree список строк для хранения кода на dot
*/
void addDeclStringToStringList(char* string, int declNumber, std::vector<std::string>& dotTree)
{
	if(declNumber==NULL||string==NULL) return ;
	std::string stringToTree;
	char* intString = new char [20];
	itoa(declNumber,intString,10);
	char* finalString = new char[strlen(intString)+strlen(string)+1+1];
	strcpy(finalString,intString);
	strcat(finalString," ");
	strcat(finalString,string);
	stringToTree = std::string(finalString);
	dotTree.push_back(stringToTree);
	delete intString;
	delete finalString;
}
/* Добавить в список строк связь узлов на языке dot
* @author Kate
* \param[in] firstNode номер первого узла
* \param[in] secondNode номер второго узла
* \param[in|out] dotTree список строк для хранения кода на dot
*/
void addLinkToStringList(int firstNode, int secondNode, std::vector<std::string>& dotTree)
{
	if(firstNode==NULL||secondNode==NULL) return ;
	std::string stringToTree;
	char* intString = new char [20];
	char* finalString = new char[50];
	itoa(firstNode,intString,10);
	strcpy(finalString,intString);
	strcat(finalString," -- ");

	itoa(secondNode,intString,10);

	strcat(finalString,intString);
	strcat(finalString,";");

	stringToTree = std::string(finalString);
	dotTree.push_back(stringToTree);
	delete intString;
	delete finalString;
}

char* makeExprNodename(enum ExprType type, char * idName)
{
	char* decl = new char [100];
	decl[0]='\0';
	if(type==_OR)
	{
		sprintf(decl,"[label=\"OR\" shape=box];");
	}
	else if(type==_AND)
	{
		sprintf(decl,"[label=\"AND\" shape=box];");
	}
	else if(type==_NOT_EQUAL)
	{
		sprintf(decl,"[label=\"!=\" shape=box];");
	}
	else if(type==_EQUAL)
	{
		sprintf(decl,"[label=\"EQUAL\" shape=box];");
	}
	else if(type==_GREATER)
	{
		sprintf(decl,"[label=\">\" shape=box];");
	}
	else if(type==_GREATER_OR_EQUAL)
	{
		sprintf(decl,"[label=\">=\" shape=box];");
	}
	else if(type==_LESS)
	{
		sprintf(decl,"[label=\"<\" shape=box];");
	}
	else if(type==_LESS_OR_EQUAL)
	{
		sprintf(decl,"[label=\"<=\" shape=box];");
	}
	else if(type==_SUB)
	{
		sprintf(decl,"[label=\"-\" shape=box];");
	}
	else if(type==_ADD)
	{
		sprintf(decl,"[label=\"+\" shape=box];");
	}
	else if(type==_INT)
	{
		sprintf(decl,"[label=\"//\" shape=box];");
	}
	else if(type==_MOD)
	{
		sprintf(decl,"[label=\"%%\" shape=box];");
	}
	else if(type==_DIV)
	{
		sprintf(decl,"[label=\"/\" shape=box];");
	}
	else if(type==_MUL)
	{
		sprintf(decl,"[label=\"*\" shape=box];");
	}
	else if(type==_POW)
	{
		sprintf(decl,"[label=\"**\" shape=box];");
	}
	else if(type==_ASSIGN)
	{
		sprintf(decl,"[label=\"=\" shape=box];");
	}
	else if(type==_ARRID)
	{
		sprintf(decl,"[label=\"[]\" shape=box];");
	}
	else if(type==_ARRACT)
	{
		sprintf(decl,"[label=\"%s\" shape=box];", idName);
	}
	else if(type==_ARRID_AND_ASSIGN)
	{
		sprintf(decl,"[label=\"[]=\" shape=box];");
	}
	return decl;
}