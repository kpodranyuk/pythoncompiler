#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "tree_structs.h"


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