#include "bypass_semant_tree.h"
#include <vector>

/* Обойти дерево и "перевести" для печати на язык dot
* @author Kate
* \param[in] root список корней дерева
* \param[in|out] nodeCount текущий индекс узла
* \param[in|out] dotTree список строк для хранения кода на dot
*/
void printStatementList(struct StmtListInfo* root)
{
	// Если список корней дерева null - выходим из функции
	if(root==NULL) return ;
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

void checkExpr(struct ExprInfo * expr)
{
}

void checkIfStmt(struct IfStmtInfo * ifstmt)
{
}

void checkWhileStmt(struct WhileStmtInfo * whilestmt)
{
}

void checkForStmt(struct ForStmtInfo * forstmt)
{
}

void checkFuncDefStmt(struct FuncDefInfo * funcdefstmt)
{
}

void checkContinueBreakStmt(enum StmtType type)
{
}

void checkReturnStmt(struct ExprInfo * expr)
{
}

void checkDelStmt(struct ExprInfo * expr)
{
}

void checkFuncParams(struct DefFuncParamListInfo* params)
{
}