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

void printExpr(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree, bool attr);
void printIfStmt(struct IfStmtInfo * ifstmt, int* nodeCount, std::vector<std::string>& dotTree, bool attr);
void printWhileStmt(struct WhileStmtInfo * whilestmt, int* nodeCount, std::vector<std::string>& dotTree, bool attr);
void printForStmt(struct ForStmtInfo * forstmt, int* nodeCount, std::vector<std::string>& dotTree, bool attr);
void printFuncDefStmt(struct FuncDefInfo * funcdefstmt, int* nodeCount, std::vector<std::string>& dotTree, bool attr);
void printContinueBreakStmt(enum StmtType type, int* nodeCount, std::vector<std::string>& dotTree, bool attr);
void printReturnStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree, bool attr);
void printDelStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree, bool attr);
void printFuncParams(struct DefFuncParamListInfo* params,int* nodeCount,std::vector<std::string>& dotTree, bool attr);

char* makeStringForDot(char* string);
char* makeExprNodename(enum ExprType type, char * idName);
void printVarVal(struct ValInfo * val, int* nodeCount, std::vector<std::string>& dotTree, bool attr);
void printExprList(struct ExprListInfo * exprlist, int* nodeCount, std::vector<std::string>& dotTree, bool attr);
void printFuncCall(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree, bool attr);
void addDeclStringToStringList(char* string, int declNumber, std::vector<std::string>& dotTree);
void addLinkToStringList(int firstNode, int secondNode, std::vector<std::string>& dotTree);
/* ������ ������ � "���������" ��� ������ �� ���� dot
* @author Kate
* \param[in] root ������ ������ ������
* \param[in|out] nodeCount ������� ������ ����
* \param[in|out] dotTree ������ ����� ��� �������� ���� �� dot
*/
void printStatementList(struct StmtListInfo* root, int* nodeCount, std::vector<std::string>& dotTree, bool attr)
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
	if(node1==1)
		addDeclStringToStringList("[label=\"MAINBLOCK\" shape=invtriangle];",node1,dotTree);
	else
		addDeclStringToStringList("[label=\"CODEBLOCK\" shape=trapezium];",node1,dotTree);
	// ���� ������� ������� ������ �� ���������..
	while(begining!=NULL)
	{
		// � ����������� �� ���� ����/�������� �������� ��������������� �������
		// ��������� ����� �������� ��������� ����
		node2=*nodeCount+1; 
		if(begining->type==_EXPR)
		{
			printExpr(begining->expr,nodeCount,dotTree, attr);
		}
		else if(begining->type==_IF)
		{
			printIfStmt(begining->ifstmt,nodeCount,dotTree, attr);
		}
		else if(begining->type==_FOR)
		{
			printForStmt(begining->forstmt,nodeCount,dotTree, attr);
		}
		else if(begining->type==_WHILE)
		{
			printWhileStmt(begining->whilestmt,nodeCount,dotTree, attr);
		}
		else if(begining->type==_FUNC_DEF)
		{
			printFuncDefStmt(begining->funcdefstmt,nodeCount,dotTree, attr);
		}
		else if(begining->type==_RETURN)
		{
			printReturnStmt(begining->expr,nodeCount,dotTree, attr);
		}
		else if(begining->type==_BREAK)
		{
			printContinueBreakStmt(_BREAK,nodeCount,dotTree, attr);
		}
		else if(begining->type==_CONTINUE)
		{
			printContinueBreakStmt(_CONTINUE,nodeCount,dotTree, attr);
		}
		else if(begining->type==_DEL)
		{
			printDelStmt(begining->expr,nodeCount,dotTree, attr);
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


void printExpr(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree, bool attr)
{
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ����
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
		printExpr(expr->left,nodeCount,dotTree, attr);
		// �������� � ������ ����� ����� �������� � ������������ ������
		addLinkToStringList(node1,node2,dotTree);
	}
	else if(expr->type==_OPERAND)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		if(attr)
		{
			curNode = new char [150+strlen(expr->idName)+1];
			if(expr->locFor!=NULL)
				sprintf(curNode,"[label=\"LT_#:%d, %d\n<ID>\n%s\"];",expr->locFor,expr->numberInTable,expr->idName);
			else
				sprintf(curNode,"[label=\"CT_#:%d\n<ID>\n%s\"];",expr->numberInTable,expr->idName);
		}
		else
		{
			curNode = new char [50+strlen(expr->idName)+1];
			sprintf(curNode,"[label=\"<ID>\n%s\"];",expr->idName);
		}
		addDeclStringToStringList(curNode,node1,dotTree);
	}
	else if(expr->type==_UMINUS)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"UMINUS\" shape=box];",node1,dotTree);
		// ��������� ����� �������� ��������� ����
		node2=*nodeCount+1; 
		printExpr(expr->left,nodeCount,dotTree, attr);
		// �������� � ������ ����� ����� �������� � ������������ ������
		addLinkToStringList(node1,node2,dotTree);
	}
	else if(expr->type==_VARVAL)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount-1;
		//addDeclStringToStringList("[label=\"VARVAL\"];",node1,dotTree);
		// ��������� ����� �������� ��������� ����
		node2=*nodeCount; 
		printVarVal(expr->exprVal,nodeCount,dotTree, attr);
		// �������� � ������ ����� ����� �������� � ������������ ������
		//addLinkToStringList(node1,node2,dotTree);
	}
	else if(expr->type==_FUNCCALL)
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"FUNC_CALL\"];",node1,dotTree);
		// �����
		printFuncCall(expr,nodeCount,dotTree, attr);
	}
	else if(expr->type==_ARRINIT)
	{
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"INITIALIZATION_LIST\"];",node1,dotTree);
		printExprList(expr->arglist,nodeCount,dotTree, attr);
	}
	else
	{
		// ��������� ����� �������� "�������������" ����
		node1=*nodeCount;
		addDeclStringToStringList(makeExprNodename(expr->type, expr->idName),node1,dotTree);
		// ��������� ����� �������� ��������� ����
		node2=*nodeCount+1; 
		printExpr(expr->left,nodeCount,dotTree, attr);
		// �������� � ������ ����� ����� �������� � ������������ ������
		addLinkToStringList(node1,node2,dotTree);
		if(expr->type==_ARRID_AND_ASSIGN)
		{
			// ��������� ����� �������� ��������� ����
			node2=*nodeCount+1; 
			printExpr(expr->middle,nodeCount,dotTree, attr);
			// �������� � ������ ����� ����� �������� � ������������ ������
			addLinkToStringList(node1,node2,dotTree);
		}
		// ��������� ����� �������� ��������� ����
		node2=*nodeCount+1; 
		printExpr(expr->right,nodeCount,dotTree, attr);
		// �������� � ������ ����� ����� �������� � ������������ ������
		addLinkToStringList(node1,node2,dotTree);
	}
}

void printVarVal(struct ValInfo * val, int* nodeCount, std::vector<std::string>& dotTree, bool attr)
{
	int node1; // ����� �������� ���� � ����� ��������� ����
	//*nodeCount+=1; // �������� ����� ����
	char* curNode;
	// ��������� ����� �������� "�������������" ����
	node1=*nodeCount;
	// � ����������� �� ���� ����/�������� �������� ��������������� �������
	if(val->type==_TRUE)
	{
		curNode = new char [100];
		sprintf(curNode,"[label=\"CT_#:%d\nVARVAL\ntrue\"];",val->numberInTable);
		addDeclStringToStringList(curNode,node1,dotTree);
	}
	else if(val->type==_FALSE)
	{
		curNode = new char [100];
		sprintf(curNode,"[label=\"CT_#:%d\nVARVAL\nfalse\"];",val->numberInTable);
		addDeclStringToStringList(curNode,node1,dotTree);
	}
	else if(val->type==_NONE)
	{
		curNode = new char [100];
		sprintf(curNode,"[label=\"CT_#:%d\nVARVAL\nNONE\"];",val->numberInTable);
		addDeclStringToStringList(curNode,node1,dotTree);
	}
	else if(val->type==_STRING)
	{
		char* buf = makeStringForDot(val->stringVal);
		curNode = new char [100+strlen(buf)+1];
		sprintf(curNode,"[label=\"CT_#:%d\nVARVAL\n%s\"];",val->numberInTable,buf);
		addDeclStringToStringList(curNode,node1,dotTree);
	}
	else if(val->type==_NUMBER)
	{
		curNode = new char [150];
		sprintf(curNode,"[label=\"CT_#:%d\nVARVAL\n%d\"];",val->numberInTable,val->intVal);
		addDeclStringToStringList(curNode,node1,dotTree);
	}
	else if(val->type==_FLOAT)
	{
		curNode = new char [150];
		sprintf(curNode,"[label=\"CT_#:%d\nVARVAL\n%f\"];",val->numberInTable,val->fVal);
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

void printExprList(struct ExprListInfo * exprlist, int* nodeCount, std::vector<std::string>& dotTree, bool attr)
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
		printExpr(begining,nodeCount,dotTree, attr);
		// �������� � ������ ����� ����� �������� � ������������ ������
		addLinkToStringList(node1,node2,dotTree);
		// ������� ��������� ������� ������ ����� �������
		begining = begining->next;
	}
}

void printIfStmt(struct IfStmtInfo * ifstmt, int* nodeCount, std::vector<std::string>& dotTree, bool attr)
{
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ���� if_stmt
	int if_stmt=*nodeCount;

	node1=*nodeCount;
	addDeclStringToStringList("[label=\"IF_STMT\"];",if_stmt,dotTree);

	// ����� ��������� ���������
	*nodeCount+=1;
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"CONDITION\"];",node1,dotTree);
	addLinkToStringList(if_stmt,node1,dotTree);
	node2=*nodeCount+1;
	printExpr(ifstmt->expr,nodeCount,dotTree, attr);
	addLinkToStringList(node1,node2,dotTree);

	// ����� ���� �������
	node1=if_stmt;
	node2=*nodeCount+1;
	printStatementList(ifstmt->stmtlist, nodeCount, dotTree, attr);
	addLinkToStringList(node1,node2,dotTree);

	// ����� elif-��
	struct ElifListInfo* begin=ifstmt->eliflist;
	while(begin!=NULL)
	{
		*nodeCount+=1;
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"ELIF\"];",node1,dotTree);
		addLinkToStringList(if_stmt,node1,dotTree);
		int elif_stmt=node1;

		// ����� ��������� ���������
		*nodeCount+=1;
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"CONDITION\"];",node1,dotTree);
		addLinkToStringList(elif_stmt,node1,dotTree);
		node2=*nodeCount+1;
		printExpr(begin->expr,nodeCount,dotTree, attr);
		addLinkToStringList(node1,node2,dotTree);

		// ����� ����
		node1=elif_stmt;
		node2=*nodeCount+1;
		printStatementList(begin->stmtlist, nodeCount, dotTree, attr);
		addLinkToStringList(node1,node2,dotTree);

		begin=begin->next;
	}

	// ����� ����� else
	if(ifstmt->elsestmtlist!=NULL)
	{
		*nodeCount+=1;
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"ELSE BLOCK\"];",node1,dotTree);
		addLinkToStringList(if_stmt,node1,dotTree);
		node2=*nodeCount+1;
		printStatementList(ifstmt->elsestmtlist, nodeCount, dotTree, attr);
		addLinkToStringList(node1,node2,dotTree);
	}
}


void printWhileStmt(struct WhileStmtInfo * whilestmt, int* nodeCount, std::vector<std::string>& dotTree, bool attr)
{
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ���� while_stmt
	int while_stmt=*nodeCount;
	
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"WHILE_STMT\"];",while_stmt,dotTree);

	// ����� ������� ����������� �����
	*nodeCount+=1;
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"CONDITION\"];",node1,dotTree);
	addLinkToStringList(while_stmt,node1,dotTree);
	node2=*nodeCount+1;
	printExpr(whilestmt->expr,nodeCount,dotTree, attr);
	addLinkToStringList(node1,node2,dotTree);

	// ����� ���� �����
	node1=while_stmt;
	node2=*nodeCount+1;
	printStatementList(whilestmt->stmtlist, nodeCount, dotTree, attr);
	addLinkToStringList(node1,node2,dotTree);

	// ����� ����� else
	if(whilestmt->elsestmt!=NULL)
	{
		*nodeCount+=1;
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"ELSE BLOCK\"];",node1,dotTree);
		addLinkToStringList(while_stmt,node1,dotTree);
		node2=*nodeCount+1;
		printStatementList(whilestmt->elsestmt, nodeCount, dotTree, attr);
		addLinkToStringList(node1,node2,dotTree);
	}
}

void printForStmt(struct ForStmtInfo * forstmt, int* nodeCount, std::vector<std::string>& dotTree, bool attr)
{
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ���� for_stmt
	int for_stmt=*nodeCount;

	node1=*nodeCount;
	addDeclStringToStringList("[label=\"FOR_STMT\"];",for_stmt,dotTree);

	// ����� �������� �����
	*nodeCount+=1;
	node1=*nodeCount;
	char* curNode = new char [50+strlen(forstmt->counter->idName)+1];
	sprintf(curNode,"[label=\"<COUNTER_ID>\n%s\"];",forstmt->counter->idName);
	addDeclStringToStringList(curNode,node1,dotTree);
	addLinkToStringList(for_stmt, node1, dotTree);

	// ����� ���������, �� ��� �������� ����
	*nodeCount+=1;
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"BYPASS_EXPR\"];",node1,dotTree);
	addLinkToStringList(for_stmt,node1,dotTree);
	node2=*nodeCount+1;
	printExpr(forstmt->expr,nodeCount,dotTree, attr);
	addLinkToStringList(node1,node2,dotTree);

	// ����� ���� �����
	node1=for_stmt;
	node2=*nodeCount+1;
	printStatementList(forstmt->stmtlist, nodeCount, dotTree, attr);
	addLinkToStringList(node1,node2,dotTree);

	// ����� ����� else
	if(forstmt->elsestmt!=NULL)
	{
		*nodeCount+=1;
		node1=*nodeCount;
		addDeclStringToStringList("[label=\"ELSE BLOCK\"];",node1,dotTree);
		addLinkToStringList(for_stmt,node1,dotTree);
		node2=*nodeCount+1;
		printStatementList(forstmt->elsestmt, nodeCount, dotTree, attr);
		addLinkToStringList(node1,node2,dotTree);
	}
}

void printFuncCall(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree, bool attr)
{
	char* curNode;
	int parent = (*nodeCount)++;
	int name = (*nodeCount)++;
	int args, node2;
	curNode = new char[50+strlen(expr->idName)];
	sprintf(curNode,"[label=\"CT_#:%d\nFUNC\n%s\"];",expr->numberInTable,expr->idName);	
	addDeclStringToStringList(curNode,name,dotTree);
	// �������� � ������ ����� ����� �������� � ������������ ������
	addLinkToStringList(parent,name,dotTree);
	// ��������� ����� �������� ��������� ����
	args = (*nodeCount);
	if(expr->arglist->first==NULL&&expr->arglist->last==NULL)
	{
		addDeclStringToStringList("[label=\"NO_ARGS\"];",args,dotTree);
	}
	else
	{
		addDeclStringToStringList("[label=\"ARGS\"];",args,dotTree);
		// ��������� ����� �������� ��������� ����
		node2=(*nodeCount)+1; 
		printExprList(expr->arglist,nodeCount,dotTree, attr);
	}
		addLinkToStringList(parent,args,dotTree);
}

void printFuncDefStmt(struct FuncDefInfo * funcdefstmt, int* nodeCount, std::vector<std::string>& dotTree, bool attr)
{
	char* curNode;
	int name = ++(*nodeCount);
	int node2;
	// ��������� ���� � ������ �������
	curNode = new char[50+strlen(funcdefstmt->functionName)];
	sprintf(curNode,"[label=\"CT_#:%d\nFUNC_DEF\n%s\"];",funcdefstmt->numberInTable,funcdefstmt->functionName);	
	addDeclStringToStringList(curNode,name,dotTree);
	// ��������� ����� �������� ��������� ����
	node2=*nodeCount+1; 
	printFuncParams(funcdefstmt->params,nodeCount,dotTree, attr);
	// �������� � ������ ����� ����� �������� � ������������ ������
	addLinkToStringList(name,node2,dotTree);
	// ��������� ����� �������� ��������� ����
	node2=*nodeCount+1; 
	printStatementList(funcdefstmt->body,nodeCount,dotTree, attr);
	// �������� � ������ ����� ����� �������� � ������������ ������
	addLinkToStringList(name,node2,dotTree);

}

void printFuncParams(struct DefFuncParamListInfo* params,int* nodeCount,std::vector<std::string>& dotTree, bool attr)
{
	if(params->first==NULL&&params->last==NULL)
	{
		addDeclStringToStringList("[label=\"NO_PARAMS\"];",++(*nodeCount),dotTree);
		return ;
	}
	char* curNode;
	// ������� DefFuncParamInfo ��������� �� ������� �������� ������
	struct DefFuncParamInfo* begining;
	// ������� ������ ������� ������ ���������
	begining = params->first;
	int node1, node2, node3; // ����� �������� ���� � ����� ��������� ����
	// ��������� ����� �������� "�������������" ����
	node1=++(*nodeCount);
	addDeclStringToStringList("[label=\"PARAMS\"];",node1,dotTree);
	// ���� ������� ������� ������ �� ���������..
	while(begining!=NULL)
	{
		// ��������� ����� �������� ��������� ����
		node2=++(*nodeCount); 
		curNode = new char[30+strlen(begining->paramName)];
		sprintf(curNode,"[label=\"%s\"];",begining->paramName);	
		addDeclStringToStringList(curNode,node2,dotTree);
		// �������� � ������ ����� ����� �������� � ������������ ������
		addLinkToStringList(node1,node2,dotTree);
		
		if(begining->paramVal!=NULL)
		{
			node3=node2;
			node2=*nodeCount+1; 
			printVarVal(begining->paramVal,nodeCount,dotTree, attr);
			// �������� � ������ ����� ����� �������� � ������������ ������
			addLinkToStringList(node3,node2,dotTree);
		}

		// ������� ��������� ������� ������ ����� �������
		begining = begining->next;
	}
}

void printContinueBreakStmt(enum StmtType type, int* nodeCount, std::vector<std::string>& dotTree, bool attr)
{
	int node1; // ����� �������� ���� � ����� ��������� ����
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

void printReturnStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree, bool attr)
{
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ����
	std::string nodeDec;
	
	// ��������� ����� �������� "�������������" ����
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"RETURN\" shape=box];",node1,dotTree);
	// ��������� ����� �������� ��������� ����
	node2=*nodeCount+1; 
	printExpr(expr,nodeCount,dotTree, attr);
	// �������� � ������ ����� ����� �������� � ������������ ������
	addLinkToStringList(node1,node2,dotTree);
}

void printDelStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree, bool attr)
{
	int node1, node2; // ����� �������� ���� � ����� ��������� ����
	*nodeCount+=1; // �������� ����� ����
	std::string nodeDec;
	
	// ��������� ����� �������� "�������������" ����
	node1=*nodeCount;
	addDeclStringToStringList("[label=\"DEL\" shape=box];",node1,dotTree);
	// ��������� ����� �������� ��������� ����
	node2=*nodeCount+1; 
	printExpr(expr,nodeCount,dotTree, attr);
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