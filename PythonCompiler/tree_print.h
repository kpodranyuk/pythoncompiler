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