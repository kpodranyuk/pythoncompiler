#include "tree_structs.h"
#include <vector>

#pragma warning(disable : 4996)

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

char* makeExprNodename(enum ExprType type, int node);
void printVarVal(struct ValInfo * val, int* nodeCount, std::vector<std::string>& dotTree);

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
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ����
	char* curNode;
	std::string nodeDec;
	// ��������� ����� �������� "�������������" ����
	node1=*nodeCount;
	curNode = new char [50];
	sprintf(curNode,"%d [label=\"MAINBLOCK\" shape=invtriangle];",node1);
	nodeDec = std::string(curNode);
	dotTree.push_back(nodeDec);
	// ���� ������� ������� ������ �� ���������..
	while(begining!=NULL)
	{
		// � ����������� �� ���� ����/�������� �������� ��������������� �������
		// ��������� ����� �������� ��������� ����
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
		else
		{
			return;
		}
		// �������� � ������ ����� ����� �������� � ������������ ������
		curNode[0] = '\0';
		sprintf(curNode,"%d -- %d;",node1,node2);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
		// ������� ��������� ������� ������ ����� �������
		begining = begining->next;
	}
}


void printExpr(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ����
	/*
	// ��������� �� ������� �������
	_ARRID,
	// ������������� �������
	_ARRINIT,
	// �������� ��� ��������
	_ARRACT,
	_FUNCCALL
	*/
	char* curNode;
	std::string nodeDec;
	// � ����������� �� ���� ����/�������� �������� ��������������� �������
	if(expr->type==_NOT)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		curNode = new char [30];
		sprintf(curNode,"%d [label=\"NOT\" shape=box];",node1);
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
	}
	else if(expr->type==_OPERAND)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		curNode = new char [30];
		sprintf(curNode,"%d [label=\"Type = OPERAND\n%s\"];",node1,expr->idName);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
	}
	else if(expr->type==_UMINUS)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		curNode = new char [30];
		sprintf(curNode,"%d [label=\"UMINUS\" shape=box];",node1);
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
	}
	else if(expr->type==_VARVAL)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		curNode = new char [30];
		sprintf(curNode,"%d [label=\"Type = VARVAL\"];",node1);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
		// ��������� ����� �������� ��������� ����
		node2=*nodeCount+1; 
		printVarVal(expr->exprVal,nodeCount,dotTree);
		// �������� � ������ ����� ����� �������� � ������������ ������
		curNode[0] = '\0';
		sprintf(curNode,"%d -- %d;",node1,node2);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);			
	}
	else
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		curNode = new char [30];
		strcpy(curNode,makeExprNodename(expr->type,node1));
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
	}
}

void printVarVal(struct ValInfo * val, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ����
	char* curNode;
	std::string nodeDec;
	// ��������� ����� �������� "�������������" ����
	node1=*nodeCount;
	// � ����������� �� ���� ����/�������� �������� ��������������� �������
	if(val->type==_TRUE)
	{
		curNode = new char [30];
		//printExpr(begining->expr,nodeCount,dotTree);
		sprintf(curNode,"%d [label=\"true\"];",node1);
	}
	else if(val->type==_FALSE)
	{
		curNode = new char [30];
		//printExpr(begining->expr,nodeCount,dotTree);
		sprintf(curNode,"%d [label=\"false\"];",node1);
	}
	else if(val->type==_STRING)
	{
		curNode = new char [30+strlen(val->stringVal)+1];
		//printExpr(begining->expr,nodeCount,dotTree);
		sprintf(curNode,"%d [label=\"%s\"];",node1,val->stringVal);
	}
	else if(val->type==_NUMBER)
	{
		curNode = new char [30];
		//printExpr(begining->expr,nodeCount,dotTree);
		sprintf(curNode,"%d [label=\"%d\"];",node1,val->intVal);
	}
	else
		return ;
	nodeDec = std::string(curNode);
	dotTree.push_back(nodeDec);
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
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ����
	char* curNode;
	std::string nodeDec;
	
	// ��������� ����� �������� "�������������" ����
	node1=*nodeCount;
	curNode = new char [30];
	sprintf(curNode,"%d [label=\"RETURN\" shape=box];",node1);
	nodeDec = std::string(curNode);
	dotTree.push_back(nodeDec);
	// ��������� ����� �������� ��������� ����
	node2=*nodeCount+1; 
	printExpr(expr,nodeCount,dotTree);
	// �������� � ������ ����� ����� �������� � ������������ ������
	curNode[0] = '\0';
	sprintf(curNode,"%d -- %d;",node1,node2);
	nodeDec = std::string(curNode);
	dotTree.push_back(nodeDec);
}

void printDelStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ����
	char* curNode;
	std::string nodeDec;
	
	// ��������� ����� �������� "�������������" ����
	node1=*nodeCount;
	curNode = new char [30];
	sprintf(curNode,"%d [label=\"DEL\" shape=box];",node1);
	nodeDec = std::string(curNode);
	dotTree.push_back(nodeDec);
	// ��������� ����� �������� ��������� ����
	node2=*nodeCount+1; 
	printExpr(expr,nodeCount,dotTree);
	// �������� � ������ ����� ����� �������� � ������������ ������
	curNode[0] = '\0';
	sprintf(curNode,"%d -- %d;",node1,node2);
	nodeDec = std::string(curNode);
	dotTree.push_back(nodeDec);
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