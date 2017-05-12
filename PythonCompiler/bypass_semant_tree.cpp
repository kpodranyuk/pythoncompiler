#include "bypass_semant_tree.h"

// ����������� �� ���������
TreeTraversal::TreeTraversal()
{
}

bool TreeTraversal::containsString(std::vector<std::string>& vec, std::string str) const
{
	std::vector<std::string>::const_iterator iter;  // ��������� �������� ��� ������ �����
	bool contains = false;							// ��������� ���� ����, ��� ������ ������� � ������
	// ��� ������� �������� ������ � ���� �� ������� ��������..
	for(iter=vec.begin(); iter<vec.end()&&!contains; iter++) 
	{
		// ���������, ����� �� ������� ������ ������
		contains=(*iter).compare(str)==0;
	}
	return contains;
}

void TreeTraversal::deleteString(std::vector<std::string>& vec, std::string str)
{
	std::vector<std::string>::const_iterator iter;  // ��������� �������� ��� ������ �����
	// ��� ������� �������� ������ � ���� �� ������� ��������..
	for(iter=vec.begin(); iter<vec.end(); iter++) 
	{
		// ���������, ����� �� ������� ������ ������
		if((*iter).compare(str)==0)
		{
			vec.erase(iter);
			break;
		}
	}
}

// �������� ������ (������ �����)
void TreeTraversal::fixTree(struct StmtListInfo* root) throw(char*)
{
	if(root == NULL)
		throw "Root of tree is a null pointer\n";
	// ������ ��������� ���������
	this->gl_state=_MAIN_STATE;
	this->lc_state=_REGULAR_STATE;
	// ��������� �������� ������
	checkStatementList(root);
}

// ����������� ������ (������ �����)
void TreeTraversal::makeTables(const struct StmtListInfo* treeRoot)
{
}

// ������ ������������������ ������ (������ ����� - ?)
void TreeTraversal::printTree(const struct StmtListInfo* treeRoot)
{
}

void TreeTraversal::checkStatementList(struct StmtListInfo* root) throw(char*)
{
	// ������� ��������� ��������� �� ������� �������� ������
	struct StmtInfo* begining;
	// ������� ������ ������� ������ ���������
	begining = root->first;
	// ���� ������� ������� ������ �� ���������..
	while(begining!=NULL)
	{
		// � ����������� �� ���� ����/�������� �������� ��������������� �������
		// ��������� ����� �������� ��������� ����
		if(begining->type==_EXPR)
		{
			checkExpr(begining->expr);
		}
		else if(begining->type==_IF)
		{
			checkIfStmt(begining->ifstmt);
		}
		else if(begining->type==_FOR)
		{
			checkForStmt(begining->forstmt);
		}
		else if(begining->type==_WHILE)
		{
			checkWhileStmt(begining->whilestmt);
		}
		else if(begining->type==_FUNC_DEF)
		{
			checkFuncDefStmt(begining->funcdefstmt);
		}
		else if(begining->type==_RETURN)
		{
			checkReturnStmt(begining->expr);
		}
		else if(begining->type==_BREAK || begining->type==_CONTINUE)
		{
			//printContinueBreakStmt(_BREAK,nodeCount,dotTree);
		}
		else if(begining->type==_DEL)
		{
			checkDelStmt(begining->expr);
		}
		else
		{
			throw "Unrecognized type of tree node";
		}
		// ������� ��������� ������� ������ ����� �������
		begining = begining->next;
	}
}

void TreeTraversal::checkExpr(struct ExprInfo * expr) throw(char*)
{
	// ���� �������
	if(expr->type==_OPERAND)
	{
		// ���������, ��� �� ��� ��������
		std::string opName = std::string(expr->idName);
		if(!containsString(this->varNames,opName))
		{
			// ���� �� ��������, ������ ������ � ������ ��������
			char* errstr=new char[35+strlen(expr->idName)];
			errstr[0]='\0';
			strcpy(errstr,"Can't use undefined operand - ");
			strcat(errstr,expr->idName);
			throw errstr;
		}
	}
	// ���� ����������
	else if(expr->type==_ASSIGN)
	{
		// ����� �� ���������� ����� ���� ������ ���� �������, ���� ������ �� ������� �������
		// ���� ����� �������
		if(expr->left->type==_OPERAND)
		{
			// ���������, ���� �� �� ��� � ������ ����������
			// � ���� ���, �� ���������
			std::string opName = std::string(expr->left->idName);
			if(!containsString(this->varNames,opName))
			{
				this->varNames.push_back(opName);
			}
			// ��������� ������ ����� ����������
			checkExpr(expr->right);
		}
		// ����� ���� ����� ������ �� �������
		else if(expr->left->type==_ARRID)
		{
			// ���������, ��� ������ ������������ ��� �� ������������ �������
			struct ExprInfo* arr = expr->left;
			if(arr->left->type==_OPERAND)
				checkExpr(arr->left);
			// ����� ���� ������ �� ��������� � ��������, ����������� ����������
			else
				throw "Can't use [brackets] for anything except arrays.";
		}
		// ����� ������� ����������, ��� ������ ��������� �������� ����-�� ����� �������� ��� �������� �������
		else
			throw "Can't assign value to anything except operand or array element.";
	}
	// ���� �������� - ��, ��������� ���������
	else if(expr->type==_NOT)
	{
		checkExpr(expr->left);
	}
	// ���� �������� ��� �������� - append/remove 
	else if(expr->type==_ARRACT)
	{
		// ���������, ����� ������ ��� ��������� � ��� � ������ ����������
		if(expr->left->type==_OPERAND)
			checkExpr(expr->left);
		else
			throw "Can't use methods to anything except arrays.";
	}
	else if(expr->type==_FUNCCALL)
	{
		;
		// TODO ��������� ������� ��� ������ ������ � ���������� 
	}
	else
	{
		if(expr->type!=_VARVAL)
		{
			checkExpr(expr->left);
			checkExpr(expr->right);
		}
	}
	//TODO ����������� � ����������� ���������� �������� �������
	/*else if(expr->type==_ARRID_AND_ASSIGN)
	{
	}*/
}

void TreeTraversal::checkIfStmt(struct IfStmtInfo * ifstmt)
{
}

void TreeTraversal::checkWhileStmt(struct WhileStmtInfo * whilestmt)
{
	lc_state = _CYCLE_STATE;
	// ��� �������
	lc_state = _REGULAR_STATE;
}

void TreeTraversal::checkForStmt(struct ForStmtInfo * forstmt)
{
	lc_state = _CYCLE_STATE;
	// ��� �������
	lc_state = _REGULAR_STATE;
}

void TreeTraversal::checkFuncDefStmt(struct FuncDefInfo * funcdefstmt)
{
	enum GlobalState lastState = gl_state;
	if(lastState == _MAIN_STATE)
		gl_state = _FUNC_STATE;
	// ��� �������
	if(lastState == _MAIN_STATE)
		gl_state = _MAIN_STATE;
}

void TreeTraversal::checkContinueBreakStmt(enum StmtType type)
{
}

void TreeTraversal::checkReturnStmt(struct ExprInfo * expr) throw(char*)
{
	// ���� �� ������ ��������� � ������� ���� ���������, ������� ����������
	if(this->gl_state == _MAIN_STATE)
		throw "Found return in global code.";
	// ����� ��������� ������������ ���������
	checkExpr(expr);
}

void TreeTraversal::checkDelStmt(struct ExprInfo * expr)
{
	// �� ������ ������ ��������, ��� ���������� ����� ���� ������ �������
	// ������� ���������, �� ������ �� ��� �������
	checkExpr(expr);
	// ���� ������� ��� �� ������, ������� ��� �� ������� ���� ���������
	deleteString(this->varNames,std::string(expr->idName));
}

void TreeTraversal::checkFuncParams(struct DefFuncParamListInfo* params)
{
}