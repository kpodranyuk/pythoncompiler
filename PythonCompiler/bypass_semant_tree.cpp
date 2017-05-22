#include "bypass_semant_tree.h"

#pragma warning(disable : 4996)

/** ������ ����� ������� ��������
* �������: � ���������; � ������ (?); ��������
* ������ � ���� ������� � ������� csv, ����������� - ;
* ������ ���������: "Constant number";"String number";"Type";"Static";"Constant value";
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

struct TreeTraversal::TableElement* TreeTraversal::makeTableEl(int num, int strNum, enum TableElemType type, int isStatic, std::string val, int local)
{
	struct TableElement* te = new struct TableElement;
	te->number=num;
	te->strNumber=strNum;
	te->type=type;
	te->val=val;
	te->isStatic=isStatic;
	te->localFor=local;
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
	currentFuncName="global.csv";
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
	this->varNames.clear();
	this->funcNames.clear();
	ValNum=0;
	TypeNum=0;
	MassTypeNum=0;
	// ��������� ���� �������� �������
	FILE* tbl = fopen(currentFuncName.c_str(),"wt");
	// ����� ��������� �������
	fprintf(tbl,"%s\n","\"Constant number\";\"String number\";\"Type\";\"Static\";\"Constant value\";\"Local for\";");
	int constantNumber=1;
	// ��������� � ������� ��������� Code
	globalTable.push_back(makeTableEl((*constNumber)++,NULL,_UTF8,NULL,std::string("Code"),NULL));
	// ��������� � ������� ��������� ������ main
	globalTable.push_back(makeTableEl((*constNumber)++,NULL,_UTF8,NULL,std::string("Main"),NULL));
	// ������� ��� ��� ��������� ���� Class
	globalTable.push_back(makeTableEl((*constNumber)++,1,_CLASS,NULL,std::string("2"),NULL));
	// ��������� � ������� ��������� ������ Value
	globalTable.push_back(makeTableEl((*constNumber)++,NULL,_UTF8,NULL,std::string("Value"),NULL));
	// ������� ��� ��� ��������� ���� Class
	globalTable.push_back(makeTableEl((*constNumber)++,NULL,_CLASS,NULL,std::string("4"),NULL));
	ValNum=*constNumber-1;
	parseStmtListForTable(treeRoot,constNumber,NULL);
	std::vector<struct TableElement*>::iterator iter;  // ��������� �������� ��� ������ �����
	// ��� ������� �������� ������ � ���� �� ������� ��������..
	for(iter=globalTable.begin(); iter<globalTable.end(); iter++) 
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
		if((*iter)->isStatic==NULL)
			output+="\";";
		else if((*iter)->isStatic)
			output+="1\";";
		else
			output+="0\";";
		output+="\"";
		output+=(*iter)->val;
		output+="\";\"";
		if((*iter)->localFor==NULL)
			output+="\";";
		else 
		{
			sprintf(buf,"%d",(*iter)->localFor);
			output+=std::string(buf);
			output+="\";";
		}
		fprintf(tbl,"%s\n",output.c_str());
		//iter=table.erase(iter);
	}
}

void TreeTraversal::parseStmtListForTable(const struct StmtListInfo* root, int* constNum, int local)
{
	
	// ������� ��������� ��������� �� ������� �������� ������
	struct StmtInfo* begining;
	// ������� ������ ������� ������ ���������
	begining = root->first;
	// ���� ������� ������� ������ �� ���������..
	while(begining!=NULL)
	{
		// � ����������� �� ���� ����/�������� �������� ��������������� �������

		// �������� ������� ������ ��� ��� �����, ������ ������� �����
		// - ���� ���������� ���������� - � ������
		// - ���� ���������� ������� - � ������
		// - ���� ���������� ������� - ��������� �����
		if(begining->type==_EXPR)
		{
			//checkExpr(begining->expr);
			parseExprForTable(begining->expr,constNum, local);
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
			parseFuncDefForTable(begining->funcdefstmt,constNum,local);
		}
		// ������� ��������� ������� ������ ����� �������
		begining = begining->next;
	}
}

void TreeTraversal::parseExprForTable(const struct ExprInfo * expr, int* constNum, int local)
{
	// ���� ����������
	if(expr->type==_ASSIGN)
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
				// ������ �������� � ����
				if(TypeNum==0&&expr->right->type!=_ARRINIT)
				{
					globalTable.push_back(makeTableEl((*constNum)++,NULL,_UTF8,NULL,std::string("LValue;"), local));
					TypeNum=*constNum-1;
				}
				else if(MassTypeNum==0&&expr->right->type==_ARRINIT)
				{
					globalTable.push_back(makeTableEl((*constNum)++,NULL,_UTF8,NULL,std::string("[LValue;"), local));
					MassTypeNum=*constNum-1;
				}
				// ��������� � ������� ������ � ����������
				globalTable.push_back(makeTableEl((*constNum)++,expr->loc->firstLine,_UTF8,NULL,opName, local));
				// ������ NameAndType
				char buf[50]="";
				if(expr->right->type==_ARRINIT)
					sprintf(buf,"%d,%d",*constNum-1,MassTypeNum);
				else
					sprintf(buf,"%d,%d",*constNum-1,TypeNum);
				globalTable.push_back(makeTableEl((*constNum)++,expr->loc->firstLine,_NAMEnTYPE,NULL,std::string(buf), local));
				// ������ fieldRef
				buf[0]='\0';
				sprintf(buf,"%d,%d",ValNum,*constNum-1);
				globalTable.push_back(makeTableEl((*constNum)++,expr->loc->firstLine,_FIELDREF,NULL,std::string(buf), local));				
			}
			// ��������� ������ ����� ����������
			parseExprForTable(expr->right,constNum,local);//checkExpr(expr->right);
		}
		// ����� ���� ����� ������ �� �������
		/*else if(expr->left->type==_ARRID)
		{
			// �������, ��� ��������� ����� ����� �� ����� - �� ���� ��������� ����
			// ��������� ������ �����
			parseExprForTable(expr->right,table,constNum); //checkExpr(expr->right);
		}*/
	}
	if(expr->type==_VARVAL)
		parseValTypeForTable(expr->exprVal,constNum,local);
}

void TreeTraversal::parseValTypeForTable(const struct ValInfo * val, int* constNum, int local)
{
	if(val->type==_TRUE)
	{
		globalTable.push_back(makeTableEl((*constNum)++,val->loc->firstLine,_INT,NULL,std::string("1"), local));
	}
	else if(val->type==_FALSE)
	{
		globalTable.push_back(makeTableEl((*constNum)++,val->loc->firstLine,_INT,NULL,std::string("0"), local));
	}
	else if(val->type==_NUMBER)
	{
		char* str = new char[50];
		sprintf(str,"%d",val->intVal);
		globalTable.push_back(makeTableEl((*constNum)++,val->loc->firstLine,_INT,NULL,std::string(str), local));
	}
	else
	{
		globalTable.push_back(makeTableEl((*constNum)++,val->loc->firstLine,_UTF8,NULL,std::string(val->stringVal), local));
		int costNumForStr = *constNum-1;
		char* str = new char[50];
		sprintf(str,"%d",costNumForStr);
		// ��������� � ������� ������ � ����������
		globalTable.push_back(makeTableEl((*constNum)++,val->loc->firstLine,_STRING,NULL,std::string(str), local));
	}
		
}

void TreeTraversal::parseFuncDefForTable(const struct FuncDefInfo * funcdefstmt, int* constNum, int local)
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
		// ��������� ���
		std::string type="";
		if(curHeader->params->first==NULL)
			type+="()";
		else
		{
			struct DefFuncParamInfo* el = curHeader->params->first;
			type+="(";
			while(el!=NULL)
			{
				type+="LValue;";
				el=el->next;
			}
			type+=")";
		}
		type+="LValue;";
		globalTable.push_back(makeTableEl((*constNum)++,funcdefstmt->nameLoc->firstLine,_UTF8,1,type, local));
		// ��������� � ������� ������ � ����������
		globalTable.push_back(makeTableEl((*constNum)++,funcdefstmt->nameLoc->firstLine,_UTF8,1,std::string(curHeader->functionName), local));
		// ������ NameAndType
		char buf[50]="";
		sprintf(buf,"%d,%d",*constNum-2,*constNum-1);
		globalTable.push_back(makeTableEl((*constNum)++,funcdefstmt->nameLoc->firstLine,_NAMEnTYPE,1,std::string(buf), local));
		// ������ methodRef
		buf[0]='\0';
		sprintf(buf,"%d,%d",ValNum,*constNum-1);
		globalTable.push_back(makeTableEl((*constNum)++,funcdefstmt->nameLoc->firstLine,_METHODREF,1,std::string(buf), local));
		local=*constNum-1;
		//currentFuncName=std::string(curHeader->functionName);

		std::vector<struct TableElement*> funcTable;
		int* funcConsts = new int;
		*funcConsts=1;
		// ��������� �� ����
		parseStmtListForTable(funcdefstmt->body,constNum,local);//checkStatementList(funcdefstmt->body);
		//programm_table.insert(TablePair(std::string(curHeader->functionName),funcTable));
		//prog.push_back(TablePair(std::string(curHeader->functionName),funcTable));
	}
	// ����� ����������� ����������
	/*else
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
	}*/
	// ���� ��� ����� ������� �� ����������� ����, ������ ���������
	if(lastState == _MAIN_STATE)
	{
		gl_state = _MAIN_STATE;
		//currentFuncName="global.csv";
		local=NULL;
	}
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
		// � ������ ������� ��� ��������� - ��� ������� - �������
		// � ��������� - ��� ������ ����������� ����������
		// ����� ���������
		// 1. ����� ������� ���������
		// 2. ���������� ���������� ���������
		// 3. ��� ������ "� = .." ����� ���������� �� ��������
		// 4. ���������� ���������� ��-��������� ������� ��������
		// 5. �������� ���������� � ��� �� ������ �� ������
		std::string name = std::string(expr->idName);
		std::vector<struct FunctionHeader*>::const_iterator iter;  // ��������� �������� ��� ������ �������
		bool contains = false;							// ��������� ���� ����, ��� ������� �� ������� � ������
		// ��� ������� �������� ������ � ���� �� ������� ��������..
		for(iter=this->funcNames.begin(); iter<this->funcNames.end()&&!contains; iter++) 
		{
			// ���������, ����� �� ������� ������� ������
			contains=strcmp(name.c_str(),(*iter)->functionName);
		}
		// ���� ������� � ����� ������ ���������..
		if(contains)
		{
			struct DefFuncParamInfo* el = (*iter)->params->first;
			int count = 0;
			int count_of_def = 0;
			while(el!=NULL)
			{
				if(el->paramVal==NULL)
					count++;
				else
					count_of_def++;
				el=el->next;
			}
			struct ExprInfo* call_el = expr->arglist->first;
			int fact_count = 0;
			int fact_count_def = 0;
			std::vector<std::string> defaults;
			while(call_el!=NULL)
			{
				if(call_el->type==_ASSIGN)
				{
					fact_count_def++;
					if(call_el->left->type==_OPERAND)
						defaults.push_back(std::string(call_el->left->idName));
					else
						throw ""; // ��������� ����������
				}
				else
				{
					fact_count++;
				}
				checkExpr(call_el);
				if(call_el->next!=NULL)
				{
					if(call_el->next->type!=_ASSIGN&&call_el->type==_ASSIGN)
						throw ""; // ��������� ����������
				}
				call_el=call_el->next;
			}
			// ���� ���������� ���������� ���������
			if(count==fact_count)
			{
				if(count_of_def<=fact_count_def)
				{
					el = (*iter)->params->first;
					while(el!=NULL)
					{
						if(el->paramVal!=NULL)
						{
							if(!containsString(defaults,std::string(el->paramName)))
								throw ""; // ��������� ����������
						}
						el=el->next;
					}
				}
			}
			else
				throw ""; // ��������� ����������
		}
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
}

void TreeTraversal::checkIfStmt(struct IfStmtInfo * ifstmt) throw(char*)
{
	/*�������� ��_���� �� ������������*/
	checkExpr(ifstmt->expr);//�������� ��������� ���������
	checkStatementList(ifstmt->stmtlist);//�������� ���� if-�
	if(ifstmt->elsestmtlist!=NULL)
		checkStatementList(ifstmt->elsestmtlist);//�������� ���� else
	// �������� elif ������
	if(ifstmt->eliflist!=NULL)
	{
		struct ElifListInfo* begin=ifstmt->eliflist;
		while(begin!=NULL)
		{
			// �������� ��������� ���������
			checkExpr(begin->expr);
			// �������� ����
			checkStatementList(begin->stmtlist);
			begin=begin->next;
		}
	}

	/*���� ��� ���������, �� ����������� ��_���� � elif-���� � ��_���� ��� elif-��*/
	struct StmtListInfo* lastElseMainIf=ifstmt->elsestmtlist;//��������� ��������� else ����(�� ����� ����� ������ � ������)
	if(ifstmt->eliflist!=NULL)
	{
		struct IfStmtInfo* currentIf = ifstmt;
		struct ElifListInfo* elifListElement=ifstmt->eliflist;
		while(elifListElement!=NULL)
		{
			// ������� ����� ��
			struct IfStmtInfo * newIf = (struct IfStmtInfo *)malloc(sizeof(struct IfStmtInfo));
			newIf->expr=elifListElement->expr;
			newIf->stmtlist=elifListElement->stmtlist;
			// ������� ����� �� ����
			struct StmtInfo* newIfStmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
			newIfStmt->type=_IF;
			newIfStmt->ifstmt=newIf;
			newIfStmt->next=NULL;
			// ������� ����� ���� ����
			struct StmtListInfo* newStmtList = (struct StmtListInfo *)malloc(sizeof(struct StmtListInfo));
			newStmtList->first=newIfStmt;
			newStmtList->last=newIfStmt;
			currentIf->elsestmtlist=newStmtList;
			currentIf=newIf;
			currentIf->eliflist=NULL;
			elifListElement=elifListElement->next;
		}
		if(ifstmt->elsestmtlist!=NULL)
		{
			currentIf->elsestmtlist=lastElseMainIf;
		}
		ifstmt->eliflist=NULL;
	}
}

void TreeTraversal::checkWhileStmt(struct WhileStmtInfo * whilestmt) throw(char*)
{
	// �������� ��������� ������ � ��������� ��� ����� � ����
	bool cycle=false;
	if(lc_state==_REGULAR_STATE)
	{
		lc_state = _CYCLE_STATE;
		cycle=true;
	}
	
	// ��� �������
	checkExpr(whilestmt->expr);//�������� ��������� ��������� ����������� �����
	checkStatementList(whilestmt->stmtlist);//�������� ���� �����
	if(whilestmt->elsestmt!=NULL)
		checkStatementList(whilestmt->elsestmt);//�������� ����� else �����

	// ��������� ��������� ��� ������ �� �����
	if(cycle==true)
	{
		lc_state = _REGULAR_STATE;
	}
}

void TreeTraversal::checkForStmt(struct ForStmtInfo * forstmt) throw(char*)
{
	// �������� ��������� ������ � ��������� ��� ����� � ����
	bool cycle=false;
	if(lc_state==_REGULAR_STATE)
	{
		lc_state = _CYCLE_STATE;
		cycle=true;
	}
	
	// ��� �������
	// ���������, ���� �� �� ��� � ������ ����������
	// � ���� ���, �� ���������
	std::string opName = std::string(forstmt->counter);
	if(!containsString(this->varNames,opName))
	{
		this->varNames.push_back(opName);
	}
	checkExpr(forstmt->expr);//�������� ���������, �� ��� �������� ����
	checkStatementList(forstmt->stmtlist);//�������� ���� �����
	if(forstmt->elsestmt!=NULL)
		checkStatementList(forstmt->elsestmt);//�������� ����� else �����

	// ��������� ��������� ��� ������ �� �����
	if(cycle==true)
	{
		lc_state = _REGULAR_STATE;
	}
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
		// ��������� ��������� ������� � ������ ��������
		std::vector<std::string> onlyInHeader;
		struct DefFuncParamInfo* el = curHeader->params->first;
		while(el!=NULL)
		{
			if(!containsString(this->varNames,std::string(el->paramName)))
			{
				this->varNames.push_back(std::string(el->paramName));
				onlyInHeader.push_back(std::string(el->paramName));
			}
			el=el->next;
		}
		// ��������� �� ����
		checkStatementList(funcdefstmt->body);

		std::vector<std::string>::iterator iter;  // ��������� �������� ��� ������ �����
		// ��� ������� �������� ������..
		for(iter=onlyInHeader.begin(); iter<onlyInHeader.end(); iter++) 
		{
			deleteString(this->varNames,*iter);
		}
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
	case(_STRING):
		str="String";
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