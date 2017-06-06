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
		throw makeStringForException("Root of tree is a null pointer\n",NULL);
	// ������ ��������� ���������
	this->gl_state=_MAIN_STATE;
	this->lc_state=_REGULAR_STATE;
	// ��������� �������� ������
	checkStatementList(root);
}

struct ConstTable_Elem* TreeTraversal::makeTableEl(enum ConstType type, int* numberInTable, char * utf8, int val_int, int arg1, int arg2)
{
	struct ConstTable_Elem* te = new struct ConstTable_Elem();
	te->type=type;
	te->numberInTable=(*numberInTable)++;
	te->next=NULL;
	if(type==CONST_UTF8)
	{
		te->value.utf8=new char[strlen(utf8)+1];
		strcpy(te->value.utf8,utf8);
	}
	else if(type==CONST_STRING || type==CONST_CLASS)
	{
		te->value.args.arg1=arg1;
	}
	else if(type==CONST_INT)
	{
		te->value.val_int=val_int;
	}
	else if(type==CONST_NAMETYPE || type==CONST_METHODREF || type==CONST_FIELDREF)
	{
		te->value.args.arg1=arg1;
		te->value.args.arg2=arg2;
	}

	return te;
}

// ����������� ������ (������ �����)
void TreeTraversal::makeTables(const struct StmtListInfo* treeRoot)
{
	
	if(treeRoot == NULL)
		throw makeStringForException("Root of tree is a null pointer\n",NULL);
	currentFuncName=std::string("");
	// ������ ��������� ���������
	this->gl_state=_MAIN_STATE;
	this->lc_state=_REGULAR_STATE;
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
	this->varNames.clear();
	this->funcHeaders.clear();
	valClassDesc=0;
	typeDesc=0;
	// TODO ������� ������� ��������� ������� ��� �������
	int constantNumber=1;
	parseStmtListForTable(treeRoot,constNumber,NULL);
	std::vector<struct TableElement*>::iterator iter;  // ��������� �������� ��� ������ �����
	// TODO ������� ������� ������ ������ ��� ������
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
			parseExprForTable(begining->expr,constNum, local, begining->expr->type);
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

void TreeTraversal::parseExprForTable(const struct ExprInfo * expr, int* constNum, int local, enum ExprType typeAboveExpression)
{
	// ���� ���� �� �������� ��� ���� ������������� ��� ������ �������� �� �������
	if(expr->type==_ASSIGN || expr->type==_ARRID || expr->type==_ARRID_AND_ASSIGN)
	{
		parseExprForTable(expr->left, constNum, local, expr->type);
		if(expr->type==_ARRID_AND_ASSIGN)
		{
			parseExprForTable(expr->middle, constNum, local, expr->type);
		}
		parseExprForTable(expr->right, constNum, local, expr->type);
	}

	// ���� �������
	if(expr->type==_OPERAND)
	{
		// ���������, ���� �� �� ��� � ������ ����������
		// � ���� ���, �� ���������
		std::string opName = std::string(expr->idName);
		if(!containsString(this->varNames,opName))
		{
			this->varNames.push_back(opName);
			// ������ �������� � ����
			// ��������� � ������� ������ � ����������
			// ������ NameAndType
			// ������ fieldRef				
		}
	}

	// ���� ���������
	if(expr->type==_VARVAL)
		parseValTypeForTable(expr->exprVal,constNum,local);
}

void TreeTraversal::parseValTypeForTable(const struct ValInfo * val, int* constNum, int local)
{
	if(val->type==_TRUE)
	{
		;
	}
	else if(val->type==_FALSE)
	{
		;
	}
	else if(val->type==_NUMBER)
	{
		;
	}
	else
	{
		;
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
	if(!containsFuncHeader(this->funcHeaders,curHeader))
	{
		// ��������� �� ��������� � ������ �������
		this->funcHeaders.push_back(curHeader);
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
		// ��������� � ������� ������ � ����������
		// ������ NameAndType
		// ������ methodRef
		//currentFuncName=std::string(curHeader->functionName);

		std::vector<struct TableElement*> funcTable;
		int* funcConsts = new int;
		*funcConsts=1;
		// ��������� �� ����
		parseStmtListForTable(funcdefstmt->body,constNum,local);
	}
	// ����� ����������� ����������

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

void TreeTraversal::checkExpr(struct ExprInfo * expr, bool assign) throw(char*)
{
	// ���� �������
	if(expr->type==_OPERAND)
	{
		// ���������, ��� �� ��� ��������
		std::string opName = std::string(expr->idName);
		if(!containsString(this->varNames,opName)&&!containsString(this->funcNames,opName))
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
			throw makeStringForException("Assignment operation can not be more than 2 times in the expression.",expr->loc);
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
				if(containsString(this->funcNames,opName))
				{
					this->deleteString(this->funcNames,opName);
					std::vector<struct FunctionHeader*>::iterator iter;  // ��������� �������� ��� ������ �������
					// ��� ������� �������� ������ � ���� �� ������� ��������..
					for(iter=this->funcHeaders.begin(); iter<this->funcHeaders.end(); iter++) 
					{
						// ���������, ����� �� ������� ������� ������
						if(strcmp((*iter)->functionName,opName.c_str())==0)
						{
							// ������� ������ ��������
							this->funcHeaders.erase(iter);
							// ������� �� �����
							break;
						}
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
	else if(expr->type==_UMINUS)
	{
		// ������� ����� ����� ���� ������ ����� ����� ������ ��� ����������, ������������ ����� �����
		// �������� ������
		if(expr->left->type==_VARVAL)
		{
			struct ValInfo* val = expr->left->exprVal;
			if(val->type!=_NUMBER)
				throw makeStringForException("Can't make negative value of anything except integer.",val->loc);
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
	if(!containsString(this->varNames,opName))
	{
		this->varNames.push_back(opName);
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
	// ���� ����� ������� �� ����������� ����, ������ ���������
	if(lastState == _MAIN_STATE)
		gl_state = _FUNC_STATE;
	// ��� �������
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
	if(!containsFuncHeader(this->funcHeaders,curHeader))
	{
		// ��������� �� ��������� � ������ �������
		this->funcHeaders.push_back(curHeader);
		if(containsString(this->varNames,curHeader->functionName))
			deleteString(this->varNames,curHeader->functionName);
		this->funcNames.push_back(curHeader->functionName);
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
}

void TreeTraversal::checkDelStmt(struct ExprInfo * expr) throw(char*)
{
	// �� ������ ������ ��������, ��� ���������� ����� ���� ������ �������
	// ������� ���������, �� ������ �� ��� �������
	checkExpr(expr,false);
	if(containsString(this->varNames,std::string(expr->idName)))
		deleteString(this->varNames,std::string(expr->idName));
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


/*!
*	---------------- ���� ��������������� ������� ��� ������ �� �������� � ������������ ----------------
*/

/*std::string TreeTraversal::convertTypeToString(enum TableElemType type)
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
}*/

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

char* TreeTraversal::makeStringForException(char* message, struct CodeLocation* location)
{
	char* bufstr;
	if(location!=NULL)
	{
		bufstr = new char [62];
		sprintf(bufstr,"Location:(%d.%d-%d.%d)",location->firstLine,location->firstColumn,location->lastLine,location->lastColumn);
	}
	else
	{
		bufstr = new char [2];
		bufstr[0] = '\0';
	}
	char* finalString = new char [strlen(bufstr)+strlen(message)+2];
	strcpy(finalString,message);
	strcat(finalString,"\n");
	strcat(finalString,bufstr);
	return finalString;
}

void TreeTraversal::initializeConstTable(struct ConstTable_List* ct)
{
	// ����� ���������
	*(ct_consts->constnumber)=1;
	// �������� ������
	ct=new struct ConstTable_List;
	ct->first=new struct ConstTable_Elem;
	// �������������� ������ �������
	ct->first->type=CONST_UTF8;
	ct->first->value.utf8="Code";
	ct->first->next=NULL;
	// ��������� ������ � ��������� ��������
	ct->last=ct->first;
	// CT_AddConst(CONST_UTF8, NULL); - ��������� ������� � ����������� �� ����
	// ������ �������� �������� � ��������� ��� �� ����� ���������� � ������
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"java/lang/Object",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "java/lang/Object";

	//CT_AddConst(CONST_CLASS, (void*)&constnumber);
	//objectClass = constnumber;

	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));
	ct_consts->objectClass=*(ct_consts->constnumber);

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "<init>";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"<init>",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "()V";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()V",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));

	//CONSTANT_INIT = constnumber;
	ct_consts->init=*(ct_consts->constnumber);

	//CT_AddConst2(CONST_METHODREF, objectClass, constnumber);
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));


	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "Program";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"Program",NULL,NULL,NULL));
	//CT_AddConst(CONST_CLASS, (void*)&constnumber);
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));

	//programClass.classConst = constnumber;
	prog->classConst=*(ct_consts->constnumber);

	//programClass.firstField = NULL;
	prog->firstField=NULL;

	// ������ ��� �������
	// ������� �������
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "main";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"main",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "([Ljava/lang/String;)V";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"([Ljava/lang/String;)V",NULL,NULL,NULL));
	ct_consts->nameconstid = *(ct_consts->constnumber) - 1;
	// ����� �� ������ �������� ��� �����?

	
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "rtl/Value";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"rtl/Value",NULL,NULL,NULL));
	//CT_AddConst(CONST_CLASS, (void*)&constnumber);
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));
	ct_consts->valueType = *(ct_consts->constnumber);
	//CT_AddConst2(CONST_METHODREF, mixedtype, CONSTANT_INIT);
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->valueType,ct_consts->init));

	/*CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "rtl/Undefined";
	CT_AddConst(CONST_CLASS, (void*)&constnumber);
	mixedtype = constnumber;
	CT_AddConst2(CONST_METHODREF, mixedtype, CONSTANT_INIT);*/


	// rtl
	//----------------------------------
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "rtl/Lib";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"rtl/Lib",NULL,NULL,NULL));
	//CT_AddConst(CONST_CLASS, (void*)&constnumber);
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));
	ct_consts->rtlClass = *(ct_consts->constnumber);
	
	// print
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "print";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"print",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;)V";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)V",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, ct_consts->rtlClass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//----------------------------------


	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "sub";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"sub",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//arifmentic = constnumber;
	ct_consts->arifmetic=*(ct_consts->constnumber);

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "add";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"add",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "mul";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mul",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "div";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"div",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "mod";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mod",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "toIntBool";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"toIntBool",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;)I";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)I",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	/*CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "ListAdd";
	CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	list = constnumber;

	CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "ListSet";
	CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "(Lrtl/Mixed;Lrtl/Mixed;Lrtl/Mixed;)V";
	CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);

	CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "ListGet";
	CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "(Lrtl/Mixed;Lrtl/Mixed;)Lrtl/Mixed;";
	CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);*/

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "eq";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"eq",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//logic = constnumber;
	ct_consts->logic=*(ct_consts->constnumber);

	//CT_AddConst(CONST_UTF8, NULL); // + 4
	//lastConst->value.utf8 = "less";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"less",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL); // + 8
	//lastConst->value.utf8 = "more";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"more",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);// + 12
	//lastConst->value.utf8 = "lessOrEq";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"lessOrEq",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);// + 16
	//lastConst->value.utf8 = "moreOrEq";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"moreOrEq",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);// + 20
	//lastConst->value.utf8 = "not";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"not",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);// + 24
	//lastConst->value.utf8 = "or";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"or",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);// + 28
	//lastConst->value.utf8 = "and";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"and",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);// + 32
	//lastConst->value.utf8 = "notEq";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"notEq",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//mixedclass = constnumber;//??
	ct_consts->valueClass=*(ct_consts->constnumber);

	// mixedFromInt 4
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "mixedFromInt";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mixedFromInt",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(I)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(I)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// mixedFromUndefined 8
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "mixedFromUndefined";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mixedFromUndefined",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "()Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// mixedFromFloat 12
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "mixedFromFloat";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mixedFromFloat",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(F)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(F)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// mixedFromList 16
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "mixedFromList";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mixedFromList",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "()Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
}

void TreeTraversal::appendToConstTable(struct ConstTable_Elem* ce)
{
	struct ConstTable_Elem* last_el=this->ct->last;
	this->ct->last=ce;
	this->ct->last->next=NULL;
	last_el->next=this->ct->last;
}