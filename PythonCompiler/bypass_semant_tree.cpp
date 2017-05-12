#include "bypass_semant_tree.h"

// Конструктор по умолчанию
TreeTraversal::TreeTraversal()
{
}

// Проверка дерева (первый обход)
void TreeTraversal::fixTree(struct StmtListInfo* root) throw(char*)
{
	if(root == NULL)
		throw "Root of tree is a null pointer\n";
	checkStatementList(root);
}

// Составление таблиц (второй обход)
void TreeTraversal::makeTables(const struct StmtListInfo* treeRoot)
{
}

// Печать аттрибутированного дерева (третий обход - ?)
void TreeTraversal::printTree(const struct StmtListInfo* treeRoot)
{
}

void TreeTraversal::checkStatementList(struct StmtListInfo* root)
{
	// Если список корней дерева null - выходим из функции
	//if(root==NULL) return ;
	// Иначе
	// Создаем локальный указатель на элемент входного дерева
	struct StmtInfo* begining;
	// Считаем первый элемент списка начальным
	begining = root->first;
	// Пока текущий элемент списка не последний..
	while(begining!=NULL)
	{
		// В зависимости от типа узла/элемента вызываем соответствующую функцию
		// Запомнить номер текущего дочернего узла
		if(begining->type==_EXPR)
		{
			checkExpr(begining->expr);
		}
		else if(begining->type==_IF)
		{
			checkIfStmt(begining->ifstmt);
		}
		else if(begining->type==_FOR)
		{
			checkForStmt(begining->forstmt);
		}
		else if(begining->type==_WHILE)
		{
			checkWhileStmt(begining->whilestmt);
		}
		else if(begining->type==_FUNC_DEF)
		{
			checkFuncDefStmt(begining->funcdefstmt);
		}
		else if(begining->type==_RETURN)
		{
			checkReturnStmt(begining->expr);
		}
		else if(begining->type==_BREAK || begining->type==_CONTINUE)
		{
			//printContinueBreakStmt(_BREAK,nodeCount,dotTree);
		}
		else if(begining->type==_DEL)
		{
			checkDelStmt(begining->expr);
		}
		else
		{
			return;
		}
		// Считаем следующий элемент списка новым текущим
		begining = begining->next;
	}
}

void TreeTraversal::checkExpr(struct ExprInfo * expr)
{
}

void TreeTraversal::checkIfStmt(struct IfStmtInfo * ifstmt)
{
}

void TreeTraversal::checkWhileStmt(struct WhileStmtInfo * whilestmt)
{
	lc_state = _CYCLE_STATE;
	// Код функции
	lc_state = _REGULAR_STATE;
}

void TreeTraversal::checkForStmt(struct ForStmtInfo * forstmt)
{
	lc_state = _CYCLE_STATE;
	// Код функции
	lc_state = _REGULAR_STATE;
}

void TreeTraversal::checkFuncDefStmt(struct FuncDefInfo * funcdefstmt)
{
	enum GlobalState lastState = gl_state;
	if(lastState == _MAIN_STATE)
		gl_state = _FUNC_STATE;
	// Код функции
	if(lastState == _MAIN_STATE)
		gl_state = _MAIN_STATE;
}

void TreeTraversal::checkContinueBreakStmt(enum StmtType type)
{
}

void TreeTraversal::checkReturnStmt(struct ExprInfo * expr)
{
}

void TreeTraversal::checkDelStmt(struct ExprInfo * expr)
{
}

void TreeTraversal::checkFuncParams(struct DefFuncParamListInfo* params)
{
}