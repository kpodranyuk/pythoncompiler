#include "bypass_semant_tree.h"

#pragma warning(disable : 4996)


bool TreeTraversal::exprContainsAssign(struct ExprInfo * expr)
{
	if(expr->type==_OPERAND || expr->type==_VARVAL)
		return false;
	if(expr->type==_ASSIGN || expr->type==_ARRID_AND_ASSIGN)
		return true;
	else
	{
		bool left=false, middle=false, right=false;
	
		if(expr->left!=NULL)
			left=exprContainsAssign(expr->left);
		if(expr->type==_ARRID_AND_ASSIGN)
			middle=exprContainsAssign(expr->middle);
		if(expr->right!=NULL)
			right=exprContainsAssign(expr->right);

		return left || middle || right;
	}
}

// �������� ������ (������ �����)
void TreeTraversal::fixTree(struct StmtListInfo* root) throw(char*)
{
	if(root == NULL)
		throw makeStringForException("Root of tree is a null pointer\n",NULL);
	// ������ ��������� ���������
	this->gl_state=_MAIN_STATE;
	this->lc_state=_REGULAR_STATE;
	currentFuncName="global";
	this->varDecls["global"].push_back("");
	this->varDecls["global"].clear();
	// ��������� �������� ������
	checkStatementList(root);
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
			checkExpr(begining->expr,false);
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
			checkContinueBreakStmt(begining);
		}
		else if(begining->type==_DEL)
		{
			checkDelStmt(begining->expr);
		}
		else
		{
			throw makeStringForException("Unrecognized type of tree node",NULL);
		}
		// ������� ��������� ������� ������ ����� �������
		begining = begining->next;
	}
}

void TreeTraversal::checkExpr(struct ExprInfo * expr, bool assign) throw(char*)
{
	// ���� ������� (�������������)
	if(expr->type==_OPERAND)
	{
		// �������� ��� ��������
		std::string opName = std::string(expr->idName);
		// ���������, ��� ��� �������� �� ��������� � ������ ������������ �������
		if(containsString(this->libFuncs,opName))
		{
			// ���� �� ��������, ������ ������ � ������ ��������
			char* errstr=new char[40+strlen(expr->idName)];
			errstr[0]='\0';
			strcpy(errstr,"Can't name operand as lib function - ");
			strcat(errstr,expr->idName);
			throw makeStringForException(errstr,expr->loc);
		}
		bool operandExists=false;
		// ���������, ��� �� ��� ��������
		// ���� ������� ���������� � ���������� ����
		// ��������� ������ ��� ������� � ������ ���������� �� ����� global
		if(this->gl_state==_MAIN_STATE)
			operandExists=containsString(this->varDecls.at("global"),opName);
		// ����� ���� ������� ���������� ������ �������
		else
		{
			// 1. ��������� ��� ������� � ������ ���������� ������ ��������� �������
			operandExists=containsString(this->varDecls.at(currentFuncName),opName);
			// 2. ���� ���������� �� ���������, �� ���� �� � ���������� ����..
			if(!operandExists)
			{
				operandExists=containsString(this->varDecls.at("global"),opName);
				// ���� ������� ������������� ���������� ����������, �� ��������� �� ��� � usage
				if(operandExists)
					if(!containsString(varUsage[currentFuncName],opName))
						varUsage[currentFuncName].push_back(opName);
			}
		}
		// 3. ���� ���������� �� �������, ����������� ����������
		if(!operandExists)
		{
			// ���� �� ��������, ������ ������ � ������ ��������
			char* errstr=new char[35+strlen(expr->idName)];
			errstr[0]='\0';
			strcpy(errstr,"Can't use undefined operand - ");
			strcat(errstr,expr->idName);
			throw makeStringForException(errstr,expr->loc);
		}

	}
	// ���� ����������
	else if(expr->type==_ASSIGN)
	{
		// ���� ������������ ��� ����, �� ������ ����������
		if(assign)
			throw makeStringForException("Can't use assignment in the expression.",expr->loc);//("Assignment operation can not be more than 2 times in the expression.",expr->loc);
		// ����� �� ���������� ����� ���� ������ ���� �������, ���� ������ �� ������� �������
		// ���� ����� �������
		if(expr->left->type==_OPERAND)
		{
			std::string opName = std::string(expr->left->idName);
			// ���������, ��� ��� �������� �� ��������� � ������ ������������ �������
			if(containsString(this->libFuncs,opName))
			{
				// ���� �� ��������, ������ ������ � ������ ��������
				char* errstr=new char[40+strlen(expr->idName)];
				errstr[0]='\0';
				strcpy(errstr,"Can't name operand as lib function - ");
				strcat(errstr,expr->idName);
				throw makeStringForException(errstr,expr->loc);
			}
			// 1. ���� ��� ����������..
			if(this->gl_state==_MAIN_STATE)
			{
				// 1.1. ���������, ��������� �� ��� ����� ����������
				// 1.2. ���� ���������� �� ���������, �� ��������� ���������� � �� ����������
				if(!containsString(this->varDecls.at("global"),opName))
					this->varDecls["global"].push_back(opName);
				// ������� �������
				if(containsString(this->funcNames,opName))
				{
					deleteString(this->funcNames,opName);
					deleteFuncHeader(this->funcHeaders,opName);
				}
			}
			// 2. ���� ��� ���������..
			else
			{
				// 2.1. ���������, ���� �� ��������� ���������� � ����� ������
				// 2.2. ���� ���������� � ����� ������ ���
				if(!containsString(this->varDecls.at(currentFuncName),opName))
				{
					// 2.2.1. ���������, ���� �� ���������� ���������� � ����� ������
					// 2.2.2. ���� ����� ���������� ���� � ���������� ����, �� ���������, �������������� �� ��� �� ����� � ���� �������
					if(containsString(this->varDecls.at("global"),opName))
					{
						// 2.2.2.1. ���� ���������� ��������������, ������ ������
						if(containsString(this->varUsage.at(currentFuncName),opName))
						{
							// ���� �� ��������, ������ ������ � ������ ��������
							char* errstr=new char[35+strlen(opName.c_str())];
							errstr[0]='\0';
							strcpy(errstr,"Variable referenced before assignment - ");
							strcat(errstr,opName.c_str());
							throw makeStringForException(errstr,expr->loc);
						}
						else
							this->varDecls[currentFuncName].push_back(opName);
						// 2.2.2.2. ����� ��������� ���������� � ������ ��������� ����������
					}
					else
						this->varDecls[currentFuncName].push_back(opName);
					// ������� �������
					if(containsString(this->funcNames,opName))
					{
						deleteString(this->funcNames,opName);
						deleteFuncHeader(this->funcHeaders,opName);
					}
				}
			}
			// ��������� ������ ����� ����������
			checkExpr(expr->right,true);
		}
		// ����� ���� ����� ������ �� �������, �� ������ ��� �������� � _ASSIGN �� _ARRID_AND_ASSIGN
		else if(expr->left->type==_ARRID)
		{
			// ��������� ������ �����
			checkExpr(expr->right, true);
			// ��������� ����� �����
			checkExpr(expr->left,true);
			
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
			throw makeStringForException("Can't assign value to anything except operand or array element.",expr->loc);
	}
	// �������� �� ��, ��� �� �� ������ �������� �� ������� �� ���� �������� "="
	else if(expr->type==_ARRID)
	{
		// ��������� ����� �����
		checkExpr(expr->left,assign);
		// ��������� ������ �����
		if(exprContainsAssign(expr->right))
			throw makeStringForException("Can't use assignment operation in expression for array index.",expr->loc);

		checkExpr(expr->right,assign);
	}
	// ���� �������� - ��, ��������� ���������
	else if(expr->type==_NOT)
	{
		// �������� �� ������������ ������ ���������(����� ��� �� ���������)
		if(exprContainsAssign(expr->left))
			throw makeStringForException("Can't use assignment operation in NOT expression.",expr->loc);
		checkExpr(expr->left,assign);
	}
	// ���� �������� ��� �������� - append/remove 
	else if(expr->type==_ARRACT)
	{
		// ���������, ��� �� �������� ���� ���� append ���� remove
		if(!(strcmp(expr->idName,"append")==0 || strcmp(expr->idName,"remove")==0))
			throw makeStringForException("There is no such operation on the list.",expr->loc);
		// ��������� ����� �����
		checkExpr(expr->left,assign);
		// ��������� ������ �����
		if(exprContainsAssign(expr->right))
			throw makeStringForException("Assignment operation should not be an argument.",expr->loc);
		checkExpr(expr->right,assign);
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
			// �������� �� ������������ ������ ���������
			if(exprContainsAssign(begining))
				throw makeStringForException("Can't use assignment operation in expression for array initialize.",expr->loc);
			// ��������� ������� ������� ������
			checkExpr(begining,assign);
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
		if(!isLibFunctionCall(expr))
		{
			std::vector<struct FunctionHeader*>::const_iterator iter;  // ��������� �������� ��� ������ �������
				bool contains = false;							// ��������� ���� ����, ��� ������� �� ������� � ������
		// ��� ������� �������� ������ � ���� �� ������� ��������..
			for(iter=this->funcHeaders.begin(); iter<this->funcHeaders.end()&&!contains; iter++) 
			{
				// ���������, ����� �� ������� ������� ������
				contains=strcmp(name.c_str(),(*iter)->functionName)==0;
				if(contains)
					break;
			}
			// ���� ������� � ����� ������ ���������..
			if(contains)
			{
				// ������� ��������� �� ������� ������ ���������� ���������� �������
				struct DefFuncParamInfo* el = (*iter)->params->first;
				// ������� ���������� ���������� �������
				int count = 0;
				// ������� ���������� ���������� ������� �� ���������
				int count_of_def = 0;
				std::vector<std::string> def_defaults;
				// ���� �� �������� ��� ���������
				// ������� ���������� ������� ���������� � �� ���������
				while(el!=NULL)
				{
					if(el->paramVal==NULL)
						count++;
					else
					{
						count_of_def++;
						def_defaults.push_back(el->paramName);
					}
					el=el->next;
				}
				// ������� ��������� �� ������� ������ ���������� ������ �������
				struct ExprInfo* call_el = expr->arglist->first;
				// ������� ������������ ���������� ����������
				int fact_count = 0;
				// ������� ����������� ���������� �������
				int fact_count_def = 0;
				std::vector<std::string> defaults;
				// ��� ������� ������������ ���������
				while(call_el!=NULL)
				{
					// ���� ��� - ����������
					if(call_el->type==_ASSIGN)
					{
						// ����������� ������� ����������� ����������
						fact_count_def++;
						// ���� ����� - �������, �� ��������� ��� ���
						if(call_el->left->type==_OPERAND)
						{
							// ���������, ��� ��� ��������� ���������� � ������ ���������� �������
							if(!containsString(def_defaults,std::string(call_el->left->idName)))
								throw makeStringForException("Can't assign value to param that isn't default",call_el->loc); 
						}
						// ����� ����������� ����������
						else
							throw makeStringForException("Can't assign value to anything except operand",call_el->loc); 
						if(exprContainsAssign(call_el->right))
							throw makeStringForException("Can't use multiple assignment",call_el->loc); 
						checkExpr(call_el->right,false);
					}
					// �����..
					else
					{
						//����������� ������� ����������� ����������
						fact_count++;
						checkExpr(call_el,true);
					}
					if(call_el->next!=NULL)
					{
						if(call_el->next->type!=_ASSIGN&&call_el->type==_ASSIGN)
							throw makeStringForException("Wrong order of factual parametrs",call_el->loc);
					}
					call_el=call_el->next;
				}
				// ���� ���������� ���������� ���������
				if(count==fact_count)
				{
					// ���� ���������� ���������� �� ��������� � ���������� ������, ��� ��� ������
					if(count_of_def<fact_count_def)
						throw makeStringForException("Wrong amount of func parametrs in call",expr->loc);					
				}
				else
					throw makeStringForException("Wrong amount of params for function call",expr->loc); // ��������� ����������
			}
			else
				throw makeStringForException("Can't call undefined function",expr->loc); // ��������� ����������
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
				throw makeStringForException("Can't make negative value of anything except integer.",val->loc);
			val->intVal*=-1;
		}
		else
			checkExpr(expr->left,assign);
	}
	else
	{
		if(expr->type!=_VARVAL)
		{
			checkExpr(expr->left,assign);
			checkExpr(expr->right,assign);
		}
	}
}

void TreeTraversal::checkIfStmt(struct IfStmtInfo * ifstmt) throw(char*)
{
	/*�������� ��_���� �� ������������*/
	if(exprContainsAssign(ifstmt->expr))
		throw makeStringForException("Assignment operation must not be in a conditional expression.",ifstmt->expr->loc);
	checkExpr(ifstmt->expr,false);//�������� ��������� ���������
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
			checkExpr(begin->expr,false);
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
	if(exprContainsAssign(whilestmt->expr))
		throw makeStringForException("Assignment operation must not be in a conditional expression.",whilestmt->expr->loc);
	checkExpr(whilestmt->expr,false);//�������� ��������� ��������� ����������� �����
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
	// 1. ���� ��� ����������..
	if(this->gl_state==_MAIN_STATE)
	{
		// 1.1. ���������, ��������� �� ��� ����� ����������
		// 1.2. ���� ���������� �� ���������, �� ��������� ���������� � �� ����������
		if(!containsString(this->varDecls.at("global"),opName))
			this->varDecls.at("global").push_back(opName);
		// ������� �������
		if(containsString(this->funcNames,opName))
		{
			deleteString(this->funcNames,opName);
			deleteFuncHeader(this->funcHeaders,opName);
		}
	}
	// 2. ���� ��� ���������..
	else
	{
		// 2.1. ���������, ���� �� ��������� ���������� � ����� ������
		// 2.2. ���� ���������� � ����� ������ ���
		if(!containsString(this->varDecls.at(currentFuncName),opName))
		{
			// 2.2.1. ���������, ���� �� ���������� ���������� � ����� ������
			// 2.2.2. ���� ����� ���������� ���� � ���������� ����, �� ���������, �������������� �� ��� �� ����� � ���� �������
			if(containsString(this->varDecls.at("global"),opName))
			{
				// 2.2.2.1. ���� ���������� ��������������, ������ ������
				if(containsString(this->varUsage.at(currentFuncName),opName))
				{
					// ���� �� ��������, ������ ������ � ������ ��������
					char* errstr=new char[35+strlen(opName.c_str())];
					errstr[0]='\0';
					strcpy(errstr,"Variable referenced before assignment - ");
					strcat(errstr,opName.c_str());
					throw makeStringForException(errstr,forstmt->expr->loc);
				}
				else
					this->varDecls.at(currentFuncName).push_back(opName);
				// 2.2.2.2. ����� ��������� ���������� � ������ ��������� ����������
			}
			else
				this->varDecls.at(currentFuncName).push_back(opName);
			// ������� �������
			if(containsString(this->funcNames,opName))
			{
				deleteString(this->funcNames,opName);
				deleteFuncHeader(this->funcHeaders,opName);
			}
		}
	}
	if(exprContainsAssign(forstmt->expr))
		throw makeStringForException("Assignment operation must not be in a conditional expression.",forstmt->expr->loc);
	checkExpr(forstmt->expr,false);//�������� ���������, �� ��� �������� ����
	checkStatementList(forstmt->stmtlist);//�������� ���� �����
	if(forstmt->elsestmt!=NULL)
		checkStatementList(forstmt->elsestmt);//�������� ����� else �����

	// ��������� ��������� ��� ������ �� �����
	if(cycle==true)
	{
		lc_state = _REGULAR_STATE;
	}
}

void TreeTraversal::checkFuncDefStmt(struct FuncDefInfo * funcdefstmt) throw(char*)
{
	// ��������� ������� ��������� �����������
	enum GlobalState lastState = gl_state;
	// ���� ���������� ������� �� ����������� ����, ������ ���������
	if(lastState == _MAIN_STATE)
		gl_state = _FUNC_STATE;
	// ����� ������ ������
	else if(lastState == _FUNC_STATE)
		throw makeStringForException("Can't define function inside function",funcdefstmt->nameLoc);
	// ���������, ��� ��� �������� �� ��������� � ������ ������������ �������
	// ��� �������
	currentFuncName=std::string(funcdefstmt->functionName);
	if(containsString(this->libFuncs,currentFuncName))
	{
		// ���� �� ��������, ������ ������ � ������ ��������
		char* errstr=new char[50+strlen(currentFuncName.c_str())];
		errstr[0]='\0';
		strcpy(errstr,"Can't define function with lib function name - ");
		strcat(errstr,currentFuncName.c_str());
		throw makeStringForException(errstr,funcdefstmt->nameLoc);
	}
	// ��������� ��������� �������
	checkFuncParams(funcdefstmt->params);
	// ������� ��������� �� ��������� � ���������� ������� �������
	struct FunctionHeader* curHeader = new struct FunctionHeader;
	// �������� � ���� ��� �������
	curHeader->functionName = new char [strlen(funcdefstmt->functionName)+1];
	strcpy(curHeader->functionName,funcdefstmt->functionName);
	// � �������� ��������� �� ������ ����������
	curHeader->params=funcdefstmt->params;
	// ���� ����� ������� ��� �� ���� ���������
	if(!containsFuncHeader(this->funcHeaders,curHeader)&&!containsString(this->funcNames,std::string(curHeader->functionName)))
	{
		// ��������� �� ��������� � ������ �������
		this->funcHeaders.push_back(curHeader);
		// ������� ���������� � ����� ������
		deleteString(this->varDecls.at("global"),curHeader->functionName);
		this->funcNames.push_back(curHeader->functionName);
		// ��������� ��������� ������� � ������ ��������
		std::vector<std::string> onlyInHeader;
		this->varDecls[currentFuncName].push_back("");
		this->varDecls[currentFuncName].clear();
		this->varUsage[currentFuncName].push_back("");
		this->varUsage[currentFuncName].clear();
		struct DefFuncParamInfo* el = curHeader->params->first;
		while(el!=NULL)
		{
			this->varDecls.at(currentFuncName).push_back(el->paramName);
			/*if(!containsString(this->varNames,std::string(el->paramName)))
			{
				this->varNames.push_back(std::string(el->paramName));
				onlyInHeader.push_back(std::string(el->paramName));
			}*/
			el=el->next;
		}
		hasReturn=false;
		// ��������� �� ����
		checkStatementList(funcdefstmt->body);
		if(!hasReturn)
		{
			// ������� ���� return � ������� �� ������� None � ��������� ��� � ����� ����������� ���� �������
			struct ValInfo* ret = createValNode(_NONE,NULL,NULL,NULL,funcdefstmt->body->last->loc);
			struct ExprInfo* retExpr = createSimpleExpr(_VARVAL, NULL, ret, ret->loc);
			struct StmtInfo* retStmtm = createFromReturnStatement(_RETURN,retExpr,retExpr->loc);
			struct StmtInfo* lastInFunc = funcdefstmt->body->last;
			funcdefstmt->body->last=retStmtm;
			lastInFunc->next=funcdefstmt->body->last;
		}
	}
	// ����� ����������� ����������
	else
	{
		// ���� �� ��������, ������ ������ � ������ ��������
		char* errStr = new char[30+strlen(curHeader->functionName)];
		strcpy(errStr,"Can't define same function: ");
		strcat(errStr,curHeader->functionName);
		throw makeStringForException(errStr,funcdefstmt->nameLoc);
	}
	// ���� ��� ����� ������� �� ����������� ����, ������ ���������
	if(lastState == _MAIN_STATE)
		gl_state = _MAIN_STATE;
}

void TreeTraversal::checkContinueBreakStmt(struct StmtInfo* contBreakStmt) throw(char*)
{
	// ���� �� ������ ��������� �� � �����, ������� ����������
	if(this->lc_state == _REGULAR_STATE)
	{
		// ��������� ��� � ������ ���������
		char* errStr = new char[31];
		if(contBreakStmt->type==_BREAK)
			strcpy(errStr,"Found break in global code.");
		else if(contBreakStmt->type==_CONTINUE)
			strcpy(errStr,"Found continue in global code.");
		// ������� ������ �����������
		throw makeStringForException(errStr,contBreakStmt->loc);
	}
}

void TreeTraversal::checkReturnStmt(struct StmtInfo* retStmt, struct ExprInfo * expr) throw(char*)
{
	// ���� �� ������ ��������� � ������� ���� ���������, ������� ����������
	if(this->gl_state == _MAIN_STATE)
		throw makeStringForException("Found return in global code.",retStmt->loc);
	// ����� ��������� ������������ ���������
	checkExpr(expr,false);
	hasReturn=true;
}

void TreeTraversal::checkDelStmt(struct ExprInfo * expr) throw(char*)
{
	// �� ������ ������ ��������, ��� ���������� ����� ���� ������ �������
	// ������� ���������, �� ������ �� ��� �������
	checkExpr(expr,false);
	if(this->gl_state==_FUNC_STATE)
		if(!containsString(varUsage[currentFuncName],std::string(expr->idName)))
			varUsage[currentFuncName].push_back(std::string(expr->idName));
	if(containsString(this->varDecls[currentFuncName],std::string(expr->idName)))
	{
		deleteString(this->varDecls[currentFuncName],std::string(expr->idName));
		
	}
	if(containsString(this->funcNames,std::string(expr->idName)))
	{
		deleteString(this->funcNames,std::string(expr->idName));
		std::vector<struct FunctionHeader*>::iterator iter;  // ��������� �������� ��� ������ �������
		// ��� ������� �������� ������ � ���� �� ������� ��������..
		for(iter=this->funcHeaders.begin(); iter<this->funcHeaders.end(); iter++) 
		{
			// ���������, ����� �� ������� ������� ������
			if(strcmp((*iter)->functionName,expr->idName)==0)
			{
				// ������� ������ ��������
				this->funcHeaders.erase(iter);
				// ������� �� �����
				break;
			}
		}
	}
}

void TreeTraversal::checkFuncParams(struct DefFuncParamListInfo* params)
{
	// �������� ���������� ������� ��� ���������� ����������� ���� � ���,
	// ��� ��������� �� ��������� ������ ���� ������� ����� ���� ���������� ��� ��������
	// ���� � ������� ��� ����������, �� �������� �� �����
	// �����
	if(params!=NULL)
	{
		// ��������� ������� ��� ������ �������
		struct DefFuncParamInfo* element = params->first;
		// ��� ������� ��������� �������
		while(element!=NULL)
		{
			// ���� ���� ��������� �������..
			if(element->next!=NULL)
			{
				// ���� ������� ������� - �������� �� ���������, � ��������� - ������� �������
				// �� ����������� ����������
				if(element->paramVal!=NULL&&element->next->paramVal==NULL)
					throw makeStringForException("Wrong order of function params",element->loc);
			}
			// ��������� � ���������� ��������
			element=element->next;
		}
	}
}