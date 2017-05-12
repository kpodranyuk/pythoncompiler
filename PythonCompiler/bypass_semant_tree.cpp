#include "bypass_semant_tree.h"

// ����������� �� ���������
TreeTraversal::TreeTraversal()
{
}

// �������� ������ (������ �����)
void TreeTraversal::fixTree(struct StmtListInfo* root) throw(char*)
{
	if(root == NULL)
		throw "Root of tree is a null pointer\n";
	checkStatementList(root);
}

// ����������� ������ (������ �����)
void TreeTraversal::makeTables(const struct StmtListInfo* treeRoot)
{
}

// ������ ������������������ ������ (������ ����� - ?)
void TreeTraversal::printTree(const struct StmtListInfo* treeRoot)
{
}

void TreeTraversal::checkStatementList(struct StmtListInfo* root)
{
	// ���� ������ ������ ������ null - ������� �� �������
	//if(root==NULL) return ;
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

void TreeTraversal::checkExpr(struct ExprInfo * expr)
{
}

void TreeTraversal::checkIfStmt(struct IfStmtInfo * ifstmt)
{
}

void TreeTraversal::checkWhileStmt(struct WhileStmtInfo * whilestmt)
{
	lc_state = _CYCLE_STATE;
	// ��� �������
	lc_state = _REGULAR_STATE;
}

void TreeTraversal::checkForStmt(struct ForStmtInfo * forstmt)
{
	lc_state = _CYCLE_STATE;
	// ��� �������
	lc_state = _REGULAR_STATE;
}

void TreeTraversal::checkFuncDefStmt(struct FuncDefInfo * funcdefstmt)
{
	enum GlobalState lastState = gl_state;
	if(lastState == _MAIN_STATE)
		gl_state = _FUNC_STATE;
	// ��� �������
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