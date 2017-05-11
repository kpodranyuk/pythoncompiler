#include "bypass_semant_tree.h"
#include <vector>

/* ������ ������ � "���������" ��� ������ �� ���� dot
* @author Kate
* \param[in] root ������ ������ ������
* \param[in|out] nodeCount ������� ������ ����
* \param[in|out] dotTree ������ ����� ��� �������� ���� �� dot
*/
void printStatementList(struct StmtListInfo* root)
{
	// ���� ������ ������ ������ null - ������� �� �������
	if(root==NULL) return ;
	// �����
	// ������� ��������� ��������� �� ������� �������� ������
	struct StmtInfo* begining;
	// ������� ������ ������� ������ ���������
	begining = root->first;
	// ���� ������� ������� ������ �� ���������..
	while(begining!=NULL)
	{
		// � ����������� �� ���� ����/�������� �������� ��������������� �������
		// ��������� ����� �������� ��������� ����
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
		// ������� ��������� ������� ������ ����� �������
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