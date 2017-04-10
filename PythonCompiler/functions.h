#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "tree_structs.h"

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

	stmtlist->last=stmt;
	stmtlist->last->next=stmtlist->last;
	return stmtlist;
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

/* �������� ���������� ��� continue, break, del, return
* \param[in] type ��� ����������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromSingleStatement(enum StmtType type)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->next=NULL;
	return stmt;
}