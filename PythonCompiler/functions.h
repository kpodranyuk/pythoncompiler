#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "tree_structs.h"

#pragma warning(disable : 4996)

/* �������� ����� ���������
* \param[in] expr ��������� �� ����������� ���������
* \param[in] exprlist ��������� �� ����
* \return ��������� �� ����
*/
struct ExprListInfo * createExprList(struct ExprInfo * expr, struct ExprListInfo * exprlist)
{
	//���� ����� ����
	if(exprlist==NULL&&expr==NULL)
	{
		//������� ����� ����
		struct ExprListInfo * newExprList=(struct ExprListInfo*)malloc(sizeof(struct ExprListInfo));
		newExprList->first=NULL;
		newExprList->last=NULL;
		return newExprList;
	}

	//���� ������ ������ � ���� �� �����������(stmt_list � ����� ���� ���� line_sep, �� ���������� ��� ��  stmtlist)
	if(expr==NULL)
	{
		return exprlist;
	}

	expr->next=NULL;
	//���� ����� ����
	if(exprlist==NULL)
	{
		//������� ����� ����
		struct ExprListInfo * newExprList=(struct ExprListInfo*)malloc(sizeof(struct ExprListInfo));
		newExprList->first=expr;
		newExprList->last=expr;
		return newExprList;
	}

	exprlist->last->next=expr;
	exprlist->last=expr;
	return exprlist;
}

/* �������� ���� �� ��������� (�������� ��� ��������)
*
*/

struct ExprInfo* createSimpleExpr(enum ExprType type, char* opName, struct ValInfo* val)
{
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));
	expr->type=type;
	if(opName!=NULL)
	{
		expr->idName = (char*)malloc(sizeof(strlen(opName)+1));
		strcpy(expr->idName, opName);
	}
	else
		expr->idName=NULL;
	expr->exprVal=val;
	// ��������
	return expr;
}

/* �������� ���� ��������� �� ���������
*
*/

struct ExprInfo* createExprInfo(enum ExprType type, struct ExprInfo* left, struct ExprInfo* right)
{
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));

	expr->type=type;
	expr->arglist=NULL;
	expr->exprVal=NULL;
	expr->idName=NULL;
	expr->left=left;
	expr->right=right;
	
	return expr;
}

/* �������� ���� ��������� �� ���������
*
*/

struct ExprInfo* createExprInfoFromFuncCall(enum ExprType type, char* funcName, struct ExprListInfo* args)
{
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));

	expr->type=type;
	expr->arglist=args;
	expr->exprVal=NULL;
	expr->idName=(char*)malloc(sizeof(strlen(funcName)+1));
	strcpy(expr->idName,funcName);
	expr->left=NULL;
	expr->right=NULL;
	
	return expr;
}

/* �������� ���� �� ���� ���������
* \param[in] type ��� �������� 
* \param[in] logVal ���������� �������� 
* \param[in] stringVal ��������� �������� 
* \param[in] numVal ������������� �������� 
*/

struct ValInfo* createValNode(enum ValType type, bool logVal, char* stringVal, int numVal)
{
	struct ValInfo* val = (struct ValInfo*)malloc(sizeof(struct ValInfo));
	val->type = type;
	val->logVal = logVal;
	if(stringVal!=NULL)
	{
		val->stringVal = (char*)malloc(sizeof(strlen(stringVal)+1));
		strcpy(val->stringVal, stringVal);
	}
	else
	{
		val->stringVal = stringVal;
	}
	val->intVal = numVal;
	return val;
}

/* �������� ��������� �����
* \param[in] stmt ��������� �� ����������� ���������
* \param[in] stmtlist ��������� �� ��������� ����
* \return ��������� �� ��������� ����
*/
struct StmtListInfo * createStatementList(struct StmtInfo * stmt, struct StmtListInfo * stmtlist)
{
	//���� ������ ������ � ���� �� �����������(stmt_list � ����� ���� ���� line_sep, �� ���������� ��� ��  stmtlist)
	if(stmt==NULL)
	{
		return stmtlist;
	}

	stmt->next=NULL;
	//���� ����� ����
	if(stmtlist==NULL)
	{
		//������� ����� ����
		struct StmtListInfo * newStmtList=(struct StmtListInfo*)malloc(sizeof(struct StmtListInfo));
		newStmtList->first=stmt;
		newStmtList->last=stmt;
		return newStmtList;
	}

	stmtlist->last->next=stmt;
	stmtlist->last=stmt;
	return stmtlist;
}

/* �������� ���������� ��� ���������
* \param[in] type ��� ����������
* \param[in] ifstmt ��������� �� if ���������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromExprStatement(enum StmtType type, struct ExprInfo * exprstmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->expr=exprstmt;
	stmt->next=NULL;
	return stmt;
}

/* �������� ���������� ��� ��������
* \param[in] type ��� ����������
* \param[in] ifstmt ��������� �� if ���������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromIfStatement(enum StmtType type, struct IfStmtInfo * ifstmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->ifstmt=ifstmt;
	stmt->next=NULL;
	return stmt;
}

/* �������� ���������� ��� ����� for
* \param[in] type ��� ����������
* \param[in] forstmt ��������� �� for ���������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromForStatement(enum StmtType type, struct ForStmtInfo * forstmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->forstmt=forstmt;
	stmt->next=NULL;
	return stmt;
}

/* �������� ���������� ��� ����� while
* \param[in] type ��� ����������
* \param[in] whilestmt ��������� �� while ���������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromWhileStatement(enum StmtType type, struct WhileStmtInfo * whilestmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->whilestmt=whilestmt;
	stmt->next=NULL;
	return stmt;
}

/* �������� ���������� ��� continue, break
* \param[in] type ��� ����������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromContinueBreakStatement(enum StmtType type)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->next=NULL;
	return stmt;
}

/* �������� ���������� ��� return
* \param[in] type ��� ����������
* \param[in] expr ������������ ���������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromReturnStatement(enum StmtType type, struct ExprInfo * expr)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->expr=expr;
	stmt->next=NULL;
	return stmt;
}

/* �������� ���������� ��� del
* \param[in] type ��� ����������
* \param[in] operand ��������� �������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromDelStatement(enum StmtType type, char * operand)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->expr=createSimpleExpr(_OPERAND, operand, NULL);
	stmt->next=NULL;
	return stmt;
}

/* �������� if
* \param[in] expr �������� ���������
* \param[in] stmtlist ����
* \param[in] eliflist ������ elif
* \param[in] elsestmtlist ���� ����� else
* \return ��������� �� if ���������
*/
struct IfStmtInfo * createIfStatement(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct ElifListInfo * eliflist, struct StmtListInfo * elsestmtlist)
{
	struct IfStmtInfo * ifstmt = (struct IfStmtInfo *)malloc(sizeof(struct IfStmtInfo));
	ifstmt->expr=expr;
	ifstmt->stmtlist=stmtlist;
	ifstmt->eliflist=eliflist;
	ifstmt->elsestmtlist=elsestmtlist;
	return ifstmt;
}

/* �������� elif �����
* \param[in] expr �������� ���������
* \param[in] stmtlist ����
* \param[in] eliflist ��������� �� ������ elif-��
* \return ��������� �� ������
*/
struct ElifListInfo * createElifList(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct ElifListInfo * eliflist)
{
	struct ElifListInfo * elif = (struct ElifListInfo *)malloc(sizeof(struct ElifListInfo));
	elif->expr=expr;
	elif->stmtlist=stmtlist;
	elif->next=NULL;

	if(eliflist==NULL)
	{
		return elif;
	}
	else 
	{
		eliflist->next=elif;
		return eliflist;
	}
}


/* �������� for
* \param[in] counter ������� �����
* \param[in] expr ��������� �� ��� �������� ����
* \param[in] stmtlist ���� �����
* \param[in] elsestmt ���� else ����� �����
* \return ��������� for
*/
struct ForStmtInfo * createForStatement(char * counter, struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct StmtListInfo * elsestmt)
{
	struct ForStmtInfo * forstmt = (struct ForStmtInfo *)malloc(sizeof(struct ForStmtInfo));
	forstmt->counter=(char*)malloc(sizeof(strlen(counter)+1));
	strcpy(forstmt->counter, counter);
	forstmt->expr=expr;
	forstmt->stmtlist=stmtlist;
	forstmt->elsestmt=elsestmt;
	return forstmt; 
}

/* �������� while
* \param[in] expr ������� ����������
* \param[in] stmtlist ���� �����
* \param[in] elsestmt ���� else ����� �����
* \return ��������� while
*/
struct WhileStmtInfo * createWhileStatement(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct StmtListInfo * elsestmt)
{
	struct WhileStmtInfo * whilestmt = (struct WhileStmtInfo *)malloc(sizeof(struct WhileStmtInfo));
	whilestmt->expr=expr;
	whilestmt->stmtlist=stmtlist;
	whilestmt->elsestmt=elsestmt;
	return whilestmt; 
}