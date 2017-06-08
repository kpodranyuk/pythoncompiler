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
//	this->varNames.clear();
	this->funcHeaders.clear();
	valClassDesc=0;
	typeDesc=0;
	this->ct_consts=new ConstTable_Consts;
	this->ct_consts->constnumber=new int(0);
	this->prog=new ClassTable_Elem;
	// TODO СОЗДАТЬ ФУНКЦИЮ ГЕНЕРАЦИИ ВСТАВКИ РТЛ ТАБЛИЦЫ
	initializeConstTable();
	//int constantNumber=1;
	//parseStmtListForTable(treeRoot,constNumber,NULL);
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
}

void TreeTraversal::parseStmtListForTable(const struct StmtListInfo* root, int* constNum, int local)
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
		// Считаем следующий элемент списка новым текущим
		begining = begining->next;
	}
}

void TreeTraversal::parseExprForTable(const struct ExprInfo * expr, int* constNum, int local, enum ExprType typeAboveExpression)
{
	// Если одна из операций где идет инициализация или взятие элемента по индексу
	if(expr->type==_ASSIGN || expr->type==_ARRID || expr->type==_ARRID_AND_ASSIGN)
	{
		parseExprForTable(expr->left, constNum, local, expr->type);
		if(expr->type==_ARRID_AND_ASSIGN)
		{
			parseExprForTable(expr->middle, constNum, local, expr->type);
		}
		parseExprForTable(expr->right, constNum, local, expr->type);
	}

	// Если операнд
	if(expr->type==_OPERAND)
	{
		// Проверяем, есть ли он уже в списке переменных
		// И если нет, то добавляем
		std::string opName = std::string(expr->idName);
		/*if(!containsString(this->varNames,opName))
		{
			this->varNames.push_back(opName);
			// Делаем привязку к типу
			// Добавляем в таблицу данные о переменной
			// Делаем NameAndType
			// Делаем fieldRef				
		}*/
	}

	// Если константа
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
				type+="LValue;";
				el=el->next;
			}
			type+=")";
		}
		type+="LValue;";
		// Добавляем в таблицу данные о переменной
		// Делаем NameAndType
		// Делаем methodRef
		//currentFuncName=std::string(curHeader->functionName);

		std::vector<struct TableElement*> funcTable;
		int* funcConsts = new int;
		*funcConsts=1;
		// Проверяем ее тело
		parseStmtListForTable(funcdefstmt->body,constNum,local);
	}
	// Иначе выбрасываем исключение

	// Если был вызов функции из глобального кода, меняем состояние
	if(lastState == _MAIN_STATE)
	{
		gl_state = _MAIN_STATE;
		//currentFuncName="global.csv";
		local=NULL;
	}
}