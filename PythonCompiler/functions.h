#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "tree_structs.h"

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

/* Создание стейтмента для continue, break, del, return
* \param[in] type тип стейтмента
* \return указатель на стейтмент
*/
struct StmtInfo * createFromSingleStatement(enum StmtType type)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->next=NULL;
	return stmt;
}