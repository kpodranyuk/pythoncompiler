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
void printExprList(struct ExprListInfo * exprlist, int* nodeCount, std::vector<std::string>& dotTree);
void printFuncCall(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree);
void addDeclStringToStringList(char* string, int declNumber, std::vector<std::string>& dotTree);
void addLinkToStringList(int firstNode, int secondNode, std::vector<std::string>& dotTree);
/* ������ ������ � "���������" ��� ������ �� ���� dot
* @author Kate
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
	std::string nodeDec;
	// ��������� ����� �������� "�������������" ����
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"MAINBLOCK\" shape=invtriangle];",node1,dotTree);
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
		addLinkToStringList(node1,node2,dotTree);
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
	*/
	char* curNode;
	std::string nodeDec;
	// � ����������� �� ���� ����/�������� �������� ��������������� �������
	if(expr->type==_NOT)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"NOT\" shape=box];",node1,dotTree);
		// ��������� ����� �������� ��������� ����
		node2=*nodeCount+1; 
		printExpr(expr->left,nodeCount,dotTree);
		// �������� � ������ ����� ����� �������� � ������������ ������
		addLinkToStringList(node1,node2,dotTree);
	}
	else if(expr->type==_OPERAND)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		curNode = new char [50+strlen(expr->idName)+1];
		sprintf(curNode,"%d [label=\"Type = OPERAND\n%s\"];",node1,expr->idName);
		nodeDec = std::string(curNode);
		dotTree.push_back(nodeDec);
	}
	else if(expr->type==_UMINUS)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"UMINUS\" shape=box];",node1,dotTree);
		// ��������� ����� �������� ��������� ����
		node2=*nodeCount+1; 
		printExpr(expr->left,nodeCount,dotTree);
		// �������� � ������ ����� ����� �������� � ������������ ������
		addLinkToStringList(node1,node2,dotTree);
	}
	else if(expr->type==_VARVAL)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"Type = VARVAL\"];",node1,dotTree);
		// ��������� ����� �������� ��������� ����
		node2=*nodeCount+1; 
		printVarVal(expr->exprVal,nodeCount,dotTree);
		// �������� � ������ ����� ����� �������� � ������������ ������
		addLinkToStringList(node1,node2,dotTree);
	}
	else if(expr->type==_FUNCCALL)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"Type = FUNC_CALL\"];",node1,dotTree);
		// �����
		printFuncCall(expr,nodeCount,dotTree);
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
		addLinkToStringList(node1,node2,dotTree);
		// ��������� ����� �������� ��������� ����
		node2=*nodeCount+1; 
		printExpr(expr->right,nodeCount,dotTree);
		// �������� � ������ ����� ����� �������� � ������������ ������
		addLinkToStringList(node1,node2,dotTree);
	}
}

void printVarVal(struct ValInfo * val, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1; // ����� �������� ���� � ����� ��������� ����
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

void printExprList(struct ExprListInfo * exprlist, int* nodeCount, std::vector<std::string>& dotTree)
{
	// ������� ExprInfo ��������� �� ������� �������� ������
	struct ExprInfo* begining;
	// ������� ������ ������� ������ ���������
	begining = exprlist->first;
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	//*nodeCount+=1; // �������� ����� ����
	std::string nodeDec;
	// ��������� ����� �������� "�������������" ����
	node1=*nodeCount;
	// ���� ������� ������� ������ �� ���������..
	while(begining!=NULL)
	{
		// ��������� ����� �������� ��������� ����
		node2=*nodeCount+1; 
		printExpr(begining,nodeCount,dotTree);
		// �������� � ������ ����� ����� �������� � ������������ ������
		addLinkToStringList(node1,node2,dotTree);
		// ������� ��������� ������� ������ ����� �������
		begining = begining->next;
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

void printFuncCall(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
	char* curNode;
	std::string nodeDec;
	int parent = (*nodeCount)++;
	int name = (*nodeCount)++;
	int args, node2;
	curNode = new char[30+strlen(expr->idName)];
	sprintf(curNode,"%d [label=\"FUNC:\t%s\"];",name,expr->idName);	
	nodeDec = std::string(curNode);
	dotTree.push_back(nodeDec);
	// �������� � ������ ����� ����� �������� � ������������ ������
	addLinkToStringList(parent,name,dotTree);
	// ��������� ����� �������� ��������� ����
	args = (*nodeCount);
	curNode[0] = '\0';
	addDeclStringToStringList("[label=\"ARGS\"];",args,dotTree);
	// ��������� ����� �������� ��������� ����
	node2=(*nodeCount)+1; 
	curNode = new char[30+strlen(expr->idName)];
	printExprList(expr->arglist,nodeCount,dotTree);
	addLinkToStringList(parent,args,dotTree);
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
	std::string nodeDec;
	
	// ��������� ����� �������� "�������������" ����
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"RETURN\" shape=box];",node1,dotTree);
	// ��������� ����� �������� ��������� ����
	node2=*nodeCount+1; 
	printExpr(expr,nodeCount,dotTree);
	// �������� � ������ ����� ����� �������� � ������������ ������
	addLinkToStringList(node1,node2,dotTree);
}

void printDelStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ����
	std::string nodeDec;
	
	// ��������� ����� �������� "�������������" ����
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"DEL\" shape=box];",node1,dotTree);
	// ��������� ����� �������� ��������� ����
	node2=*nodeCount+1; 
	printExpr(expr,nodeCount,dotTree);
	// �������� � ������ ����� ����� �������� � ������������ ������
	addLinkToStringList(node1,node2,dotTree);
}
/* �������� � ������ ����� ���������� ���� �� ����� dot
* @author Kate
* \param[in] string ������ � ��������� ���� � ��� �����������
* \param[in] declNumber ����� ����
* \param[in|out] dotTree ������ ����� ��� �������� ���� �� dot
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
/* �������� � ������ ����� ����� ����� �� ����� dot
* @author Kate
* \param[in] firstNode ����� ������� ����
* \param[in] secondNode ����� ������� ����
* \param[in|out] dotTree ������ ����� ��� �������� ���� �� dot
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