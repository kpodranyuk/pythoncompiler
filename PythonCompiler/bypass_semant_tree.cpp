#include "bypass_semant_tree.h"

#pragma warning(disable : 4996)

/** ������ ����� ������� ��������
* �������: � ���������; � ������ (?); ��������
* ������ � ���� ������� � ������� csv, ����������� - ;
* ������ ���������: "Constant number";"String number";"Constant value";
* ������ ������ ���������: 1;;"utf-8";"Code"
* ������� �������� ��� ����������� ���� ������ ������ � ���� global.csv
* ������� �������� ��� ������� ������ � ���� <func_name>.csv
**/

/* ����������� �� ������:
* !!! ������� ����� ������ ���������� ����������, �� ���������� ��� ���������� ������� - ��� !!!
* !!! ������� ���������� ��������� ����������, �� �� ������ �� �������� (�� ��������, �� �� ������) !!!
*/

// ����������� �� ���������
TreeTraversal::TreeTraversal()
{
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

TreeTraversal::TableElement* TreeTraversal::makeTableEl(int num, int strNum, enum TableElemType type, std::string val)
{
	struct TableElement* te = new struct TableElement;
	te->number=num;
	te->strNumber=strNum;
	te->type=type;
	te->val=val;
	return te;
}

// ����������� ������ (������ �����)
void TreeTraversal::makeTables(const struct StmtListInfo* treeRoot)
{
	
	if(treeRoot == NULL)
		throw "Root of tree is a null pointer\n";
	currentFuncName=std::string("");
	// ������ ��������� ���������
	this->gl_state=_MAIN_STATE;
	this->lc_state=_REGULAR_STATE;
	// ��������� �������� ������
	//checkStatementList(root);
	currentFuncName="global";
	int* constNumber = new int;
	*constNumber=1;
	/**
	* � ������� ����������� ������ ������ (��� ��������, � ��� ������ main � value)
	* ���� ������ - �� ����, ���� ������� ��� ���������� ����������� ���� ��� ��������� ������ main
	* ������ ������ - �������
	* ������ ������� - ������� ����������� ����
	* �� ���� ������� ����������� ������ ���
	* - ���������� ����������
	* - ���������� �������
	* - ���������� �������
	*/
	parseStmtListForTable(treeRoot,this->globalTable,constNumber);
}

void TreeTraversal::parseStmtListForTable(const struct StmtListInfo* root, std::vector<struct TableElement*>& table, int* constNum)
{
	currentFuncName+=".csv";
	// ��������� ���� �������� �������
	FILE* tbl = fopen(currentFuncName.c_str(),"wt");
	// ����� ��������� �������
	fprintf(tbl,"%s\n","\"Constant number\";\"String number\";\"Type\";\"Constant value\"");
	int constantNumber=1;
	// ��������� � ������� ��������� Code
	table.push_back(makeTableEl((*constNum)++,NULL,_UTF8,std::string("Code")));
	// ��������� � ������� ��������� ������ main
	table.push_back(makeTableEl((*constNum)++,NULL,_UTF8,std::string("Main")));
	// ������� ��� ��� ��������� ���� Class
	table.push_back(makeTableEl((*constNum)++,1,_CLASS,std::string("2")));
	// ��������� � ������� ��������� ������ Value
	table.push_back(makeTableEl((*constNum)++,NULL,_UTF8,std::string("Value")));
	// ������� ��� ��� ��������� ���� Class
	table.push_back(makeTableEl((*constNum)++,NULL,_CLASS,std::string("4")));
	ValNum=*constNum-1;
	// ������ �������� � ����
	table.push_back(makeTableEl((*constNum)++,NULL,_UTF8,std::string("LValue;")));
	TypeNum=*constNum-1;
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
			//checkExpr(begining->expr);
			parseExprForTable(begining->expr,table,constNum);
		}
		else if(begining->type==_IF)
		{
			//checkIfStmt(begining->ifstmt);
		}
		else if(begining->type==_FOR)
		{
			//checkForStmt(begining->forstmt);
		}
		else if(begining->type==_WHILE)
		{
			//checkWhileStmt(begining->whilestmt);
		}
		else if(begining->type==_FUNC_DEF)
		{
			//checkFuncDefStmt(begining->funcdefstmt);
		}
		else if(begining->type==_RETURN)
		{
			//checkReturnStmt(begining,begining->expr);
		}
		else if(begining->type==_BREAK || begining->type==_CONTINUE)
		{
			// ���� �� ������ ��������� � ������� ���� ���������, ������� ����������
			/*if(this->gl_state == _MAIN_STATE)
			{
				// ��������� ������ � �������� ������
				char* bufstr = new char [50];
				sprintf(bufstr,"(%d.%d-%d.%d)",begining->loc->firstLine,begining->loc->firstColumn,begining->loc->lastLine,begining->loc->lastColumn);
				// ��������� ��� � ������ ���������
				char* errStr = new char[31+62];
				if(begining->type==_BREAK)
					strcpy(errStr,"Found break in global code.");
				else if(begining->type==_CONTINUE)
					strcpy(errStr,"Found continue in global code.");
				strcat(errStr,"\nLocation: ");
				strcat(errStr,bufstr);
				// ������� ������ �����������
				throw errStr;
			}*/
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
	std::vector<struct TableElement*>::iterator iter;  // ��������� �������� ��� ������ �����
	// ��� ������� �������� ������ � ���� �� ������� ��������..
	for(iter=table.begin(); iter<table.end(); iter++) 
	{
		std::string output="";
		char buf[50]="";
		output+="\"";
		sprintf(buf,"%d",(*iter)->number);
		output+=std::string(buf);
		output+="\";\"";
		if((*iter)->strNumber!=NULL)
		{
			sprintf(buf,"%d",(*iter)->strNumber);
			output+=std::string(buf);
		}
		output+="\";\"";
		output+=convertTypeToString((*iter)->type);
		output+="\";\"";
		output+=(*iter)->val;
		output+="\";";
		fprintf(tbl,"%s\n",output.c_str());
		//iter=table.erase(iter);
	}
}

void TreeTraversal::parseExprForTable(const struct ExprInfo * expr, std::vector<struct TableElement*>& table, int* constNum)
{
	// ���� �������
	if(expr->type==_OPERAND)
	{
		// ���������������, ��� ������ ��� ���������� � ��� ���������
		// ��� ��� ����� ������ �� ��������
		;
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
				// ��������� � ������� ������ � ����������
				table.push_back(makeTableEl((*constNum)++,expr->loc->firstLine,_UTF8,opName));
				// ������ NameAndType
				char buf[50]="";
				sprintf(buf,"%d,%d",*constNum-1,TypeNum);
				table.push_back(makeTableEl((*constNum)++,expr->loc->firstLine,_NAMEnTYPE,std::string(buf)));
				// ������ fieldRef
				buf[0]='\0';
				sprintf(buf,"%d,%d",ValNum,*constNum-1);
				table.push_back(makeTableEl((*constNum)++,expr->loc->firstLine,_FIELDREF,std::string(buf)));				
			}
			// ��������� ������ ����� ����������
			parseExprForTable(expr->right,table,constNum);//checkExpr(expr->right);
		}
		// ����� ���� ����� ������ �� �������
		else if(expr->left->type==_ARRID)
		{
			// �������, ��� ��������� ����� ����� �� ����� - �� ���� ��������� ����
			// ��������� ������ �����
			parseExprForTable(expr->right,table,constNum); //checkExpr(expr->right);
		}
	}
	// ���� �������� ��� �������� - append/remove 
	else if(expr->type==_ARRACT)
	{
		// �������, ��� ��������� ����� ����� �� ����� - �� ���� ��������� ����
		// ��������� ������ �����
		parseExprForTable(expr->right,table,constNum);//checkExpr(expr->right);
	}
	// ���� ������������� ������� 
	else if(expr->type==_ARRINIT)
	{
		// �������, ��� ��������� ����� ����� �� ����� - �� ���� ��������� ����
		// ������� ��������� ��������� �� ������ ������ ���������
		struct ExprInfo* begining;
		// ������� ������ ������� ������ ���������
		begining = expr->arglist->first;
		// ���� ������� ������� ������ �� ���������..
		while(begining!=NULL)
		{
			// ��������� ������� ������� ������
			parseExprForTable(begining,table,constNum); //checkExpr(begining);
			// ������� ��������� ������� ������ ����� �������
			begining = begining->next;
		}
	}
	/*else if(expr->type==_FUNCCALL)
	{
		;
		// TODO ��������� ������� ��� ������ ������ � ���������� 
	}*/
	else
	{
		if(expr->type!=_VARVAL)
		{
			parseExprForTable(expr->left,table,constNum);
			parseExprForTable(expr->right,table,constNum);
			//checkExpr(expr->left);
			//checkExpr(expr->right);
		}
	}
	//TODO ����������� � ����������� ���������� �������� �������
	/*else if(expr->type==_ARRID_AND_ASSIGN)
	{
	}*/
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
			checkReturnStmt(begining,begining->expr);
		}
		else if(begining->type==_BREAK || begining->type==_CONTINUE)
		{
			// ���� �� ������ ��������� �� � �����, ������� ����������
			if(this->lc_state == _REGULAR_STATE)
			{
				// ��������� ������ � �������� ������
				char* bufstr = new char [50];
				sprintf(bufstr,"(%d.%d-%d.%d)",begining->loc->firstLine,begining->loc->firstColumn,begining->loc->lastLine,begining->loc->lastColumn);
				// ��������� ��� � ������ ���������
				char* errStr = new char[31+62];
				if(begining->type==_BREAK)
					strcpy(errStr,"Found break in global code.");
				else if(begining->type==_CONTINUE)
					strcpy(errStr,"Found continue in global code.");
				strcat(errStr,"\nLocation: ");
				strcat(errStr,bufstr);
				// ������� ������ �����������
				throw errStr;
			}
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
			char* bufstr = new char [50];
			sprintf(bufstr,"(%d.%d-%d.%d)",expr->loc->firstLine,expr->loc->firstColumn,expr->loc->lastLine,expr->loc->lastColumn);
			// ���� �� ��������, ������ ������ � ������ ��������
			char* errstr=new char[35+strlen(expr->idName)+62];
			errstr[0]='\0';
			strcpy(errstr,"Can't use undefined operand - ");
			strcat(errstr,expr->idName);
			strcat(errstr,"\nLocation: ");
			strcat(errstr,bufstr);
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
		// ����� ���� ����� ������ �� �������, �� ������ ��� �������� � _ASSIGN �� _ARRID_AND_ASSIGN
		else if(expr->left->type==_ARRID)
		{
			// ��������� ������ �����
			checkExpr(expr->right);
			// ���������, ��� ������ ������������ ��� �� ������������ �������(���� ������� �������)
			struct ExprInfo* arr = expr->left;
			if(arr->left->type==_OPERAND)
				checkExpr(arr->left);
			
			// �������� ����
			expr->type=_ARRID_AND_ASSIGN;
			struct ExprInfo* leftNode = expr->left;
			expr->left=leftNode->left;
			expr->middle=leftNode->right;
			leftNode->left=NULL;
			leftNode->right=NULL;

		}
		// ����� ������� ����������, ��� ������ ��������� �������� ����-�� ����� �������� ��� �������� �������
		else
		{
			char* bufstr = new char [50];
			sprintf(bufstr,"(%d.%d-%d.%d)",expr->loc->firstLine,expr->loc->firstColumn,expr->loc->lastLine,expr->loc->lastColumn);
			// ���� �� ��������, ������ ������ � ������ ��������
			char* errstr=new char[64+62];
			errstr[0]='\0';
			strcpy(errstr,"Can't assign value to anything except operand or array element.");
			strcat(errstr,"\nLocation: ");
			strcat(errstr,bufstr);
			throw errstr;
		}
	}
	// ���� �������� - ��, ��������� ���������
	else if(expr->type==_NOT)
	{
		checkExpr(expr->left);
	}
	// ���� �������� ��� �������� - append/remove 
	else if(expr->type==_ARRACT)
	{
		// ���������, ��� �� �������� ���� ���� append ���� remove
		if(!(strcmp(expr->idName,"append")==0 || strcmp(expr->idName,"remove")==0))
		{
			char* bufstr = new char [50];
			sprintf(bufstr,"(%d.%d-%d.%d)",expr->loc->firstLine,expr->loc->firstColumn,expr->loc->lastLine,expr->loc->lastColumn);
			char* errstr=new char[64+62];
			errstr[0]='\0';
			strcpy(errstr,"There is no such operation on the list.");
			strcat(errstr,"\nLocation: ");
			strcat(errstr,bufstr);
			throw errstr;
		}
		// ���������, ����� ������ ��� ��������� � ��� � ������ ����������
		if(expr->left->type==_OPERAND)
			checkExpr(expr->left);

		// ��������� ������ �����
		checkExpr(expr->right);
	}
	// ���� ������������� ������� 
	else if(expr->type==_ARRINIT)
	{
		// �������, ��� ��������� ����� ����� �� ����� - �� ���� ��������� ����
		// ������� ��������� ��������� �� ������ ������ ���������
		struct ExprInfo* begining;
		// ������� ������ ������� ������ ���������
		begining = expr->arglist->first;
		// ���� ������� ������� ������ �� ���������..
		while(begining!=NULL)
		{
			// ��������� ������� ������� ������
			checkExpr(begining);
			// ������� ��������� ������� ������ ����� �������
			begining = begining->next;
		}
	}
	else if(expr->type==_FUNCCALL)
	{
		;
		// TODO ��������� ������� ��� ������ ������ � ���������� 
	}
	else if(expr->type==_UMINUS)
	{
		// ������� ����� ����� ���� ������ ����� ����� ������ ��� ����������, ������������ ����� �����
		// �������� ������
		if(expr->left->type==_VARVAL)
		{
			struct ValInfo* val = expr->left->exprVal;
			if(val->type!=_NUMBER)
			{
				char* bufstr = new char [50];
				sprintf(bufstr,"(%d.%d-%d.%d)",val->loc->firstLine,val->loc->firstColumn,val->loc->lastLine,val->loc->lastColumn);
				// ���� �� ��������, ������ ������ � ������ ��������
				char* errstr=new char[55+62];
				errstr[0]='\0';
				strcpy(errstr,"Can't make negative value of anything except integer.");
				strcat(errstr,"\nLocation: ");
				strcat(errstr,bufstr);
				throw errstr;
			}
		}
		else
			checkExpr(expr->left);
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
	// ��������� ������� ��������� �����������
	enum GlobalState lastState = gl_state;
	// ���� ����� ������� �� ����������� ����, ������ ���������
	if(lastState == _MAIN_STATE)
		gl_state = _FUNC_STATE;
	// ��� �������
	// ������� ��������� �� ��������� � ���������� ������� �������
	struct FunctionHeader* curHeader = new struct FunctionHeader;
	// �������� � ���� ��� �������
	curHeader->functionName = new char [strlen(funcdefstmt->functionName)+1];
	strcpy(curHeader->functionName,funcdefstmt->functionName);
	// � �������� ��������� �� ������ ����������
	curHeader->params=funcdefstmt->params;
	// ���� ����� ������� ��� �� ���� ���������
	if(!containsFuncHeader(this->funcNames,curHeader))
	{
		// ��������� �� ��������� � ������ �������
		this->funcNames.push_back(curHeader);
		// ��������� �� ����
		checkStatementList(funcdefstmt->body);
	}
	// ����� ����������� ����������
	else
	{
		char* bufstr = new char [50];
		sprintf(bufstr,"(%d.%d-%d.%d)",funcdefstmt->nameLoc->firstLine,funcdefstmt->nameLoc->firstColumn,funcdefstmt->nameLoc->lastLine,funcdefstmt->nameLoc->lastColumn);
		// ���� �� ��������, ������ ������ � ������ ��������
		char* errStr = new char[30+strlen(curHeader->functionName)+62];
		strcpy(errStr,"Can't define same function: ");
		strcat(errStr,curHeader->functionName);
		strcat(errStr,"\nLocation: ");
		strcat(errStr,bufstr);
		throw errStr;
	}
	// ���� ��� ����� ������� �� ����������� ����, ������ ���������
	if(lastState == _MAIN_STATE)
		gl_state = _MAIN_STATE;
}

void TreeTraversal::checkContinueBreakStmt(enum StmtType type)
{
}

void TreeTraversal::checkReturnStmt(struct StmtInfo* retStmt, struct ExprInfo * expr) throw(char*)
{
	// ���� �� ������ ��������� � ������� ���� ���������, ������� ����������
	if(this->gl_state == _MAIN_STATE)
	{
		char* bufstr = new char [50];
		sprintf(bufstr,"(%d.%d-%d.%d)",retStmt->loc->firstLine,retStmt->loc->firstColumn,retStmt->loc->lastLine,retStmt->loc->lastColumn);
		// ���� �� ��������, ������ ������ � ������ ��������
		char* errStr = new char[30+62];
		strcpy(errStr,"Found return in global code.");
		strcat(errStr,"\nLocation: ");
		strcat(errStr,bufstr);
		throw errStr;
	}
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


/*!
*	---------------- ���� ��������������� ������� ��� ������ �� �������� � ������������ ----------------
*/

std::string TreeTraversal::convertTypeToString(enum TableElemType type)
{
	std::string str="";
	switch (type)
	{
	case(_UTF8):
		str="UTF-8";
		break;
	case(_INT):
		str="INT";
		break;
	case(_NAMEnTYPE):
		str="Name&Type";
		break;
	case(_FIELDREF):
		str="FieldRef";
		break;
	case(_METHODREF):
		str="MethodRef";
		break;
	case(_CLASS):
		str="Class";
		break;		
	}
	return str;
}

bool TreeTraversal::isEqualFuncHeaders(struct FunctionHeader* first, struct FunctionHeader* second) const
{
	return strcmp(first->functionName,second->functionName)==0&&isEqualDefFuncParams(first->params,second->params);
}

bool TreeTraversal::isEqualDefFuncParams(struct DefFuncParamListInfo* first, struct DefFuncParamListInfo* second) const
{
	struct DefFuncParamInfo* beginFirst = first->first;
	struct DefFuncParamInfo* beginSecond = second->first;
	while(beginFirst!=NULL&&beginSecond!=NULL)
	{
		// �����, ����� ����� �������� � �������� (���� ����)
		// ���� ������� ����� ���������� - ������ �� �����
		if(strcmp(beginFirst->paramName,beginSecond->paramName)!=0)
			return false;
		// ���� �� ����� �������� ���������� - ������ �� �����
		if(!isEqualVarVals(beginFirst->paramVal,beginSecond->paramVal))
			return false;
		// ��� ��� �������� �����, ���������� � ��������� ��������� ���������
		beginFirst = beginFirst->next;
		beginSecond = beginSecond->next;
	}
	// ���� ������ �����, �� ��� ��������� - ����, ����� - �� �����
	return beginFirst==NULL&&beginSecond==NULL;
}

bool TreeTraversal::isEqualVarVals(struct ValInfo* first, struct ValInfo* second) const
{
	// ���� ��� ����, �� �����
	if(first==NULL&&second==NULL)
		return true;
	// ���� ��� �� ����..
	else if(first!=NULL&&second!=NULL)
	{
		// ���� ����� ����, �������� ������ �����, ���������� �������� � ��������� ��������, �� �����
		return (first->type==second->type&&
			first->intVal==second->intVal&&
			first->logVal==second->logVal&&
			(first->stringVal==NULL&&second->stringVal==NULL||
			first->stringVal!=NULL&&second->stringVal!=NULL&&
			strcmp(first->stringVal,second->stringVal)==0));
	}
	return false;
}

bool TreeTraversal::containsFuncHeader(std::vector<struct FunctionHeader*>& vec, struct FunctionHeader* func) const
{
	std::vector<struct FunctionHeader*>::const_iterator iter;  // ��������� �������� ��� ������ �������
	bool contains = false;							// ��������� ���� ����, ��� ������� �� ������� � ������
	// ��� ������� �������� ������ � ���� �� ������� ��������..
	for(iter=vec.begin(); iter<vec.end()&&!contains; iter++) 
	{
		// ���������, ����� �� ������� ������� ������
		contains=isEqualFuncHeaders((*iter),func);
	}
	return contains;
}

void TreeTraversal::deleteFuncHeader(std::vector<struct FunctionHeader*>& vec, struct FunctionHeader* func)
{
	std::vector<struct FunctionHeader*>::iterator iter;  // ��������� �������� ��� ������ �������
	// ��� ������� �������� ������ � ���� �� ������� ��������..
	for(iter=vec.begin(); iter<vec.end(); iter++) 
	{
		// ���������, ����� �� ������� ������� ������
		if(isEqualFuncHeaders((*iter),func))
		{
			// ������� ������ ��������
			vec.erase(iter);
			// ������� �� �����
			break;
		}
	}
}

bool TreeTraversal::containsString(std::vector<std::string>& vec, std::string str) const
{
	std::vector<std::string>::const_iterator iter;  // ��������� �������� ��� ������ �����
	bool contains = false;							// ��������� ���� ����, ��� ������ �� ������� � ������
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
	std::vector<std::string>::iterator iter;  // ��������� �������� ��� ������ �����
	// ��� ������� �������� ������ � ���� �� ������� ��������..
	for(iter=vec.begin(); iter<vec.end(); iter++) 
	{
		// ���������, ����� �� ������� ������ ������
		if((*iter).compare(str)==0)
		{
			// ������� ������ ��������
			vec.erase(iter);
			// ������� �� �����
			break;
		}
	}
}