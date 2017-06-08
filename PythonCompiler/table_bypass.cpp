#include "bypass_semant_tree.h"

#pragma warning(disable : 4996)

struct ConstTable_Elem* TreeTraversal::makeTableEl(enum ConstType type, int* numberInTable, char * utf8, int val_int, int arg1, int arg2)
{
	struct ConstTable_Elem* te = new struct ConstTable_Elem();
	te->type=type;
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
	te->numberInTable=++(*numberInTable);
	return te;
}

char* convertConstTypeToString(enum ConstType type)
{
	switch (type)
	{
	case CONST_UTF8:
		return "UTF-8";
	case CONST_INT:
		return "INT";
	case CONST_FLOAT:
		return "FLOAT";
	case CONST_CLASS:
		return "Class";
	case CONST_STRING:
		return "STRING";
	case CONST_FIELDREF:
		return "FieldRef";
	case CONST_METHODREF:
		return "MethodRef";
	case CONST_NAMETYPE:
		return "Name&Type";
	}
}

char* convertValToString(struct ConstTable_Elem* el)
{
	char* buf=new char;
	char* final=new char;
	if(el->type==CONST_UTF8)
		return el->value.utf8;
	else if(el->type==CONST_STRING || el->type==CONST_CLASS)
	{
		itoa(el->value.args.arg1,buf,10);
		return buf;
	}
	else if(el->type==CONST_INT)
	{
		itoa(el->value.val_int,buf,10);
		return buf;
	}
	else if(el->type==CONST_NAMETYPE || el->type==CONST_METHODREF || el->type==CONST_FIELDREF)
	{
		final=new char [35];
		itoa(el->value.args.arg1,buf,10);
		strcpy(final,buf);
		strcat(final,", ");
		itoa(el->value.args.arg2,buf,10);
		strcat(final,buf);
		return final;
	}
}

char* convertTableElementToString(struct ConstTable_Elem* el)
{
	std::string final="\"";
	final.append(std::to_string((long double)el->numberInTable));
	final.append("\";\"");
	final.append(convertConstTypeToString(el->type));
	final.append("\";\"");
	final.append(convertValToString(el));
	final.append("\";");
	char* strToRet = new char[final.length()+1];
	strcpy(strToRet,final.c_str());
	return strToRet;
}

char* convertFieldElementToString(struct FieldTable_Elem* el)
{
	std::string final="\"";
	final.append(std::to_string((long double)el->access));
	final.append("\";\"");
	final.append(std::to_string((long double)el->fieldName));
	final.append("\";\"");
	final.append(std::to_string((long double)el->fieldDesc));
	final.append("\";\"");
	final.append(std::to_string((long double)el->attrs));
	final.append("\";");
	char* strToRet = new char[final.length()+1];
	strcpy(strToRet,final.c_str());
	return strToRet;
}

char* convertMethodElementToString(struct MethodTable_Elem* el)
{
	std::string final="\"";
	final.append(std::to_string((long double)el->access));
	final.append("\";\"");
	final.append(std::to_string((long double)el->methodName));
	final.append("\";\"");
	final.append(std::to_string((long double)el->methodDesc));
	final.append("\";\"");
	final.append(std::to_string((long double)el->attrs));
	final.append("\";");
	char* strToRet = new char[final.length()+1];
	strcpy(strToRet,final.c_str());
	return strToRet;
}

char* convertVarElementToString(struct Variable* el)
{
	std::string final="\"";
	final.append(std::to_string((long double)el->num));
	final.append("\";\"");
	final.append(std::string(el->name));
	final.append("\";\"");
	final.append(std::string("LValue;"));
	final.append("\";\"");
	final.append(std::to_string((long double)el->localFor));
	final.append("\";");
	char* strToRet = new char[final.length()+1];
	strcpy(strToRet,final.c_str());
	return strToRet;
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
	this->varDecls.clear();
	this->varDecls["global"].clear();
	this->funcHeaders.clear();
	valClassDesc=0;
	typeDesc=0;
	this->ct_consts=new ConstTable_Consts;
	this->ct_consts->constnumber=new int(0);
	this->prog=new ClassTable_Elem;
	// TODO ������� ������� ��������� ������� ��� �������
	initializeConstTable();

	fields=new FieldTable_List;
	fields->first=NULL;
	fields->last=NULL;

	methods=new MethodTable_List;
	methods->first=NULL;
	methods->last=NULL;

	vars=new VariableTable_List;
	vars->first=NULL;
	vars->last=NULL;

	parseStmtListForTable(treeRoot,NULL);
	//std::vector<struct TableElement*>::iterator iter;  // ��������� �������� ��� ������ �����
	// TODO ������� ������� ������ ������ ��� ������
	FILE* table = fopen("const_table.csv","wt");
	fprintf(table,"\"Constant number\";\"Type\";\"Constant value\";\n");
	struct ConstTable_Elem* el = this->ct->first;
	while(el!=NULL)
	{
		fprintf(table,"%s\n",convertTableElementToString(el));
		el=el->next;
	}
	fclose(table);
	FILE* fieldT = fopen("field_table.csv","wt");
	fprintf(fieldT,"\"Access\";\"Name Ref\";\"Desc\";\"Attr\";\n");
	struct FieldTable_Elem* fe = this->fields->first;
	while(fe!=NULL)
	{
		fprintf(fieldT,"%s\n",convertFieldElementToString(fe));
		fe=fe->next;
	}
	fclose(fieldT);
	FILE* methodT = fopen("method_table.csv","wt");
	fprintf(methodT,"\"Access\";\"Name Ref\";\"Desc\";\"Attr\";\n");
	struct MethodTable_Elem* me = this->methods->first;
	while(me!=NULL)
	{
		fprintf(methodT,"%s\n",convertMethodElementToString(me));
		me=me->next;
	}
	fclose(methodT);
	FILE* localsT = fopen("locals_table.csv","wt");
	fprintf(localsT,"\"Number\";\"Name\";\"Type\";\"Local for\";\n");
	struct Variable* v = this->vars->first;
	while(v!=NULL)
	{
		fprintf(localsT,"%s\n",convertVarElementToString(v));
		v=v->next;
	}
	fclose(localsT);
}

void TreeTraversal::parseStmtListForTable(const struct StmtListInfo* root, int local)
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
		// - ������� �������� ����������� � ��� ������� � �������
		if(begining->type==_EXPR)
		{
			//checkExpr(begining->expr);
			parseExprForTable(begining->expr, local, begining->expr->type);
		}
		else if(begining->type==_IF)
		{
			//checkIfStmt(begining->ifstmt);
			parseIfForTable(begining->ifstmt,local);
		}
		else if(begining->type==_FOR)
		{
			//checkForStmt(begining->forstmt);
			parseForForTable(begining->forstmt,local);
		}
		else if(begining->type==_WHILE)
		{
			//checkWhileStmt(begining->whilestmt);
			parseWhileForTable(begining->whilestmt,local);
		}
		else if(begining->type==_FUNC_DEF)
		{
			//checkFuncDefStmt(begining->funcdefstmt);
			parseFuncDefForTable(begining->funcdefstmt,local);
		}
		else if(begining->type==_RETURN)
		{
			//checkReturnStmt(begining,begining->expr);
			parseExprForTable(begining->expr, local, begining->expr->type);
		}
		// ������� ��������� ������� ������ ����� �������
		begining = begining->next;
	}
}

void TreeTraversal::parseExprForTable(const struct ExprInfo * expr, int local, enum ExprType typeAboveExpression)
{
	// ���� �������
	if(expr->type==_OPERAND)
	{
		if(local!=NULL)
		{
			if(!containsString(this->varDecls[currentFuncName],std::string(expr->idName))&&!containsString(this->varDecls["global"],std::string(expr->idName))&&typeAboveExpression==_ASSIGN)
			{
				struct Variable* var = new struct Variable;
				var->name=new char[strlen(expr->idName)+1];
				strcpy(var->name,expr->idName);
				var->localFor=local;
				appendToVarsTable(var);
				this->varDecls[currentFuncName].push_back(std::string(expr->idName));
			}
		}
		else
		{
			// ���������, ���� �� �� ��� � ������ ����������
			// � ���� ���, �� ���������
			std::string opName = std::string(expr->idName);
			if(!containsString(this->varDecls["global"],opName))
			{
				struct FieldTable_Elem* curElem=new FieldTable_Elem;
				appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,expr->idName,NULL,NULL,NULL));
				curElem->fieldName=*(ct_consts->constnumber);
				// ������ �������� � ����
				if(this->varDecls["global"].empty())
				{
					appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"Lrtl/Value;",NULL,NULL,NULL));
					ct_consts->descId=*(ct_consts->constnumber);
					// ������ NameAndType
					appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,ct_consts->descId));
				}
				else
					// ������ NameAndType
					appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),ct_consts->descId));
				curElem->fieldDesc=ct_consts->descId;
				// ������ fieldRef
				appendToConstTable(makeTableEl(CONST_FIELDREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));	
				this->varDecls["global"].push_back(opName);
				appendToFieldTable(curElem);
			}
		}		
	}
	// ���������, ��������� �� ������ ���������
	else if(expr->type==_ARRINIT||expr->type==_FUNCCALL)
	{
		// ������� ��������� ��������� �� ������� ������
		struct ExprInfo* curExpr;
		// ������� ������ ������� ������ ���������
		curExpr = expr->arglist->first;
		// ���� ������� ������� ������ �� ���������..
		while(curExpr!=NULL)
		{
			parseExprForTable(curExpr, local, expr->type);
			curExpr = curExpr->next;
		}
	}	
	// ������� ���������
	else if(expr->type==_NOT || expr->type==_UMINUS)
	{
		parseExprForTable(expr->left, local, expr->type);
	}
	// ���� ���������
	else if(expr->type==_VARVAL)
		parseValTypeForTable(expr->exprVal,local);
	// ���� � ����������� ���������
	else
	{
		parseExprForTable(expr->left, local, expr->type);
		if(expr->type==_ARRID_AND_ASSIGN)
		{
			parseExprForTable(expr->middle, local, expr->type);
		}
		parseExprForTable(expr->right, local, expr->type);
	}
}

void TreeTraversal::parseValTypeForTable(const struct ValInfo * val, int local)
{
	if(val->type==_TRUE)
	{
		appendToConstTable(makeTableEl(CONST_INT,ct_consts->constnumber,NULL,1,NULL,NULL));
	}
	else if(val->type==_FALSE)
	{
		appendToConstTable(makeTableEl(CONST_INT,ct_consts->constnumber,NULL,0,NULL,NULL));
	}
	else if(val->type==_NUMBER)
	{
		appendToConstTable(makeTableEl(CONST_INT,ct_consts->constnumber,NULL,val->intVal,NULL,NULL));
	}
	else if(val->type==_STRING)
	{
		appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,val->stringVal,NULL,NULL,NULL));
		appendToConstTable(makeTableEl(CONST_STRING,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));
	}
		
}

void TreeTraversal::parseFuncDefForTable(const struct FuncDefInfo * funcdefstmt, int local)
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
		char* Ctype=new char [strlen(type.c_str())+1];
		strcpy(Ctype,type.c_str());
		struct MethodTable_Elem* curElem=new MethodTable_Elem;
		int mRef=0;
		appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,funcdefstmt->functionName,NULL,NULL,NULL));
		curElem->methodAttr=NULL;
		curElem->methodName=*(ct_consts->constnumber);
		// ������ �������� � ����
		appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,Ctype,NULL,NULL,NULL));
		curElem->methodDesc=*(ct_consts->constnumber);
		// ������ NameAndType
		appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
		// ������ methodRef
		appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));	
		mRef=*(ct_consts->constnumber);
		appendToMethodTable(curElem);
		currentFuncName=std::string(funcdefstmt->functionName);
		struct DefFuncParamInfo* el = curHeader->params->first;
		while(el!=NULL)
		{
			struct Variable* var = new struct Variable;
			var->name=new char[strlen(el->paramName)+1];
			strcpy(var->name,el->paramName);
			var->localFor=mRef;
			appendToVarsTable(var);
			this->varDecls[currentFuncName].push_back(std::string(var->name));
			el=el->next;
		}

		// ��������� �� ����
		parseStmtListForTable(funcdefstmt->body,mRef);
	}
	// ���� ��� ����� ������� �� ����������� ����, ������ ���������
	if(lastState == _MAIN_STATE)
	{
		gl_state = _MAIN_STATE;
		currentFuncName="global";
		local=NULL;
	}
}

void TreeTraversal::parseIfForTable(const struct IfStmtInfo * ifstmt, int local)
{
	// ��������� �������� ���������
	parseExprForTable(ifstmt->expr,local,ifstmt->expr->type);
	// ��������� ���� �������
	parseStmtListForTable(ifstmt->stmtlist,local);
	// ��������� else ����, ���� �� ����
	if(ifstmt->elsestmtlist!=NULL)
		parseStmtListForTable(ifstmt->elsestmtlist,local);
}

void TreeTraversal::parseForForTable(const struct ForStmtInfo * forstmt, int local)
{
	// ��������� ��������� �������� �����
	parseExprForTable(forstmt->expr,local,forstmt->expr->type);
	// ��������� ���� �����
	parseStmtListForTable(forstmt->stmtlist,local);
	// ��������� else ����, ���� �� ����
	if(forstmt->elsestmt!=NULL)
		parseStmtListForTable(forstmt->elsestmt,local);
}

void TreeTraversal::parseWhileForTable(const struct WhileStmtInfo * whilestmt, int local)
{
	// ��������� ��������� �������� �����
	parseExprForTable(whilestmt->expr,local,whilestmt->expr->type);
	// ��������� ���� �����
	parseStmtListForTable(whilestmt->stmtlist,local);
	// ��������� else ����, ���� �� ����
	if(whilestmt->elsestmt!=NULL)
		parseStmtListForTable(whilestmt->elsestmt,local);
}