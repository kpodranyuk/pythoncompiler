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
}

// Проверка дерева (первый обход)
void TreeTraversal::fixTree(struct StmtListInfo* root) throw(char*)
{
	if(root == NULL)
		throw "Root of tree is a null pointer\n";
	// Задаем начальные состояния
	this->gl_state=_MAIN_STATE;
	this->lc_state=_REGULAR_STATE;
	// Запускаем проверку дерева
	checkStatementList(root);
}

struct TreeTraversal::TableElement* TreeTraversal::makeTableEl(int num, int strNum, enum TableElemType type, int isStatic, std::string val)
{
	struct TableElement* te = new struct TableElement;
	te->number=num;
	te->strNumber=strNum;
	te->type=type;
	te->val=val;
	te->isStatic=isStatic;
	return te;
}

// Составление таблиц (второй обход)
void TreeTraversal::makeTables(const struct StmtListInfo* treeRoot)
{
	
	if(treeRoot == NULL)
		throw "Root of tree is a null pointer\n";
	currentFuncName=std::string("");
	// Задаем начальные состояния
	this->gl_state=_MAIN_STATE;
	this->lc_state=_REGULAR_STATE;
	// Запускаем проверку дерева
	//checkStatementList(root);
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
	this->varNames.clear();
	this->funcNames.clear();
	ValNum=0;
	TypeNum=0;
	MassTypeNum=0;
	parseStmtListForTable(treeRoot,this->globalTable,constNumber);
}

void TreeTraversal::parseStmtListForTable(const struct StmtListInfo* root, std::vector<struct TableElement*>& table, int* constNum)
{
	currentFuncName+=".csv";
	// Открываем файл констант функции
	FILE* tbl = fopen(currentFuncName.c_str(),"wt");
	// Пишем заголовок таблицы
	fprintf(tbl,"%s\n","\"Constant number\";\"String number\";\"Type\";\"Static\";\"Constant value\"");
	int constantNumber=1;
	// Добавляем в таблицу константу Code
	table.push_back(makeTableEl((*constNum)++,NULL,_UTF8,NULL,std::string("Code")));
	// Добавляем в таблицу константу класса main
	table.push_back(makeTableEl((*constNum)++,NULL,_UTF8,NULL,std::string("Main")));
	// Создаем для нее константу типа Class
	table.push_back(makeTableEl((*constNum)++,1,_CLASS,NULL,std::string("2")));
	// Добавляем в таблицу константу класса Value
	table.push_back(makeTableEl((*constNum)++,NULL,_UTF8,NULL,std::string("Value")));
	// Создаем для нее константу типа Class
	table.push_back(makeTableEl((*constNum)++,NULL,_CLASS,NULL,std::string("4")));
	ValNum=*constNum-1;
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
			parseFuncDefForTable(begining->funcdefstmt,table,constNum);
		}
		// Считаем следующий элемент списка новым текущим
		begining = begining->next;
	}
	std::vector<struct TableElement*>::iterator iter;  // Объявляем итератор для списка строк
	// Для каждого элемента списка и пока не найдено значение..
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
		if((*iter)->isStatic==NULL)
			output+="\";";
		else if((*iter)->isStatic)
			output+="1\";";
		else
			output+="0\";";
		output+="\"";
		output+=(*iter)->val;
		output+="\";";
		fprintf(tbl,"%s\n",output.c_str());
		//iter=table.erase(iter);
	}
}

void TreeTraversal::parseExprForTable(const struct ExprInfo * expr, std::vector<struct TableElement*>& table, int* constNum)
{
	// Если присвоение
	if(expr->type==_ASSIGN)
	{
		// Слева от присовения может быть только либо операнд, либо взятие по индексу массива
		// Если слева операнд
		if(expr->left->type==_OPERAND)
		{
			// Проверяем, есть ли он уже в списке переменных
			// И если нет, то добавляем
			std::string opName = std::string(expr->left->idName);
			if(!containsString(this->varNames,opName))
			{
				this->varNames.push_back(opName);
				// Делаем привязку к типу
				if(TypeNum==0&&expr->right->type!=_ARRINIT)
				{
					table.push_back(makeTableEl((*constNum)++,NULL,_UTF8,NULL,std::string("LValue;")));
					TypeNum=*constNum-1;
				}
				else if(MassTypeNum==0&&expr->right->type==_ARRINIT)
				{
					table.push_back(makeTableEl((*constNum)++,NULL,_UTF8,NULL,std::string("[LValue;")));
					MassTypeNum=*constNum-1;
				}
				// Добавляем в таблицу данные о переменной
				table.push_back(makeTableEl((*constNum)++,expr->loc->firstLine,_UTF8,NULL,opName));
				// Делаем NameAndType
				char buf[50]="";
				if(expr->right->type==_ARRINIT)
					sprintf(buf,"%d,%d",*constNum-1,MassTypeNum);
				else
					sprintf(buf,"%d,%d",*constNum-1,TypeNum);
				table.push_back(makeTableEl((*constNum)++,expr->loc->firstLine,_NAMEnTYPE,NULL,std::string(buf)));
				// Делаем fieldRef
				buf[0]='\0';
				sprintf(buf,"%d,%d",ValNum,*constNum-1);
				table.push_back(makeTableEl((*constNum)++,expr->loc->firstLine,_FIELDREF,NULL,std::string(buf)));				
			}
			// Проверяем правую часть присвоения
			parseExprForTable(expr->right,table,constNum);//checkExpr(expr->right);
		}
		// Иначе если слева взятие по индексу
		/*else if(expr->left->type==_ARRID)
		{
			// Считаем, что проверять левую часть не нужно - на этап генерации кода
			// Проверяем правую часть
			parseExprForTable(expr->right,table,constNum); //checkExpr(expr->right);
		}*/
	}
}

void TreeTraversal::parseFuncDefForTable(const struct FuncDefInfo * funcdefstmt, std::vector<struct TableElement*>& table, int* constNum)
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
	if(!containsFuncHeader(this->funcNames,curHeader))
	{
		// Добавляем ее заголовок в массив функций
		this->funcNames.push_back(curHeader);
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
		table.push_back(makeTableEl((*constNum)++,funcdefstmt->nameLoc->firstLine,_UTF8,1,type));
		// Добавляем в таблицу данные о переменной
		table.push_back(makeTableEl((*constNum)++,funcdefstmt->nameLoc->firstLine,_UTF8,1,std::string(curHeader->functionName)));
		// Делаем NameAndType
		char buf[50]="";
		sprintf(buf,"%d,%d",*constNum-2,*constNum-1);
		table.push_back(makeTableEl((*constNum)++,funcdefstmt->nameLoc->firstLine,_NAMEnTYPE,1,std::string(buf)));
		// Делаем methodRef
		buf[0]='\0';
		sprintf(buf,"%d,%d",ValNum,*constNum-1);
		table.push_back(makeTableEl((*constNum)++,funcdefstmt->nameLoc->firstLine,_METHODREF,1,std::string(buf)));

		currentFuncName=std::string(curHeader->functionName);

		std::vector<struct TableElement*> funcTable;
		int* funcConsts = new int;
		*funcConsts=1;
		// Проверяем ее тело
		parseStmtListForTable(funcdefstmt->body,funcTable,funcConsts);//checkStatementList(funcdefstmt->body);
		//programm_table.insert(TablePair(std::string(curHeader->functionName),funcTable));
		prog.push_back(TablePair(std::string(curHeader->functionName),funcTable));
	}
	// Иначе выбрасываем исключение
	/*else
	{
		char* bufstr = new char [50];
		sprintf(bufstr,"(%d.%d-%d.%d)",funcdefstmt->nameLoc->firstLine,funcdefstmt->nameLoc->firstColumn,funcdefstmt->nameLoc->lastLine,funcdefstmt->nameLoc->lastColumn);
		// Если не объявлен, выдаем ошибку с именем операнда
		char* errStr = new char[30+strlen(curHeader->functionName)+62];
		strcpy(errStr,"Can't define same function: ");
		strcat(errStr,curHeader->functionName);
		strcat(errStr,"\nLocation: ");
		strcat(errStr,bufstr);
		throw errStr;
	}*/
	// Если был вызов функции из глобального кода, меняем состояние
	if(lastState == _MAIN_STATE)
	{
		gl_state = _MAIN_STATE;
		currentFuncName="global.csv";
	}
}

// Печать аттрибутированного дерева (третий обход - ?)
void TreeTraversal::printTree(const struct StmtListInfo* treeRoot)
{
}

void TreeTraversal::checkStatementList(struct StmtListInfo* root) throw(char*)
{
	// Создаем локальный указатель на элемент входного дерева
	struct StmtInfo* begining;
	// Считаем первый элемент списка начальным
	begining = root->first;
	// Пока текущий элемент списка не последний..
	while(begining!=NULL)
	{
		// В зависимости от типа узла/элемента вызываем соответствующую функцию
		// Запомнить номер текущего дочернего узла
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
			// Если мы сейчас находимся не в цикле, бросаем исключение
			if(this->lc_state == _REGULAR_STATE)
			{
				// Формируем строку с локацией токена
				char* bufstr = new char [50];
				sprintf(bufstr,"(%d.%d-%d.%d)",begining->loc->firstLine,begining->loc->firstColumn,begining->loc->lastLine,begining->loc->lastColumn);
				// Соединяем все в строке сообщения
				char* errStr = new char[31+62];
				if(begining->type==_BREAK)
					strcpy(errStr,"Found break in global code.");
				else if(begining->type==_CONTINUE)
					strcpy(errStr,"Found continue in global code.");
				strcat(errStr,"\nLocation: ");
				strcat(errStr,bufstr);
				// Бросаем строку исключением
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
		// Считаем следующий элемент списка новым текущим
		begining = begining->next;
	}
}

void TreeTraversal::checkExpr(struct ExprInfo * expr) throw(char*)
{
	// Если операнд
	if(expr->type==_OPERAND)
	{
		// Проверяем, что он уже объявлен
		std::string opName = std::string(expr->idName);
		if(!containsString(this->varNames,opName))
		{
			char* bufstr = new char [50];
			sprintf(bufstr,"(%d.%d-%d.%d)",expr->loc->firstLine,expr->loc->firstColumn,expr->loc->lastLine,expr->loc->lastColumn);
			// Если не объявлен, выдаем ошибку с именем операнда
			char* errstr=new char[35+strlen(expr->idName)+62];
			errstr[0]='\0';
			strcpy(errstr,"Can't use undefined operand - ");
			strcat(errstr,expr->idName);
			strcat(errstr,"\nLocation: ");
			strcat(errstr,bufstr);
			throw errstr;
		}
	}
	// Если присвоение
	else if(expr->type==_ASSIGN)
	{
		// Слева от присовения может быть только либо операнд, либо взятие по индексу массива
		// Если слева операнд
		if(expr->left->type==_OPERAND)
		{
			// Проверяем, есть ли он уже в списке переменных
			// И если нет, то добавляем
			std::string opName = std::string(expr->left->idName);
			if(!containsString(this->varNames,opName))
			{
				this->varNames.push_back(opName);
			}
			// Проверяем правую часть присвоения
			checkExpr(expr->right);
		}
		// Иначе если слева взятие по индексу, то меняем тип операции с _ASSIGN на _ARRID_AND_ASSIGN
		else if(expr->left->type==_ARRID)
		{
			// Проверяем правую часть
			checkExpr(expr->right);
			// Проверяем, что взятие производится уже по объявленному массиву(если операнд маассив)
			struct ExprInfo* arr = expr->left;
			if(arr->left->type==_OPERAND)
				checkExpr(arr->left);
			
			// Изменяем узел
			expr->type=_ARRID_AND_ASSIGN;
			struct ExprInfo* leftNode = expr->left;
			expr->left=leftNode->left;
			expr->middle=leftNode->right;
			leftNode->left=NULL;
			leftNode->right=NULL;

		}
		// Иначе бросаем исключение, что нельзя присвоить значение чему-то кроме операнда или элемента массива
		else
		{
			char* bufstr = new char [50];
			sprintf(bufstr,"(%d.%d-%d.%d)",expr->loc->firstLine,expr->loc->firstColumn,expr->loc->lastLine,expr->loc->lastColumn);
			// Если не объявлен, выдаем ошибку с именем операнда
			char* errstr=new char[64+62];
			errstr[0]='\0';
			strcpy(errstr,"Can't assign value to anything except operand or array element.");
			strcat(errstr,"\nLocation: ");
			strcat(errstr,bufstr);
			throw errstr;
		}
	}
	// Если действие - не, проверяем выражение
	else if(expr->type==_NOT)
	{
		checkExpr(expr->left);
	}
	// Если действие над массивом - append/remove 
	else if(expr->type==_ARRACT)
	{
		// Проверяем, что бы действие было либо append либо remove
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
		// Проверяем, чтобы массив был операндом и был в списке переменных
		if(expr->left->type==_OPERAND)
			checkExpr(expr->left);

		// Проверяем правую часть
		checkExpr(expr->right);
	}
	// Если инициализация массива 
	else if(expr->type==_ARRINIT)
	{
		// Считаем, что проверять левую часть не нужно - на этап генерации кода
		// Создаем локальный указатель на начала списка элементов
		struct ExprInfo* begining;
		// Считаем первый элемент списка начальным
		begining = expr->arglist->first;
		// Пока текущий элемент списка не последний..
		while(begining!=NULL)
		{
			// Проверяем текущий элемент списка
			checkExpr(begining);
			// Считаем следующий элемент списка новым текущим
			begining = begining->next;
		}
	}
	else if(expr->type==_FUNCCALL)
	{
		// У вызова функции два параметра - имя функции - операнд
		// И парамлист - как список фактических аргументов
		// Нужно проверить
		// 1. Имена функций совпадают
		// 2. Количество аргументов совпадают
		// 3. При вызове "д = .." после параметров по значению
		// 4. Количество аргументов по-умолчанию считать отдельно
		// 5. Затереть переменную с тем же именем из списка
		std::string name = std::string(expr->idName);
		std::vector<struct FunctionHeader*>::const_iterator iter;  // Объявляем итератор для списка функций
		bool contains = false;							// Объявляем флаг того, что функция не найдена в списке
		// Для каждого элемента списка и пока не найдено значение..
		for(iter=this->funcNames.begin(); iter<this->funcNames.end()&&!contains; iter++) 
		{
			// Проверяем, равна ли текущая функция нужной
			contains=strcmp(name.c_str(),(*iter)->functionName);
		}
		// Если функция с таким именем объявлена..
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
						throw ""; // Выбросить исключение
				}
				else
				{
					fact_count++;
				}
				checkExpr(call_el);
				if(call_el->next!=NULL)
				{
					if(call_el->next->type!=_ASSIGN&&call_el->type==_ASSIGN)
						throw ""; // Выбросить исключение
				}
				call_el=call_el->next;
			}
			// Если количество аргументов совпадает
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
								throw ""; // Выбросить исключение
						}
						el=el->next;
					}
				}
			}
			else
				throw ""; // Выбросить исключение
		}
	}
	else if(expr->type==_UMINUS)
	{
		// Унарный минус может быть только перед целым числом или выражением, возвращающим целое число
		// Проверим первое
		if(expr->left->type==_VARVAL)
		{
			struct ValInfo* val = expr->left->exprVal;
			if(val->type!=_NUMBER)
			{
				char* bufstr = new char [50];
				sprintf(bufstr,"(%d.%d-%d.%d)",val->loc->firstLine,val->loc->firstColumn,val->loc->lastLine,val->loc->lastColumn);
				// Если не объявлен, выдаем ошибку с именем операнда
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

void TreeTraversal::checkIfStmt(struct IfStmtInfo * ifstmt)
{
}

void TreeTraversal::checkWhileStmt(struct WhileStmtInfo * whilestmt) throw(char*)
{
	// Проверка установка флагов и состояний при входе в цикл
	bool cycle=false;
	if(lc_state==_REGULAR_STATE)
	{
		lc_state = _CYCLE_STATE;
		cycle=true;
	}
	
	// Код функции
	checkExpr(whilestmt->expr);//проверка условного выражения
	checkStatementList(whilestmt->stmtlist);//проверка тела цикла
	if(whilestmt->elsestmt!=NULL)
		checkStatementList(whilestmt->elsestmt);//проверка блока else цикла

	// Установка состояний при выходе из цикла
	if(cycle==true)
	{
		lc_state = _REGULAR_STATE;
	}
}

void TreeTraversal::checkForStmt(struct ForStmtInfo * forstmt) throw(char*)
{
	// Проверка установка флагов и состояний при входе в цикл
	bool cycle=false;
	if(lc_state==_REGULAR_STATE)
	{
		lc_state = _CYCLE_STATE;
		cycle=true;
	}
	
	// Код функции
	// Проверяем, есть ли он уже в списке переменных
	// И если нет, то добавляем
	std::string opName = std::string(forstmt->counter);
	if(!containsString(this->varNames,opName))
	{
		this->varNames.push_back(opName);
	}
	checkExpr(forstmt->expr);//проверка выражения, по чем проходит цикл
	checkStatementList(forstmt->stmtlist);//проверка тела цикла
	if(forstmt->elsestmt!=NULL)
		checkStatementList(forstmt->elsestmt);//проверка блока else цикла

	// Установка состояний при выходе из цикла
	if(cycle==true)
	{
		lc_state = _REGULAR_STATE;
	}
}

void TreeTraversal::checkFuncDefStmt(struct FuncDefInfo * funcdefstmt)
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
	if(!containsFuncHeader(this->funcNames,curHeader))
	{
		// Добавляем ее заголовок в массив функций
		this->funcNames.push_back(curHeader);
		// Добавляем аргументы функции в список констант
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
		// Проверяем ее тело
		checkStatementList(funcdefstmt->body);

		std::vector<std::string>::iterator iter;  // Объявляем итератор для списка строк
		// Для каждого элемента списка..
		for(iter=onlyInHeader.begin(); iter<onlyInHeader.end(); iter++) 
		{
			deleteString(this->varNames,*iter);
		}
	}
	// Иначе выбрасываем исключение
	else
	{
		char* bufstr = new char [50];
		sprintf(bufstr,"(%d.%d-%d.%d)",funcdefstmt->nameLoc->firstLine,funcdefstmt->nameLoc->firstColumn,funcdefstmt->nameLoc->lastLine,funcdefstmt->nameLoc->lastColumn);
		// Если не объявлен, выдаем ошибку с именем операнда
		char* errStr = new char[30+strlen(curHeader->functionName)+62];
		strcpy(errStr,"Can't define same function: ");
		strcat(errStr,curHeader->functionName);
		strcat(errStr,"\nLocation: ");
		strcat(errStr,bufstr);
		throw errStr;
	}
	// Если был вызов функции из глобального кода, меняем состояние
	if(lastState == _MAIN_STATE)
		gl_state = _MAIN_STATE;
}

void TreeTraversal::checkContinueBreakStmt(enum StmtType type)
{
}

void TreeTraversal::checkReturnStmt(struct StmtInfo* retStmt, struct ExprInfo * expr) throw(char*)
{
	// Если мы сейчас находимся в главном коде программы, бросаем исключение
	if(this->gl_state == _MAIN_STATE)
	{
		char* bufstr = new char [50];
		sprintf(bufstr,"(%d.%d-%d.%d)",retStmt->loc->firstLine,retStmt->loc->firstColumn,retStmt->loc->lastLine,retStmt->loc->lastColumn);
		// Если не объявлен, выдаем ошибку с именем операнда
		char* errStr = new char[30+62];
		strcpy(errStr,"Found return in global code.");
		strcat(errStr,"\nLocation: ");
		strcat(errStr,bufstr);
		throw errStr;
	}
	// Иначе проверяем возвращаемое выражение
	checkExpr(expr);
}

void TreeTraversal::checkDelStmt(struct ExprInfo * expr)
{
	// Из правил бизона известно, что выражением может быть только операнд
	// Поэтому проверяем, не удален ли уже операнд
	checkExpr(expr);
	// Если операнд еще не удален, удаляем его из массива имен перменных
	deleteString(this->varNames,std::string(expr->idName));
}

void TreeTraversal::checkFuncParams(struct DefFuncParamListInfo* params)
{
}


/*!
*	---------------- БЛОК ВСПОМОГАТЕЛЬНЫХ МЕТОДОВ ДЛЯ РАБОТЫ СО СПИСКАМИ И КОНТЕЙНЕРАМИ ----------------
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