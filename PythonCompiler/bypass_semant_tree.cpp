#include "bypass_semant_tree.h"

#pragma warning(disable : 4996)

/** Формат файла таблицы констант
* столбцы: № константы; № строки (?); значение
* запись в файл ведется в формате csv, разделитель - ;
* запись заголовка: "Constant number";"String number";"Type";"Static";"Constant value";
* пример записи константы: 1;;"utf-8";"Code"
* таблицу констант для глобального кода всегда писать в файл global.csv
* таблицу констант для функций писать в файл <func_name>.csv
**/

/* Напоминалка по Питону:
* !!! Функция может видеть глобальные переменные, но глобальный код переменные функции - нет !!!
* !!! Функция использует глобальне переменные, но не меняет их значение (по значению, не по ссылке) !!!
*/

// Конструктор по умолчанию
TreeTraversal::TreeTraversal()
{
	libFuncs.push_back("print");
	libFuncs.push_back("input");
	libFuncs.push_back("repr");
	libFuncs.push_back("int");
}

// Печать аттрибутированного дерева (третий обход - ?)
void TreeTraversal::printTree(const struct StmtListInfo* treeRoot)
{
}

/*!
*	---------------- БЛОК ВСПОМОГАТЕЛЬНЫХ МЕТОДОВ ДЛЯ РАБОТЫ СО СПИСКАМИ И КОНТЕЙНЕРАМИ ----------------
*/

bool TreeTraversal::isLibFunctionCall(struct ExprInfo * expr)
{
	std::string opName = std::string(expr->idName);
	if(!containsString(this->libFuncs,opName))
		return false;
	// Создаем указатель на элемент списка аргументов вызова функции
	struct ExprInfo* call_el;
	if(strcmp(opName.c_str(),"input")==0||strcmp(opName.c_str(),"print")==0)
	{		
		call_el = expr->arglist->first;
		// Для каждого фактического аргумента
		while(call_el!=NULL)
		{
			checkExpr(call_el,true);
			call_el=call_el->next;
		}
	}
	else if(strcmp(opName.c_str(),"repr")==0)
	{
		int paramsCount=0;
		call_el = expr->arglist->first;
		// Для каждого фактического аргумента
		while(call_el!=NULL)
		{
			paramsCount++;
			if(paramsCount>1)
				throw makeStringForException("Too much params for call: repr",expr->loc);
			checkExpr(call_el,true);
			call_el=call_el->next;
		}
		if(paramsCount==0)
			throw makeStringForException("Need param for call: repr",expr->loc);
	}
	else if(strcmp(opName.c_str(),"int")==0)
	{
		int paramsCount=0;
		call_el = expr->arglist->first;
		// Для каждого фактического аргумента
		while(call_el!=NULL)
		{
			paramsCount++;
			if(paramsCount>2)
				throw makeStringForException("Too much params for call: int",expr->loc);
			checkExpr(call_el,true);
			call_el=call_el->next;
		}
		if(paramsCount==0)
			throw makeStringForException("Need param for call: int",expr->loc);
	}
	return true;
}

void TreeTraversal::appendToConstTable(struct ConstTable_Elem* ce)
{
	struct ConstTable_Elem* last_el=this->ct->last;
	this->ct->last=ce;
	this->ct->last->next=NULL;
	last_el->next=this->ct->last;
}

void TreeTraversal::appendToFieldTable(struct FieldTable_Elem* fe)
{
	if(this->prog->firstField==NULL)
	{
		fe->next=NULL;
		this->prog->firstField=fe;
		this->prog->lastField=fe;
		return;
	}
	struct FieldTable_Elem* last_el=this->prog->lastField;
	this->prog->lastField=fe;
	this->prog->lastField->next=NULL;
	last_el->next=this->prog->lastField;
}

void TreeTraversal::appendToMethodTable(struct MethodTable_Elem* me)
{
	if(this->prog->methodsFirst==NULL)
	{
		me->next=NULL;
		this->prog->methodsFirst=me;
		this->prog->methodsLast=me;
		return;
	}
	struct MethodTable_Elem* last_el=this->prog->methodsLast;
	this->prog->methodsLast=me;
	this->prog->methodsLast->next=NULL;
	last_el->next=this->prog->methodsLast;
}

void TreeTraversal::appendToVarsTable(struct Variable* v)
{
	if(this->vars->first==NULL)
	{
		this->vars->count=0;
		v->next=NULL;
		v->num=(this->vars->count)++;
		this->vars->first=v;
		this->vars->last=v;
		return;
	}
	v->num=(this->vars->count)++;
	struct Variable* last_v=this->vars->last;
	this->vars->last=v;
	this->vars->last->next=NULL;
	last_v->next=this->vars->last;
}

void TreeTraversal::appendToMDescs(char* desc, int num)
{
	struct mDescInfo* m=new struct mDescInfo;
	m->num=num;
	m->desc=desc;
	mDescs.push_back(m);
}

void TreeTraversal::appendMToMT(char* name, int methodRef)
{
	struct methodInfo* curMI=new struct methodInfo;
	curMI->name=name;
	curMI->MR=methodRef;
	this->MRs.push_back(curMI);
}

void TreeTraversal::appendCToC(enum ValType type, int val, float f_val, char* charVal, int number)
{
	struct constInfo* curC=new struct constInfo;
	curC->t=type;
	curC->val=val;
	curC->f_val=f_val;
	curC->strVal=charVal;
	curC->num=number;
	this->consts.push_back(curC);
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
		// Равны, когда равны названия и значения (если есть)
		// Если неравны имена параметров - списки не равны
		if(strcmp(beginFirst->paramName,beginSecond->paramName)!=0)
			return false;
		// Если не равны значения параметров - списки не равны
		if(!isEqualVarVals(beginFirst->paramVal,beginSecond->paramVal))
			return false;
		// Так как элементы равны, сдвигаемся к сравнению следующих элементов
		beginFirst = beginFirst->next;
		beginSecond = beginSecond->next;
	}
	// Если списки равны, то оба указателя - нулл, иначе - не равны
	return beginFirst==NULL&&beginSecond==NULL;
}

bool TreeTraversal::isEqualVarVals(struct ValInfo* first, struct ValInfo* second) const
{
	// Если оба нулл, то равны
	if(first==NULL&&second==NULL)
		return true;
	// Если оба не нулл..
	else if(first!=NULL&&second!=NULL)
	{
		// Если равны типы, значения целого числа, логические значения и строковые значения, то равны
		return (first->type==second->type&&
			first->intVal==second->intVal&&
			first->logVal==second->logVal&&
			(first->stringVal==NULL&&second->stringVal==NULL||
			first->stringVal!=NULL&&second->stringVal!=NULL&&
			strcmp(first->stringVal,second->stringVal)==0));
	}
	return false;
}

bool TreeTraversal::containsFuncHeader(std::vector<struct FunctionHeader>& vec, struct FunctionHeader func) const
{
	std::vector<struct FunctionHeader>::iterator iter;  // Объявляем итератор для списка функций
	bool contains = false;							// Объявляем флаг того, что функция не найдена в списке
	// Для каждого элемента списка и пока не найдено значение..
	for(iter=vec.begin(); iter<vec.end()&&!contains; iter++) 
	{
		// Проверяем, равна ли текущая функция нужной
		contains=isEqualFuncHeaders(&(*iter),&func);
	}
	return contains;
}

void TreeTraversal::deleteFuncHeader(std::vector<struct FunctionHeader>& vec, struct FunctionHeader func)
{
	std::vector<struct FunctionHeader>::iterator iter;  // Объявляем итератор для списка функций
	// Для каждого элемента списка и пока не найдено значение..
	for(iter=vec.begin(); iter<vec.end(); iter++) 
	{
		// Проверяем, равна ли текущая функция нужной
		if(isEqualFuncHeaders(&(*iter),&func))
		{
			// Удаляем нужное значение
			vec.erase(iter);
			// Выходим из цикла
			break;
		}
	}
}

void TreeTraversal::deleteFuncHeader(std::vector<struct FunctionHeader>& vec, std::string funcName)
{
	std::vector<struct FunctionHeader>::iterator iter;  // Объявляем итератор для списка функций
	// Для каждого элемента списка и пока не найдено значение..
	for(iter=vec.begin(); iter<vec.end(); iter++) 
	{
		// Проверяем, равна ли текущая функция нужной
		if(strcmp((*iter).functionName,funcName.c_str())==0)
		{
			// Удаляем нужное значение
			vec.erase(iter);
			// Выходим из цикла
			break;
		}
	}
}

bool TreeTraversal::containsString(std::vector<std::string>& vec, std::string str) const
{
	std::vector<std::string>::const_iterator iter;  // Объявляем итератор для списка строк
	bool contains = false;							// Объявляем флаг того, что строка не найдена в списке
	// Для каждого элемента списка и пока не найдено значение..
	for(iter=vec.begin(); iter<vec.end()&&!contains; iter++) 
	{
		// Проверяем, равна ли текущая строка нужной
		contains=(*iter).compare(str)==0;
	}
	return contains;
}

void TreeTraversal::deleteString(std::vector<std::string>& vec, std::string str)
{
	if(containsString(vec,str))
	{
		std::vector<std::string>::iterator iter;  // Объявляем итератор для списка строк
		// Для каждого элемента списка и пока не найдено значение..
		for(iter=vec.begin(); iter<vec.end(); iter++) 
		{
			// Проверяем, равна ли текущая строка нужной
			if((*iter).compare(str)==0)
			{
				// Удаляем нужное значение
				vec.erase(iter);
				// Выходим из цикла
				break;
			}
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

void TreeTraversal::initializeConstTable()
{
	// Номер константы
	*(ct_consts->constnumber)=1;
	// Выделяем память
	ct=new struct ConstTable_List;
	ct->first=new struct ConstTable_Elem;
	// Инициализируем первый элемент
	ct->first->type=CONST_UTF8;
	ct->first->value.utf8="Code";
	ct->first->next=NULL;
	ct->first->numberInTable=(*(ct_consts->constnumber));
	// Связываем первый и последний элементы
	ct->last=ct->first;
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"java/lang/Object",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),NULL));
	ct_consts->objectClass=*(ct_consts->constnumber);

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"<init>",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()V",NULL,NULL,NULL,NULL));
	appendToMDescs("()V",*(ct_consts->constnumber));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	ct_consts->init=*(ct_consts->constnumber);
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->objectClass,*(ct_consts->constnumber)));

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"Program",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),NULL));

	prog->classConst=*(ct_consts->constnumber);
	//prog->firstField=NULL;

	// Запись имён функций
	// Главная функция
	struct MethodTable_Elem* curElem=new MethodTable_Elem;
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"main",NULL,NULL,NULL,NULL));
	curElem->methodName=*(ct_consts->constnumber);
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"([Ljava/lang/String;)V",NULL,NULL,NULL,NULL));
	curElem->methodDesc=*(ct_consts->constnumber);
	appendToMDescs("([Ljava/lang/String;)V",*(ct_consts->constnumber));
	ct_consts->nameconstid = *(ct_consts->constnumber) - 1;
	curElem->methodRef=NULL;
	curElem->localVarsCount=2;
	appendToMethodTable(curElem);
	// НУЖНО ЛИ ПИСАТЬ МЕТОДРЕФ ДЛЯ МЕЙНА?

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"rtl/Value",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),NULL));
	ct_consts->valueType = *(ct_consts->constnumber);
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->valueType,ct_consts->init));

	// rtl
	//----------------------------------
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"rtl/Lib",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),NULL));
	ct_consts->rtlClass = *(ct_consts->constnumber);
	
	// toString
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"toString",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL,NULL));
	appendToMDescs("(Lrtl/Value;)Lrtl/Value;",*(ct_consts->constnumber));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	appendMToMT("repr", *(ct_consts->constnumber));
	// print
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"print",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)V",NULL,NULL,NULL,NULL));
	appendToMDescs("(Lrtl/Value;)V",*(ct_consts->constnumber));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	appendMToMT("print", *(ct_consts->constnumber));
	// input
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"input",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//appendToMDescs("(Lrtl/Value;)Lrtl/Value;",*(ct_consts->constnumber));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	appendMToMT("input", *(ct_consts->constnumber));
	// toNumber
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"toNumber",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//appendToMDescs("(Lrtl/Value;)Lrtl/Value;",*(ct_consts->constnumber));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	appendMToMT("int", *(ct_consts->constnumber));
	// toNumber&base
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"tonumberBase",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL,NULL));
	appendToMDescs("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",*(ct_consts->constnumber));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//----------------------------------

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"sub",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//appendToMDescs("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",*(ct_consts->constnumber));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//arifmentic = constnumber;
	ct_consts->arifmetic=*(ct_consts->constnumber);

	//+3
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"add",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	//+6
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mul",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	//+9
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"pow",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	//+12
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"div",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	//+15
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mod",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	//+17
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"toIntBool",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)I",NULL,NULL,NULL,NULL));
	appendToMDescs("(Lrtl/Value;)I",*(ct_consts->constnumber));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));


	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"ListGet",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	ct_consts->list=*(ct_consts->constnumber);
	//+4
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"ListSet",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;Lrtl/Value;)V",NULL,NULL,NULL,NULL));
	appendToMDescs("(Lrtl/Value;Lrtl/Value;Lrtl/Value;)V",*(ct_consts->constnumber));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	//+7
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"ListAppend",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	//+10
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"ListAddInitialize",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	//+13
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"ListRemove",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
	
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"eq",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//logic = constnumber;
	ct_consts->logic=*(ct_consts->constnumber);

	// + 3
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"less",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 6
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"more",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 9
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"lessOrEq",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 12
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"moreOrEq",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 15
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"not",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 18
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"or",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 21
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"and",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 24
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"notEq",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//mixedclass = constnumber;//??
	ct_consts->valueClass=*(ct_consts->constnumber);

	// mixedFromInt 4
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"valueFromInt",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(I)Lrtl/Value;",NULL,NULL,NULL,NULL));
	appendToMDescs("(I)Lrtl/Value;",*(ct_consts->constnumber));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// mixedFromUndefined 8
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"valueFromNone",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()Lrtl/Value;",NULL,NULL,NULL,NULL));
	appendToMDescs("()Lrtl/Value;",*(ct_consts->constnumber));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// mixedFromList 16
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"valueFromList",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("()Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"valueFromBoolean",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(I)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"valueFromFloat",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(F)Lrtl/Value;",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"valueFromString",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Ljava/lang/String;)Lrtl/Value;",NULL,NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"intDiv",NULL,NULL,NULL,NULL));
	//appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,NULL,*(ct_consts->constnumber),findMDesc("(Lrtl/Value;Lrtl/Value;)Lrtl/Value;")));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
}


struct ClassTable_Elem* TreeTraversal::getClassTable()
{
	int fieldsC=0;
	struct FieldTable_Elem* elem=prog->firstField;
	while(elem!=NULL)
	{
		fieldsC++;
		elem=elem->next;
	}
	prog->fieldCount=fieldsC;
	int methodsC=0;
	struct MethodTable_Elem* mElem=prog->methodsFirst;
	while(mElem!=NULL)
	{
		methodsC++;
		mElem=mElem->next;
	}
	prog->methodCount=methodsC;

	mElem = prog->methodsFirst;
	while(mElem!=NULL)
	{
		int countVar=0;
		Variable* var = this->vars->first;
		while(var!=NULL)
		{
			if(var->localFor==mElem->methodRef)
				countVar++;

			var=var->next;
		}
		mElem->localVarsCount=countVar;

		mElem=mElem->next;
	}

	return this->prog;
}

struct ConstTable_List*  TreeTraversal::getConstTableList()
{
	return this->ct;
}

struct ConstTable_Consts*  TreeTraversal::getConstTableConsts()
{
	return this->ct_consts;
}

struct VariableTable_List*  TreeTraversal::getVariableTableList()
{
	return this->vars;
}