#include "tree_structs.h"
#include <vector>

/** ������� ������� �� ����� DOT
* ��� ���� ������ ����������� ���: graph <���> {���� �����}
* ���������� ����: <����� ����> [label=\"������ � ������� � ����\" shape=����� ����];
* ������: 1 [label=\"{4}\" shape=box];
* ����� ����� ������ (���� ���� �� ������������): <����� ����> -- <����� ����>;
* ������: 1 -- 2;
**/

/*---------	�������� - ���������� ����� �����, �������� - ������� (�� ��������� ��� ���� shape)	---------*/

void printExpr(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree);
void printIfStmt(struct IfStmtInfo * ifstmt, int* nodeCount, std::vector<std::string>& dotTree);
void printWhileStmt(struct WhileStmtInfo * whilestmt, int* nodeCount, std::vector<std::string>& dotTree);
void printForStmt(struct ForStmtInfo * forstmt, int* nodeCount, std::vector<std::string>& dotTree);
void printFuncDefStmt(struct FuncDefStmtInfo * funcdefstmt, int* nodeCount, std::vector<std::string>& dotTree);
void printContinueBreakStmt(int* nodeCount, std::vector<std::string>& dotTree);
void printReturnStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree);
void printDelStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree);

/* ������ ������ � "���������" ��� ������ �� ���� dot
* \param[in] root ������ ������ ������
* \param[in|out] nodeCount ������� ������ ����
* \param[in|out] dotTree ������ ����� ��� �������� ���� �� dot
*/
void printStatementList(struct StmtListInfo* root, int* nodeCount, std::vector<std::string>& dotTree)
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
		// ������� ��������� ������� ������ ����� �������
		begining = begining->next;
	}
}


void printExpr(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ����
	/*
	_OR,
	_AND,
	_NOT,
	_NOT_EQUAL,
	_EQUAL,
	_GREATER,
	_GREATER_OR_EQUAL,
	_LESS,
	_LESS_OR_EQUAL,
	_SUB,
	_ADD,
	_INT,
	_MOD,
	_DIV,
	_MUL,
	_POW,
	_ASSIGN,
	// ��������� �� ������� �������
	_ARRID,
	// ������������� �������
	_ARRINIT,
	// �������� ��� ��������
	_ARRACT,
	_VARVAL,	
	_OPERAND,
	_FUNCCALL
	*/

	char* curNode;
	std::string nodeDec;
	// � ����������� �� ���� ����/�������� �������� ��������������� �������
		if(expr->type==_OR)
		{
			// ��������� ����� �������� "�������������" ����
			node1=*nodeCount;
			curNode = new char [30];
			//printExpr(begining->expr,nodeCount,dotTree);
			sprintf(curNode,"%d [label=\"OR\" shape=box];",node1);
			nodeDec = std::string(curNode);
			dotTree.push_back(nodeDec);
			// ��������� ����� �������� ��������� ����
			node2=*nodeCount+1; 
			printExpr(expr->left,nodeCount,dotTree);
			// �������� � ������ ����� ����� �������� � ������������ ������
			curNode[0] = '\0';
			sprintf(curNode,"%d -- %d;",node1,node2);
			nodeDec = std::string(curNode);
			dotTree.push_back(nodeDec);
			// ��������� ����� �������� ��������� ����
			node2=*nodeCount+1; 
			printExpr(expr->right,nodeCount,dotTree);
			// �������� � ������ ����� ����� �������� � ������������ ������
			curNode[0] = '\0';
			sprintf(curNode,"%d -- %d;",node1,node2);
			nodeDec = std::string(curNode);
			dotTree.push_back(nodeDec);
			//std::string nodeDec = [label=\"OR\" shape=box];";
		}
		else if(expr->type==_OPERAND)
		{
			// ��������� ����� �������� "�������������" ����
			node1=*nodeCount;
			curNode = new char [30];
			//printExpr(begining->expr,nodeCount,dotTree);
			sprintf(curNode,"%d [label=\"Type = OPERAND\n%s\"];",node1,expr->idName);
			nodeDec = std::string(curNode);
			dotTree.push_back(nodeDec);
		}
		/*else if(expr->type==_FOR)
		{
			printForStmt(begining->forstmt,nodeCount,dotTree);
		}
		else if(expr->type==_WHILE)
		{
			printWhileStmt(begining->whilestmt,nodeCount,dotTree);
		}
		else if(expr->type==_FUNC_DEF)
		{
			printFuncDefStmt(begining->funcdefstmt,nodeCount,dotTree);
		}
		else if(expr->type==_RETURN)
		{
			printReturnStmt(begining->expr,nodeCount,dotTree);
		}
		else if(expr->type==_BREAK)
		{
			printContinueBreakStmt(nodeCount,dotTree);
		}
		else if(expr->type==_CONTINUE)
		{
			printContinueBreakStmt(nodeCount,dotTree);
		}
		else if(expr->type==_DEL)
		{
			printDelStmt(begining->expr,nodeCount,dotTree);
		}*/
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