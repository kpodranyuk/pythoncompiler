#include "tree_structs.h"
#include <vector>

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
void printFuncDefStmt(struct FuncDefStmtInfo * funcdefstmt, int* nodeCount, std::vector<std::string>& dotTree);
void printContinueBreakStmt(int* nodeCount, std::vector<std::string>& dotTree);
void printReturnStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree);
void printDelStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree);

char* makeExprNodename(enum ExprType type, int node);
void printVarVal(struct ValInfo * val, int* nodeCount, std::vector<std::string>& dotTree);

/* Обойти дерево и "перевести" для печати на язык dot
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
	char* curNode;
	std::string nodeDec;
	// Запомнить номер текущего "родительского" узла
	node1=*nodeCount;
	curNode = new char [50];
	sprintf(curNode,"%d [label=\"MAINBLOCK\" shape=invtriangle];",node1);
	nodeDec = std::string(curNode);
	dotTree.push_back(nodeDec);
	// Пока текущий элемент списка не последний..
	while(begining!=NULL)
	{
		// В зависимости от типа узла/элемента вызываем соответствующую функцию
		if(begining->type==_EXPR)
		{
			// Запомнить номер текущего дочернего узла
			node2=*nodeCount+1; 
			printExpr(begining->expr,nodeCount,dotTree);
			// Добавить в список связь между дочерним и родительским узлами
			curNode[0] = '\0';
			sprintf(curNode,"%d -- %d;",node1,node2);
			nodeDec = std::string(curNode);
			dotTree.push_back(nodeDec);
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
			printContinueBreakStmt(nodeCount,dotTree);
		}
		else if(begining->type==_CONTINUE)
		{
			printContinueBreakStmt(nodeCount,dotTree);
		}
		else if(begining->type==_DEL)
		{
			printDelStmt(begining->expr,nodeCount,dotTree);
		}
		// Считаем следующий элемент списка новым текущим
		begining = begining->next;
	}
}


void printExpr(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // Номер главного узла и номер дочернего узла
	*nodeCount+=1; // Получить номер узла
	/*
	// обращение по индексу массива
	_ARRID,
	// инициализация массива
	_ARRINIT,
	// действия над массивом
	_ARRACT,
	_FUNCCALL
	*/

	char* curNode;
	std::string nodeDec;
	// В зависимости от типа узла/элемента вызываем соответствующую функцию
	if(expr->type==_NOT)
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		curNode = new char [30];
		sprintf(curNode,"%d [label=\"NOT\"];",node1);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
		// Запомнить номер текущего дочернего узла
		node2=*nodeCount+1; 
		printExpr(expr->left,nodeCount,dotTree);
		// Добавить в список связь между дочерним и родительским узлами
		curNode[0] = '\0';
		sprintf(curNode,"%d -- %d;",node1,node2);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
	}
	else if(expr->type==_OPERAND)
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		curNode = new char [30];
		//printExpr(begining->expr,nodeCount,dotTree);
		sprintf(curNode,"%d [label=\"Type = OPERAND\n%s\"];",node1,expr->idName);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
	}
	else if(expr->type==_VARVAL)
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		curNode = new char [30];
		//printExpr(begining->expr,nodeCount,dotTree);
		sprintf(curNode,"%d [label=\"Type = VARVAL\"];",node1);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
		// Запомнить номер текущего дочернего узла
		node2=*nodeCount+1; 
		printVarVal(expr->exprVal,nodeCount,dotTree);
		// Добавить в список связь между дочерним и родительским узлами
		curNode[0] = '\0';
		sprintf(curNode,"%d -- %d;",node1,node2);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);			
	}
	else
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		curNode = new char [30];
		//printExpr(begining->expr,nodeCount,dotTree);
		strcpy(curNode,makeExprNodename(expr->type,node1));
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
		// Запомнить номер текущего дочернего узла
		node2=*nodeCount+1; 
		printExpr(expr->left,nodeCount,dotTree);
		// Добавить в список связь между дочерним и родительским узлами
		curNode[0] = '\0';
		sprintf(curNode,"%d -- %d;",node1,node2);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
		// Запомнить номер текущего дочернего узла
		node2=*nodeCount+1; 
		printExpr(expr->right,nodeCount,dotTree);
		// Добавить в список связь между дочерним и родительским узлами
		curNode[0] = '\0';
		sprintf(curNode,"%d -- %d;",node1,node2);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
		//std::string nodeDec = [label=\"OR\" shape=box];";
	}
}

char* makeExprNodename(enum ExprType type, int node)
{
	char* decl = new char [100];
	decl[0]='\0';
	if(type==_OR)
	{
		sprintf(decl,"%d [label=\"OR\" shape=box];",node);
	}
	else if(type==_AND)
	{
		sprintf(decl,"%d [label=\"AND\" shape=box];",node);
	}
	else if(type==_NOT_EQUAL)
	{
		sprintf(decl,"%d [label=\"!=\" shape=box];",node);
	}
	else if(type==_EQUAL)
	{
		sprintf(decl,"%d [label=\"EQUAL\" shape=box];",node);
	}
	else if(type==_GREATER)
	{
		sprintf(decl,"%d [label=\">\" shape=box];",node);
	}
	else if(type==_GREATER_OR_EQUAL)
	{
		sprintf(decl,"%d [label=\">=\" shape=box];",node);
	}
	else if(type==_LESS)
	{
		sprintf(decl,"%d [label=\"<\" shape=box];",node);
	}
	else if(type==_LESS_OR_EQUAL)
	{
		sprintf(decl,"%d [label=\"<=\" shape=box];",node);
	}
	else if(type==_SUB)
	{
		sprintf(decl,"%d [label=\"-\" shape=box];",node);
	}
	else if(type==_ADD)
	{
		sprintf(decl,"%d [label=\"+\" shape=box];",node);
	}
	else if(type==_INT)
	{
		sprintf(decl,"%d [label=\"//\" shape=box];",node);
	}
	else if(type==_MOD)
	{
		sprintf(decl,"%d [label=\"%\" shape=box];",node);
	}
	else if(type==_DIV)
	{
		sprintf(decl,"%d [label=\"/\" shape=box];",node);
	}
	else if(type==_MUL)
	{
		sprintf(decl,"%d [label=\"*\" shape=box];",node);
	}
	else if(type==_POW)
	{
		sprintf(decl,"%d [label=\"**\" shape=box];",node);
	}
	else if(type==_ASSIGN)
	{
		sprintf(decl,"%d [label=\"=\" shape=box];",node);
	}
	return decl;
}

void printVarVal(struct ValInfo * val, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // Номер главного узла и номер дочернего узла
	*nodeCount+=1; // Получить номер узла
	char* curNode;
	std::string nodeDec;
	// В зависимости от типа узла/элемента вызываем соответствующую функцию
	if(val->type==_TRUE)
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		curNode = new char [30];
		//printExpr(begining->expr,nodeCount,dotTree);
		sprintf(curNode,"%d [label=\"true\"];",node1);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
	}
	else if(val->type==_FALSE)
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		curNode = new char [30];
		//printExpr(begining->expr,nodeCount,dotTree);
		sprintf(curNode,"%d [label=\"false\"];",node1);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
	}
	else if(val->type==_STRING)
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		curNode = new char [30+strlen(val->stringVal)+1];
		//printExpr(begining->expr,nodeCount,dotTree);
		sprintf(curNode,"%d [label=\"%s\"];",node1,val->stringVal);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
	}
	else if(val->type==_NUMBER)
	{
		// Запомнить номер текущего "родительского" узла
		node1=*nodeCount;
		curNode = new char [30];
		//printExpr(begining->expr,nodeCount,dotTree);
		sprintf(curNode,"%d [label=\"%d\"];",node1,val->intVal);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
	}
}

void printIfStmt(struct IfStmtInfo * ifstmt, int* nodeCount, std::vector<std::string>& dotTree)
{
}

void printWhileStmt(struct WhileStmtInfo * whilestmt, int* nodeCount, std::vector<std::string>& dotTree)
{
}

void printForStmt(struct ForStmtInfo * forstmt, int* nodeCount, std::vector<std::string>& dotTree)
{
}

void printFuncDefStmt(struct FuncDefStmtInfo * funcdefstmt, int* nodeCount, std::vector<std::string>& dotTree)
{
}

void printContinueBreakStmt(int* nodeCount, std::vector<std::string>& dotTree)
{
}

void printReturnStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
}

void printDelStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
}