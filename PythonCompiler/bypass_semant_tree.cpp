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
	if(this->fields->first==NULL)
	{
		fe->next=NULL;
		this->fields->first=fe;
		this->fields->last=fe;
		return;
	}
	struct FieldTable_Elem* last_el=this->fields->last;
	this->fields->last=fe;
	this->fields->last->next=NULL;
	last_el->next=this->fields->last;
}

void TreeTraversal::appendToMethodTable(struct MethodTable_Elem* me)
{
	if(this->methods->first==NULL)
	{
		me->next=NULL;
		this->methods->first=me;
		this->methods->last=me;
		return;
	}
	struct MethodTable_Elem* last_el=this->methods->last;
	this->methods->last=me;
	this->methods->last->next=NULL;
	last_el->next=this->methods->last;
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

bool TreeTraversal::containsFuncHeader(std::vector<struct FunctionHeader*>& vec, struct FunctionHeader* func) const
{
	std::vector<struct FunctionHeader*>::const_iterator iter;  // Объявляем итератор для списка функций
	bool contains = false;							// Объявляем флаг того, что функция не найдена в списке
	// Для каждого элемента списка и пока не найдено значение..
	for(iter=vec.begin(); iter<vec.end()&&!contains; iter++) 
	{
		// Проверяем, равна ли текущая функция нужной
		contains=isEqualFuncHeaders((*iter),func);
	}
	return contains;
}

void TreeTraversal::deleteFuncHeader(std::vector<struct FunctionHeader*>& vec, struct FunctionHeader* func)
{
	std::vector<struct FunctionHeader*>::iterator iter;  // Объявляем итератор для списка функций
	// Для каждого элемента списка и пока не найдено значение..
	for(iter=vec.begin(); iter<vec.end(); iter++) 
	{
		// Проверяем, равна ли текущая функция нужной
		if(isEqualFuncHeaders((*iter),func))
		{
			// Удаляем нужное значение
			vec.erase(iter);
			// Выходим из цикла
			break;
		}
	}
}

void TreeTraversal::deleteFuncHeader(std::vector<struct FunctionHeader*>& vec, std::string funcName)
{
	std::vector<struct FunctionHeader*>::iterator iter;  // Объявляем итератор для списка функций
	// Для каждого элемента списка и пока не найдено значение..
	for(iter=vec.begin(); iter<vec.end(); iter++) 
	{
		// Проверяем, равна ли текущая функция нужной
		if(strcmp((*iter)->functionName,funcName.c_str())==0)
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
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"java/lang/Object",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));
	ct_consts->objectClass=*(ct_consts->constnumber);

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"<init>",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()V",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	ct_consts->init=*(ct_consts->constnumber);
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->objectClass,*(ct_consts->constnumber)));

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"Program",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));

	prog->classConst=*(ct_consts->constnumber);
	prog->firstField=NULL;

	// Запись имён функций
	// Главная функция
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"main",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"([Ljava/lang/String;)V",NULL,NULL,NULL));
	ct_consts->nameconstid = *(ct_consts->constnumber) - 1;
	// НУЖНО ЛИ ПИСАТЬ МЕТОДРЕФ ДЛЯ МЕЙНА?

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"rtl/Value",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));
	ct_consts->valueType = *(ct_consts->constnumber);
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->valueType,ct_consts->init));

	/*CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "rtl/Undefined";
	CT_AddConst(CONST_CLASS, (void*)&constnumber);
	mixedtype = constnumber;
	CT_AddConst2(CONST_METHODREF, mixedtype, CONSTANT_INIT);*/


	// rtl
	//----------------------------------
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"rtl/Lib",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));
	ct_consts->rtlClass = *(ct_consts->constnumber);
	
	// print
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"print",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)V",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//----------------------------------

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"sub",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//arifmentic = constnumber;
	ct_consts->arifmetic=*(ct_consts->constnumber);

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"add",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mul",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"div",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mod",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"toIntBool",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)I",NULL,NULL,NULL));
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

	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"eq",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//logic = constnumber;
	ct_consts->logic=*(ct_consts->constnumber);

	// + 4
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"less",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 8
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"more",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 12
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"lessOrEq",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 16
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"moreOrEq",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 20
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"not",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 24
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"or",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 28
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"and",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// + 32
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"notEq",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//mixedclass = constnumber;//??
	ct_consts->valueClass=*(ct_consts->constnumber);

	// mixedFromInt 4
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mixedFromInt",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(I)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// mixedFromUndefined 8
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mixedFromUndefined",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// mixedFromFloat 12
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mixedFromFloat",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(F)Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// mixedFromList 16
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mixedFromList",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()Lrtl/Value;",NULL,NULL,NULL));
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
}