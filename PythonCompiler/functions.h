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

/* Создание стейтмент листа
* \param[in] type тип стейтмента
* \param[in] ifstmt указатель на if стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createIfStatement(enum StmtType type, struct IfStmtInfo * ifstmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->ifstmt=ifstmt;
	stmt->next=NULL;
	return stmt;
}