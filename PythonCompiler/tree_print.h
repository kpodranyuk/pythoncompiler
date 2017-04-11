#include "tree_structs.h"
#include <vector>

void printStatementList(struct StmtListInfo* root, int* nodeCount, std::vector<std::string>& dotTree)
{
	struct StmtInfo* begining;
	if(root==NULL) return ;
	begining = root->first;
	while(begining!=NULL)
	{
		if(begining->type==_EXPR)
		{
		}
		else if(begining->type==_IF)
		{
		}
		else if(begining->type==_FOR)
		{
		}
		else if(begining->type==_WHILE)
		{
		}
		else if(begining->type==_FUNC_DEF)
		{
		}
		else if(begining->type==_RETURN)
		{
		}
		else if(begining->type==_BREAK)
		{
		}
		else if(begining->type==_CONTINUE)
		{
		}
		else if(begining->type==_DEL)
		{
		}
		begining = begining->next;
	}
}


void printExpr(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
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

void printFuncDefStmt(struct FuncDefStmtInfo * funcdefstmt, int* nodeCount, std::vector<std::string>& dotTree)
{
}

void printContinueBreakStmt(int* nodeCount, std::vector<std::string>& dotTree)
{
}

void printReturnStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
}

void printDelStmt(struct ExprInfo * expr, int* nodeCount, std::vector<std::string>& dotTree)
{
}