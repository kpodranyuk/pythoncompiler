#include "struct_functions.h"

#pragma warning(disable : 4996)

/* �������� ������� �������� ���� ���������
* @author Kate
* \param[in] FL ������ ������ ��������
* \param[in] FC ������� ������ ��������
* \param[in] LL ������ ����� ��������
* \param[in] LC ������� ����� ��������
* \return ��������� �� ��������� ������� �������� ���� ���������
*/
struct CodeLocation* createCodeLocation(int FL, int FC, int LL, int LC)
{
	struct CodeLocation* loc=(struct CodeLocation*)malloc(sizeof(struct CodeLocation));
	loc->firstLine=FL;
	loc->firstColumn=FC;
	loc->lastLine=LL;
	loc->lastColumn=LC;
	return loc;
}

/* �������� ����� ���������
* @author Kate
* \param[in] expr ��������� �� ����������� ���������
* \param[in] exprlist ��������� �� ����
* \return ��������� �� ����
*/
struct ExprListInfo * createExprList(struct ExprInfo * expr, struct ExprListInfo * exprlist)
{
	// ���� ������ �� ������ ���������� 
	if(exprlist==NULL&&expr==NULL)
	{
		// ������� ������ ������ � ���������� ���
		struct ExprListInfo * newExprList=(struct ExprListInfo*)malloc(sizeof(struct ExprListInfo));
		newExprList->first=NULL;
		newExprList->last=NULL;
		return newExprList;
	}
	// ���� ������ ������ � ���� �� �����������
	if(expr==NULL)
	{
		return exprlist;
	}
	// �������, ��� ���������� �������� � ��������� ���
	expr->next=NULL;
	// ���� �������� ������ ���
	if(exprlist==NULL)
	{
		// ������� ����� ������, ������������ ��������� ������� �������
		struct ExprListInfo * newExprList=(struct ExprListInfo*)malloc(sizeof(struct ExprListInfo));
		newExprList->first=expr;
		newExprList->last=expr;
		// ���������� ���
		return newExprList;
	}
	// ����� ��������� ������� ������ �� ������� ���������
	exprlist->last->next=expr;
	exprlist->last=expr;
	// ���������� ����������� ������� ������
	return exprlist;
}

/* �������� �������� ���� �� ��������� (�������� ��� ��������)
* @author Kate
* \param[in] type ��� ���������
* \param[in] opName ��� ��������
* \param[in] val �������� ��������
* \return ��������� �� ����
*/
struct ExprInfo* createSimpleExpr(enum ExprType type, char* opName, struct ValInfo* val, struct CodeLocation* loc)
{
	// �������� ������ ��� ����
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));
	// ����������� ���
	expr->type=type;
	// ���� �������� ��� ��������..
	if(opName!=NULL)
	{
		// �������� ������ � ���� ��� ��� �������� � ������������
		expr->idName = (char*)malloc(sizeof(strlen(opName)+1));
		strcpy(expr->idName, opName);
	}
	// ����� ����������� NULL
	else
		expr->idName=NULL;
	// ����������� ���������� �������� ��������
	expr->exprVal=val;

	expr->loc=loc;
	expr->left=NULL;
	expr->right=NULL;
	expr->middle=NULL;

	// ���������� ��������� ����
	return expr;
}

/* �������� ���� ��������� �� ���������
* @author Kate
* \param[in] type ��� ���������
* \param[in] left ����� ���������
* \param[in] right ������ ���������
* \return ��������� �� ����� ����
*/
struct ExprInfo* createExprInfo(enum ExprType type, struct ExprInfo* left, struct ExprInfo* right, struct CodeLocation* operationLoc)
{
	// �������� ������ ��� ����� ����-���������
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));
	// ����������� ���
	expr->type=type;
	// �������, ��� � ���� ���� ������ ����� � ������ �������� ����
	expr->arglist=NULL;
	expr->exprVal=NULL;
	expr->idName=NULL;
	expr->left=left;
	expr->right=right;
	expr->loc=operationLoc;
	expr->middle=NULL;
	// ���������� ����� ��������� ����
	return expr;
}

/* �������� ���� ��������� ������������� �������
* \param[in] type ��� ���������
* \param[in] exprlist ���� ���������
* \return ��������� �� ����� ����
*/
struct ExprInfo* createInitListInfo(enum ExprType type, struct ExprListInfo * exprlist, struct CodeLocation* bracket)
{
	// �������� ������ ��� ����� ����-���������
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));
	// ����������� ���
	expr->type=type;
	// �������, ��� � ���� ���� ������ ������ �����������
	expr->arglist=exprlist;
	expr->exprVal=NULL;
	expr->idName=NULL;
	expr->left=NULL;
	expr->right=NULL;
	expr->loc=bracket;
	// ���������� ����� ��������� ����
	return expr;
}

/* �������� ���� ��������� �������� ��� ��������
* \param[in] type ��� ���������
* \param[in] left ����� ���������(� ���� ���������)
* \param[in] right ������ ���������
* \param[in] actName ��� ����������� �������� � �������
* \return ��������� �� ����� ����
*/
struct ExprInfo* createActListInfo(enum ExprType type, struct ExprInfo* left, struct ExprInfo* right, char * actName, struct CodeLocation* dotPos)
{
	// �������� ������ ��� ����� ����-���������
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));
	// ����������� ���
	expr->type=type;
	expr->idName = (char*)malloc(sizeof(strlen(actName)+1));
	strcpy(expr->idName, actName);
	expr->left=left;
	expr->right=right;
	expr->arglist=NULL;
	expr->exprVal=NULL;
	expr->loc=dotPos;
	// ���������� ����� ��������� ����
	return expr;
}

/* �������� ���� ��������� �� ������ �������
* @author Kate
* \param[in] type ��� ���������
* \param[in] funcName ��� �������
* \param[in] args ������ ����������
* \return ��������� �� ����� ����
*/
struct ExprInfo* createExprInfoFromFuncCall(enum ExprType type, char* funcName, struct ExprListInfo* args, struct CodeLocation* callL)
{
	// �������� ������ ��� ����
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));
	// ��������������� ��� � ���������
	expr->type=type;
	expr->arglist=args;
	// �������� ������ ��� ��� �������
	expr->idName=(char*)malloc(sizeof(strlen(funcName)+1));
	strcpy(expr->idName,funcName);
	// ������� ��������� �������������
	expr->exprVal=NULL;
	expr->left=NULL;
	expr->right=NULL;
	expr->next=NULL;
	expr->loc=callL;
	// ���������� ����
	return expr;
}

/* �������� ���� �� ���� ���������
* @author Kate
* \param[in] type ��� �������� 
* \param[in] logVal ���������� �������� 
* \param[in] stringVal ��������� �������� 
* \param[in] numVal ������������� �������� 
*/
struct ValInfo* createValNode(enum ValType type, bool logVal, char* stringVal, int numVal, struct CodeLocation* loc)
{
	// �������� ������ ��� ����
	struct ValInfo* val = (struct ValInfo*)malloc(sizeof(struct ValInfo));
	// ��������������� ��� � ���������� ��������
	val->type = type;
	val->logVal = logVal;
	// ���� ������ ��������� ��������
	/*if(stringVal!=NULL)
	{
		// �������� ������ � ������������ ��������
		val->stringVal = (char*)malloc(sizeof(strlen(stringVal)+1));
		strcpy(val->stringVal, stringVal);
	}
	// ����� ���������������
	else*/
		val->stringVal = stringVal;
	// ��������������� ������������� ��������
	val->intVal = numVal;
	val->loc=loc;
	// ���������� ��������� ����
	return val;
}


/* �������� ������ ���������� �������
* @author Kate
* \param[in] newParamName ����� �������� �������
* \param[in] params ������ ����������
* \return ��������� �� ����� ������
*/
struct DefFuncParamListInfo* createDefFuncParamListInfo(char* newParamName, struct ValInfo* newParamVal, struct DefFuncParamListInfo* params, struct CodeLocation* loc)
{
	// ���� ������ �� ������ ���������� 
	if(newParamName==NULL&&params==NULL&&newParamVal==NULL)
	{
		// ������� ������ ������ � ���������� ���
		struct DefFuncParamListInfo * newParamList=(struct DefFuncParamListInfo*)malloc(sizeof(struct DefFuncParamListInfo));
		newParamList->first=NULL;
		newParamList->last=NULL;
		return newParamList;
	}
	// ���� ������ ������ � ���� �� �����������
	if(newParamName==NULL)
	{
		return params;
	}
	// ������� ����� ����-�������
	struct DefFuncParamInfo* newParam = (struct DefFuncParamInfo*)malloc(sizeof(struct DefFuncParamInfo));
	newParam->paramName = (char*)malloc(strlen(newParamName)+1);
	strcpy(newParam->paramName,newParamName);
	// �������, ��� ���������� �������� � ��������� ���
	newParam->next=NULL;
	newParam->paramVal=newParamVal;
	newParam->loc=loc;
	// ���� �������� ������ ���
	if(params==NULL)
	{
		// ������� ����� ������, ������������ ��������� ������� �������
		struct DefFuncParamListInfo * newParamList=(struct DefFuncParamListInfo*)malloc(sizeof(struct DefFuncParamListInfo));
		newParamList->first=newParam;
		newParamList->last=newParam;
		// ���������� ���
		return newParamList;
	}
	// ����� ��������� ������� ������ �� ������� ���������
	params->last->next=newParam;
	params->last=newParam;
	// ���������� ����������� ������� ������
	return params;
}

/* �������� ���� ���������� �������
* @author Kate
* \param[in] funcName ��� �������
* \param[in] params ��������� �������
* \param[in] funcBody ���� �������
* \return ��������� �� ���� ���������� �������
*/
struct FuncDefInfo* createFuncDef(char* funcName, struct DefFuncParamListInfo* params, struct StmtListInfo* funcBody, struct CodeLocation* nameL)
{
	if(funcName==NULL||funcBody==NULL) 
		return NULL;
	struct FuncDefInfo* func = (struct FuncDefInfo*)malloc(sizeof(struct FuncDefInfo));
	func->functionName = (char*)malloc(strlen(funcName)+1);
	strcpy(func->functionName,funcName);
	func->params=params;
	func->body=funcBody;
	func->nameLoc=nameL;
	return func;
}

/* �������� ���������� ��� ����������� �������
* @author Kate
* \param[in] type ��� ����������
* \param[in] funcDef ��������� �� ���������� �������
* \return ��������� �� ���������
*/
struct StmtInfo* createFromFuncDefStatement(enum StmtType type, struct FuncDefInfo* funcDef)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->funcdefstmt=funcDef;
	stmt->next=NULL;
	return stmt;
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
struct StmtInfo * createFromContinueBreakStatement(enum StmtType type, struct CodeLocation* loc)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->next=NULL;
	stmt->loc=loc;
	return stmt;
}

/* �������� ���������� ��� return
* \param[in] type ��� ����������
* \param[in] expr ������������ ���������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromReturnStatement(enum StmtType type, struct ExprInfo * expr, struct CodeLocation* loc)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->expr=expr;
	stmt->next=NULL;
	stmt->loc=loc;
	return stmt;
}

/* �������� ���������� ��� del
* \param[in] type ��� ����������
* \param[in] operand ��������� �������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromDelStatement(enum StmtType type, char * operand, struct CodeLocation* delLoc, struct CodeLocation* opLoc)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->expr=createSimpleExpr(_OPERAND, operand, NULL,opLoc);
	stmt->next=NULL;
	stmt->loc=delLoc;
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