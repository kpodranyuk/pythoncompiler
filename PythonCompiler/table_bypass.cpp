#include "bypass_semant_tree.h"

#pragma warning(disable : 4996)

struct ConstTable_Elem* TreeTraversal::makeTableEl(enum ConstType type, int* numberInTable, char * utf8, int val_int, float val_float, int arg1, int arg2)
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
	else if(type==CONST_FLOAT)
	{
		te->value.val_float=val_float;
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
	else if(el->type==CONST_FLOAT)
	{
		//ftoa(el->value.val_float,buf,10);
		sprintf(buf,"%f",el->value.val_float);
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

void TreeTraversal::findOpInCT(struct ExprInfo* expr,int local)
{
	std::vector<struct opInfo*>::iterator it;
	for(it=ops.begin();it<ops.end();it++)
	{
		if(strcmp((*it)->opName,expr->idName)==0&&(*it)->local==local)
		{
			expr->locFor=(*it)->local;
			expr->numberInTable=(*it)->FR;
			break;
		}
	}
}

bool TreeTraversal::findCInCT(struct ValInfo* val)
{
	std::vector<struct constInfo*>::iterator it;
	for(it=consts.begin();it<consts.end();it++)
	{
		if((*it)->t==val->type&&val->type==_NONE||(*it)->t==val->type&&val->type==_TRUE||(*it)->t==val->type&&val->type==_FALSE||
			(*it)->t==val->type&&val->type==_NUMBER&&(*it)->val==val->intVal||
			(*it)->t==val->type&&val->type==_STRING&&strcmp((*it)->strVal,val->stringVal)==0)
		{
			val->numberInTable=(*it)->num;
			return true;
		}
	}
	return false;
}

void TreeTraversal::findMInMT(struct ExprInfo* expr)
{
	std::vector<struct methodInfo*>::iterator it;
	for(it=MRs.begin();it<MRs.end();it++)
	{
		if(strcmp((*it)->name,expr->idName)==0)
		{
			expr->locFor=NULL;
			expr->numberInTable=(*it)->MR;
			break;
		}
	}
}

int TreeTraversal::findMDesc(char* desc)
{
	std::vector<struct mDescInfo*>::iterator it;
	for(it=mDescs.begin();it<mDescs.end();it++)
	{
		if(strcmp((*it)->desc,desc)==0)
		{
			return (*it)->num;
		}
	}
	return -1;
}

// Составление таблиц (второй обход)
void TreeTraversal::makeTables(const struct StmtListInfo* treeRoot)
{
	
	if(treeRoot == NULL)
		throw makeStringForException("Root of tree is a null pointer\n",NULL);
	currentFuncName=std::string("");
	// Задаем начальные состояния
	this->gl_state=_MAIN_STATE;
	this->lc_state=_REGULAR_STATE;
	currentFuncName="global";
	int* constNumber = new int;
	*constNumber=1;
	/**
	* В таблицу добавляются только классы (при создании, у нас только main и value)
	* Поля класса - по идее, надо вынести все переменные глобального кода как локальные метода main
	* Методы класса - функции
	* Первая таблица - таблица глобального кода
	* То есть таблица обновляется только при
	* - объявлении переменной
	* - объявлении массива
	* - объявлении функции
	*/
	this->varDecls.clear();
	this->varDecls["global"].clear();
	this->funcHeaders.clear();
	valClassDesc=0;
	typeDesc=0;
	this->ct_consts=new ConstTable_Consts;
	this->ct_consts->constnumber=new int(0);
	this->prog=new ClassTable_Elem;

	this->prog->firstField=NULL;
	this->prog->lastField=NULL;

	this->prog->methodsFirst=NULL;
	this->prog->methodsLast=NULL;

	// TODO СОЗДАТЬ ФУНКЦИЮ ГЕНЕРАЦИИ ВСТАВКИ РТЛ ТАБЛИЦЫ
	initializeConstTable();

	vars=new VariableTable_List;
	vars->first=NULL;
	vars->last=NULL;
	currentMR=NULL;
	parseStmtListForTable(treeRoot,NULL);
	//std::vector<struct TableElement*>::iterator iter;  // Объявляем итератор для списка строк
	// TODO СОЗДАТЬ ФУНКЦИЮ ОБХОДА ТАБЛИЦ ДЛЯ ПЕЧАТИ
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
	struct FieldTable_Elem* fe = this->prog->firstField;
	while(fe!=NULL)
	{
		fprintf(fieldT,"%s\n",convertFieldElementToString(fe));
		fe=fe->next;
	}
	fclose(fieldT);
	FILE* methodT = fopen("method_table.csv","wt");
	fprintf(methodT,"\"Access\";\"Name Ref\";\"Desc\";\"Attr\";\n");
	struct MethodTable_Elem* me = this->prog->methodsFirst;
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
	
	// Создаем локальный указатель на элемент входного дерева
	struct StmtInfo* begining;
	// Считаем первый элемент списка начальным
	begining = root->first;
	// Пока текущий элемент списка не последний..
	while(begining!=NULL)
	{
		// В зависимости от типа узла/элемента вызываем соответствующую функцию

		// Вызываем функции только для тех типов, внутри которых может
		// - быть объявление переменной - в экспре
		// - быть объявление массива - в экспре
		// - быть объявление функции - отдельным узлом
		// - таблица констант обновляется и при участии в экспрах
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
			begining->localFor=currentMR;
			currentMR=NULL;
		}
		else if(begining->type==_RETURN)
		{
			//checkReturnStmt(begining,begining->expr);
			parseExprForTable(begining->expr, local, begining->expr->type);
		}
		if(begining->type!=_FUNC_DEF)
			begining->localFor=currentMR;
		// Считаем следующий элемент списка новым текущим
		begining = begining->next;
	}
}

void TreeTraversal::parseExprForTable(struct ExprInfo * expr, int local, enum ExprType typeAboveExpression)
{
	// Если операнд
	if(expr->type==_OPERAND&&typeAboveExpression==__COUNTER	)
	{
		if(local!=NULL)
		{
			if(!containsString(this->varDecls[currentFuncName],std::string(expr->idName)))
			{
				struct Variable* var = new struct Variable;
				var->name=new char[strlen(expr->idName)+1];
				strcpy(var->name,expr->idName);
				var->localFor=local;
				appendToVarsTable(var);
				this->varDecls[currentFuncName].push_back(std::string(expr->idName));
				expr->locFor=local;
				expr->numberInTable=this->vars->count-1;
				struct opInfo* curOp = new struct opInfo;
				curOp->local=local;
				curOp->FR=expr->numberInTable;
				curOp->opName=var->name;
				this->ops.push_back(curOp);
			}
		}
		else
		{
			// Проверяем, есть ли он уже в списке переменных
			// И если нет, то добавляем
			std::string opName = std::string(expr->idName);
			if(!containsString(this->varDecls["global"],opName))
			{
				struct FieldTable_Elem* curElem=new FieldTable_Elem;
				appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,expr->idName,NULL,NULL,NULL,NULL));
				curElem->fieldName=*(ct_consts->constnumber);
				// Делаем привязку к типу
				if(this->varDecls["global"].empty())
				{
					appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"Lrtl/Value;",NULL,NULL,NULL,NULL));
					ct_consts->descId=*(ct_consts->constnumber);
					// Делаем NameAndType
					appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,ct_consts->descId));
				}
				else
					// Делаем NameAndType
					appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),ct_consts->descId));
				curElem->fieldDesc=ct_consts->descId;
				// Делаем fieldRef
				appendToConstTable(makeTableEl(CONST_FIELDREF,ct_consts->constnumber,NULL,NULL,NULL,/*ct_consts->valueType*/prog->classConst,*(ct_consts->constnumber)));	
				this->varDecls["global"].push_back(opName);
				appendToFieldTable(curElem);
				expr->locFor=NULL;
				expr->numberInTable=*(ct_consts->constnumber);
				struct opInfo* curOp = new struct opInfo;
				curOp->local=NULL;
				curOp->FR=expr->numberInTable;
				curOp->opName=expr->idName;
				this->ops.push_back(curOp);
			}
		}	
	}
	else if(expr->type==_OPERAND)
	{
		expr->locFor=NULL;
		expr->numberInTable=NULL;
		findOpInCT(expr, local);
		if(expr->locFor==NULL&&expr->numberInTable==NULL)
			findOpInCT(expr, NULL);
	}
	else if(expr->type==_ASSIGN)
	{
		struct ExprInfo* leftE=expr->left;
		if(local!=NULL)
		{
			if(!containsString(this->varDecls[currentFuncName],std::string(leftE->idName)))
			{
				struct Variable* var = new struct Variable;
				var->name=new char[strlen(leftE->idName)+1];
				strcpy(var->name,leftE->idName);
				var->localFor=local;
				appendToVarsTable(var);
				this->varDecls[currentFuncName].push_back(std::string(leftE->idName));
				leftE->locFor=local;
				leftE->numberInTable=this->vars->count-1;
				struct opInfo* curOp = new struct opInfo;
				curOp->local=local;
				curOp->FR=leftE->numberInTable;
				curOp->opName=var->name;
				this->ops.push_back(curOp);
			}
			else
				findOpInCT(leftE, local);
		}
		else
		{
			// Проверяем, есть ли он уже в списке переменных
			// И если нет, то добавляем
			std::string opName = std::string(leftE->idName);
			if(!containsString(this->varDecls["global"],opName))
			{
				struct FieldTable_Elem* curElem=new FieldTable_Elem;
				appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,leftE->idName,NULL,NULL,NULL,NULL));
				curElem->fieldName=*(ct_consts->constnumber);
				// Делаем привязку к типу
				if(this->varDecls["global"].empty())
				{
					appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"Lrtl/Value;",NULL,NULL,NULL,NULL));
					ct_consts->descId=*(ct_consts->constnumber);
					// Делаем NameAndType
					appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,ct_consts->descId));
				}
				else
					// Делаем NameAndType
					appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),ct_consts->descId));
				curElem->fieldDesc=ct_consts->descId;
				// Делаем fieldRef
				appendToConstTable(makeTableEl(CONST_FIELDREF,ct_consts->constnumber,NULL,NULL,NULL,/*ct_consts->valueType*/prog->classConst,*(ct_consts->constnumber)));	
				this->varDecls["global"].push_back(opName);
				appendToFieldTable(curElem);
				leftE->locFor=NULL;
				leftE->numberInTable=*(ct_consts->constnumber);
				struct opInfo* curOp = new struct opInfo;
				curOp->local=NULL;
				curOp->FR=leftE->numberInTable;
				curOp->opName=leftE->idName;
				this->ops.push_back(curOp);
			}
			else
				findOpInCT(leftE, local);
		}	
		parseExprForTable(expr->right, local, expr->type);
	}
	// Выражение, состоящие из списка выражений
	else if(expr->type==_ARRINIT||expr->type==_FUNCCALL)
	{
		if(expr->type==_FUNCCALL)
			findMInMT(expr);
		// Создаем локальный указатель на элемент списка
		struct ExprInfo* curExpr;
		// Считаем первый элемент списка начальным
		curExpr = expr->arglist->first;
		// Пока текущий элемент списка не последний..
		while(curExpr!=NULL)
		{
			parseExprForTable(curExpr, local, expr->type);
			curExpr = curExpr->next;
		}
	}	
	// Унарное выражение
	else if(expr->type==_NOT || expr->type==_UMINUS)
	{
		parseExprForTable(expr->left, local, expr->type);
	}
	// Если константа
	else if(expr->type==_VARVAL)
		parseValTypeForTable(expr->exprVal,local);
	// Двух и трехместные выражения
	else if(expr->type!=_OPERAND)
	{
		parseExprForTable(expr->left, local, expr->type);
		if(expr->type==_ARRID_AND_ASSIGN)
		{
			parseExprForTable(expr->middle, local, expr->type);
		}
		parseExprForTable(expr->right, local, expr->type);
	}
}

void TreeTraversal::parseValTypeForTable(struct ValInfo * val, int local)
{
	if(!findCInCT(val))
	{
		if(val->type==_TRUE)
		{
			appendToConstTable(makeTableEl(CONST_INT,ct_consts->constnumber,NULL,1,NULL,NULL,NULL));
			val->numberInTable=*(ct_consts->constnumber);
			appendCToC(val->type, 1, NULL, NULL, *(ct_consts->constnumber));
		}
		else if(val->type==_FALSE)
		{
			appendToConstTable(makeTableEl(CONST_INT,ct_consts->constnumber,NULL,0,NULL,NULL,NULL));
			val->numberInTable=*(ct_consts->constnumber);
			appendCToC(val->type, 0, NULL, NULL,*(ct_consts->constnumber));
		}
		else if(val->type==_NUMBER)
		{
			appendToConstTable(makeTableEl(CONST_INT,ct_consts->constnumber,NULL,val->intVal,NULL,NULL,NULL));
			val->numberInTable=*(ct_consts->constnumber);
			appendCToC(val->type, val->intVal,NULL, NULL, *(ct_consts->constnumber));
		}
		else if(val->type==_FLOAT)
		{
			appendToConstTable(makeTableEl(CONST_FLOAT,ct_consts->constnumber,NULL,NULL,val->fVal,NULL,NULL));
			val->numberInTable=*(ct_consts->constnumber);
			appendCToC(val->type, NULL, val->fVal, NULL, *(ct_consts->constnumber));
		}
		else if(val->type==_STRING)
		{
			appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,val->stringVal,NULL,NULL,NULL,NULL));
			appendToConstTable(makeTableEl(CONST_STRING,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),NULL));
			val->numberInTable=*(ct_consts->constnumber);
			appendCToC(val->type, NULL, NULL,val->stringVal, *(ct_consts->constnumber));
		}
		else if(val->type==_NONE)
		{
			appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"NONE",NULL,NULL,NULL,NULL));
			//appendToConstTable(makeTableEl(CONST_STRING,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));
			val->numberInTable=*(ct_consts->constnumber);
			appendCToC(val->type, NULL, NULL, "NONE", *(ct_consts->constnumber));
		}
	}
}

void TreeTraversal::parseFuncDefForTable(struct FuncDefInfo * funcdefstmt, int local)
{
	// Проверяем текущее состояние анализатора
	enum GlobalState lastState = gl_state;
	// Если вызов функции из глобального кода, меняем состояние
	if(lastState == _MAIN_STATE)
		gl_state = _FUNC_STATE;
	// Код функции
	// Создаем указатель на структуру с заголовком входной функции
	struct FunctionHeader* curHeader = new struct FunctionHeader;
	// Копируем в него имя функции
	curHeader->functionName = new char [strlen(funcdefstmt->functionName)+1];
	strcpy(curHeader->functionName,funcdefstmt->functionName);
	// И передаем указатель на список параметров
	curHeader->params=funcdefstmt->params;
	// Если такая функция еще не была объявлена
	if(!containsFuncHeader(this->funcHeaders,curHeader))
	{
		// Добавляем ее заголовок в массив функций
		this->funcHeaders.push_back(curHeader);
		// Формируем тип
		std::string type="";
		if(curHeader->params->first==NULL)
			type+="()";
		else
		{
			struct DefFuncParamInfo* el = curHeader->params->first;
			type+="(";
			while(el!=NULL)
			{
				type+="Lrtl/Value;";
				el=el->next;
			}
			type+=")";
		}
		type+="Lrtl/Value;";
		char* Ctype=new char [strlen(type.c_str())+1];
		strcpy(Ctype,type.c_str());
		int existingDesc=findMDesc(Ctype);
		struct MethodTable_Elem* curElem=new MethodTable_Elem;
		int mRef=0;
		appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,funcdefstmt->functionName,NULL,NULL,NULL,NULL));
		curElem->methodName=*(ct_consts->constnumber);
		// Делаем привязку к типу
		if(existingDesc==-1)
		{
			appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,Ctype,NULL,NULL,NULL,NULL));
			curElem->methodDesc=*(ct_consts->constnumber);
			// Делаем NameAndType
			appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
		}
		else
		{
			curElem->methodDesc=existingDesc;
			// Делаем NameAndType
			appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),existingDesc));
		}
		// Делаем methodRef
		appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,/*ct_consts->valueType*/prog->classConst,*(ct_consts->constnumber)));	
		mRef=*(ct_consts->constnumber);
		curElem->methodRef=mRef;
		appendToMethodTable(curElem);
		currentMR=mRef;
		currentFuncName=std::string(funcdefstmt->functionName);
		this->vars->count=0;
		struct DefFuncParamInfo* el = curHeader->params->first;
		while(el!=NULL)
		{
			struct Variable* var = new struct Variable;
			var->name=new char[strlen(el->paramName)+1];
			strcpy(var->name,el->paramName);
			var->localFor=mRef;
			appendToVarsTable(var);
			this->varDecls[currentFuncName].push_back(std::string(var->name));
			struct opInfo* curOp = new struct opInfo;
			curOp->local=mRef;
			curOp->FR=this->vars->count-1;
			curOp->opName=el->paramName;
			this->ops.push_back(curOp);
			el=el->next;
		}
		appendMToMT(funcdefstmt->functionName, mRef);
		funcdefstmt->numberInTable=mRef;
		// Проверяем ее тело
		parseStmtListForTable(funcdefstmt->body,mRef);
	}
	// Если был вызов функции из глобального кода, меняем состояние
	if(lastState == _MAIN_STATE)
	{
		gl_state = _MAIN_STATE;
		currentFuncName="global";
		local=NULL;
	}
}

void TreeTraversal::parseIfForTable(const struct IfStmtInfo * ifstmt, int local)
{
	// Проверяем условное выражение
	parseExprForTable(ifstmt->expr,local,ifstmt->expr->type);
	// Проверяем тело условия
	parseStmtListForTable(ifstmt->stmtlist,local);
	// Проверяем else блок, если он есть
	if(ifstmt->elsestmtlist!=NULL)
		parseStmtListForTable(ifstmt->elsestmtlist,local);
}

void TreeTraversal::parseForForTable(const struct ForStmtInfo * forstmt, int local)
{
	// Проверяем выражение счетчика цикла
	parseExprForTable(forstmt->counter,local,__COUNTER);
	// Проверяем выражение счетчика цикла
	parseExprForTable(forstmt->expr,local,forstmt->expr->type);
	// Проверяем тело цикла
	parseStmtListForTable(forstmt->stmtlist,local);
	// Проверяем else блок, если он есть
	if(forstmt->elsestmt!=NULL)
		parseStmtListForTable(forstmt->elsestmt,local);
}

void TreeTraversal::parseWhileForTable(const struct WhileStmtInfo * whilestmt, int local)
{
	// Проверяем выражение счетчика цикла
	parseExprForTable(whilestmt->expr,local,whilestmt->expr->type);
	// Проверяем тело цикла
	parseStmtListForTable(whilestmt->stmtlist,local);
	// Проверяем else блок, если он есть
	if(whilestmt->elsestmt!=NULL)
		parseStmtListForTable(whilestmt->elsestmt,local);
}