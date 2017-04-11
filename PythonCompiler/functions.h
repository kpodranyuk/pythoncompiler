#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "tree_structs.h"

/* Создание узла из выражения
*
*/

struct ExprListInfo* createExprList()
{
	// Заглушка
	return NULL;
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

	stmtlist->last=stmt;
	stmtlist->last->next=stmtlist->last;
	return stmtlist;
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

/* Создание if стейтмента
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