#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "tree_structs.h"

#pragma warning(disable : 4996)

/* Создание листа выражений
* \param[in] expr указатель на добавляемое выражение
* \param[in] exprlist указатель на лист
* \return указатель на лист
*/
struct ExprListInfo * createExprList(struct ExprInfo * expr, struct ExprListInfo * exprlist)
{
	//если новый узел
	if(exprlist==NULL&&expr==NULL)
	{
		//создаем новый узел
		struct ExprListInfo * newExprList=(struct ExprListInfo*)malloc(sizeof(struct ExprListInfo));
		newExprList->first=NULL;
		newExprList->last=NULL;
		return newExprList;
	}

	//если ничего нового в узел не добавляется(stmt_list и после него идет line_sep, то возвращаем тот же  stmtlist)
	if(expr==NULL)
	{
		return exprlist;
	}

	expr->next=NULL;
	//если новый узел
	if(exprlist==NULL)
	{
		//создаем новый узел
		struct ExprListInfo * newExprList=(struct ExprListInfo*)malloc(sizeof(struct ExprListInfo));
		newExprList->first=expr;
		newExprList->last=expr;
		return newExprList;
	}

	exprlist->last->next=expr;
	exprlist->last=expr;
	return exprlist;
}

/* Создание узла из выражения (операнда или значения)
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
	// Заглушка
	return expr;
}

/* Создание узла выражения из выражений
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

/* Создание узла выражения из выражений
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

/* Создание узла из типа выражения
* \param[in] type тип значения 
* \param[in] logVal логическое значение 
* \param[in] stringVal строковое значение 
* \param[in] numVal целочисленное значение 
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

/* Создание стейтмент листа
* \param[in] stmt указатель на добавляемый стейтмент
* \param[in] stmtlist указатель на стейтмент лист
* \return указатель на стейтмент лист
*/
struct StmtListInfo * createStatementList(struct StmtInfo * stmt, struct StmtListInfo * stmtlist)
{
	//если ничего нового в узел не добавляется(stmt_list и после него идет line_sep, то возвращаем тот же  stmtlist)
	if(stmt==NULL)
	{
		return stmtlist;
	}

	stmt->next=NULL;
	//если новый узел
	if(stmtlist==NULL)
	{
		//создаем новый узел
		struct StmtListInfo * newStmtList=(struct StmtListInfo*)malloc(sizeof(struct StmtListInfo));
		newStmtList->first=stmt;
		newStmtList->last=stmt;
		return newStmtList;
	}

	stmtlist->last->next=stmt;
	stmtlist->last=stmt;
	return stmtlist;
}

/* Создание стейтмента для выражения
* \param[in] type тип стейтмента
* \param[in] ifstmt указатель на if стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createFromExprStatement(enum StmtType type, struct ExprInfo * exprstmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->expr=exprstmt;
	stmt->next=NULL;
	return stmt;
}

/* Создание стейтмента для развилки
* \param[in] type тип стейтмента
* \param[in] ifstmt указатель на if стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createFromIfStatement(enum StmtType type, struct IfStmtInfo * ifstmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->ifstmt=ifstmt;
	stmt->next=NULL;
	return stmt;
}

/* Создание стейтмента для цикла for
* \param[in] type тип стейтмента
* \param[in] forstmt указатель на for стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createFromForStatement(enum StmtType type, struct ForStmtInfo * forstmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->forstmt=forstmt;
	stmt->next=NULL;
	return stmt;
}

/* Создание стейтмента для цикла while
* \param[in] type тип стейтмента
* \param[in] whilestmt указатель на while стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createFromWhileStatement(enum StmtType type, struct WhileStmtInfo * whilestmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->whilestmt=whilestmt;
	stmt->next=NULL;
	return stmt;
}

/* Создание стейтмента для continue, break
* \param[in] type тип стейтмента
* \return указатель на стейтмент
*/
struct StmtInfo * createFromContinueBreakStatement(enum StmtType type)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->next=NULL;
	return stmt;
}

/* Создание стейтмента для return
* \param[in] type тип стейтмента
* \param[in] expr возвращаемое выражение
* \return указатель на стейтмент
*/
struct StmtInfo * createFromReturnStatement(enum StmtType type, struct ExprInfo * expr)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->expr=expr;
	stmt->next=NULL;
	return stmt;
}

/* Создание стейтмента для del
* \param[in] type тип стейтмента
* \param[in] operand очищаемый операнд
* \return указатель на стейтмент
*/
struct StmtInfo * createFromDelStatement(enum StmtType type, char * operand)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->expr=createSimpleExpr(_OPERAND, operand, NULL);
	stmt->next=NULL;
	return stmt;
}

/* Создание if
* \param[in] expr условное выражение
* \param[in] stmtlist тело
* \param[in] eliflist список elif
* \param[in] elsestmtlist тело блока else
* \return указатель на if стейтмент
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

/* Создание elif листа
* \param[in] expr условное выражение
* \param[in] stmtlist тело
* \param[in] eliflist указатель на список elif-ов
* \return указатель на список
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


/* Создание for
* \param[in] counter счетчик цикла
* \param[in] expr выражение по чем проходит цикл
* \param[in] stmtlist тело цикла
* \param[in] elsestmt тело else после цикла
* \return указатель for
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

/* Создание while
* \param[in] expr условие выполнения
* \param[in] stmtlist тело цикла
* \param[in] elsestmt тело else после цикла
* \return указатель while
*/
struct WhileStmtInfo * createWhileStatement(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct StmtListInfo * elsestmt)
{
	struct WhileStmtInfo * whilestmt = (struct WhileStmtInfo *)malloc(sizeof(struct WhileStmtInfo));
	whilestmt->expr=expr;
	whilestmt->stmtlist=stmtlist;
	whilestmt->elsestmt=elsestmt;
	return whilestmt; 
}