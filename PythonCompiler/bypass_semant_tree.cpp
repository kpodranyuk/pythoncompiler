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
		throw makeStringForException("Root of tree is a null pointer\n",NULL);
	// Задаем начальные состояния
	this->gl_state=_MAIN_STATE;
	this->lc_state=_REGULAR_STATE;
	// Запускаем проверку дерева
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

// Составление таблиц (второй обход)
void TreeTraversal::makeTables(const struct StmtListInfo* treeRoot)
{
	
	if(treeRoot == NULL)
		throw makeStringForException("Root of tree is a null pointer\n",NULL);
	currentFuncName=std::string("");
	// Задаем начальные состояния
	this->gl_state=_MAIN_STATE;
	this->lc_state=_REGULAR_STATE;
	// Запускаем проверку дерева
	//checkStatementList(root);
	currentFuncName="global.csv";
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
	this->funcHeaders.clear();
	ValNum=0;
	TypeNum=0;
	MassTypeNum=0;
	// Открываем файл констант функции
	FILE* tbl = fopen(currentFuncName.c_str(),"wt");
	// Пишем заголовок таблицы
	fprintf(tbl,"%s\n","\"Constant number\";\"String number\";\"Type\";\"Static\";\"Constant value\";\"Local for\";");
	int constantNumber=1;
	// Добавляем в таблицу константу Code
	globalTable.push_back(makeTableEl((*constNumber)++,NULL,_UTF8,NULL,std::string("Code"),NULL));
	// Добавляем в таблицу константу класса main
	globalTable.push_back(makeTableEl((*constNumber)++,NULL,_UTF8,NULL,std::string("Main"),NULL));
	// Создаем для нее константу типа Class
	globalTable.push_back(makeTableEl((*constNumber)++,1,_CLASS,NULL,std::string("2"),NULL));
	// Добавляем в таблицу константу класса Value
	globalTable.push_back(makeTableEl((*constNumber)++,NULL,_UTF8,NULL,std::string("Value"),NULL));
	// Создаем для нее константу типа Class
	globalTable.push_back(makeTableEl((*constNumber)++,NULL,_CLASS,NULL,std::string("4"),NULL));
	ValNum=*constNumber-1;
	parseStmtListForTable(treeRoot,constNumber,NULL);
	std::vector<struct TableElement*>::iterator iter;  // Объявляем итератор для списка строк
	// Для каждого элемента списка и пока не найдено значение..
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
		if(!containsString(this->varNames,opName))
		{
			this->varNames.push_back(opName);
			// Делаем привязку к типу
			if(TypeNum==0&&typeAboveExpression!=_ARRINIT)
			{
				globalTable.push_back(makeTableEl((*constNum)++,NULL,_UTF8,NULL,std::string("LValue;"), local));
				TypeNum=*constNum-1;
			}
			else if(MassTypeNum==0&&typeAboveExpression==_ARRINIT)
			{
				globalTable.push_back(makeTableEl((*constNum)++,NULL,_UTF8,NULL,std::string("[LValue;"), local));
				MassTypeNum=*constNum-1;
			}
			// Добавляем в таблицу данные о переменной
			globalTable.push_back(makeTableEl((*constNum)++,expr->loc->firstLine,_UTF8,NULL,opName, local));
			// Делаем NameAndType
			char buf[50]="";
			if(typeAboveExpression==_ARRINIT)
				sprintf(buf,"%d,%d",*constNum-1,MassTypeNum);
			else
				sprintf(buf,"%d,%d",*constNum-1,TypeNum);
			globalTable.push_back(makeTableEl((*constNum)++,expr->loc->firstLine,_NAMEnTYPE,NULL,std::string(buf), local));
			// Делаем fieldRef
			buf[0]='\0';
			sprintf(buf,"%d,%d",ValNum,*constNum-1);
			globalTable.push_back(makeTableEl((*constNum)++,expr->loc->firstLine,_FIELDREF,NULL,std::string(buf), local));				
		}
	}

	// Если константа
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
		// Добавляем в таблицу данные о переменной
		globalTable.push_back(makeTableEl((*constNum)++,val->loc->firstLine,_STRING,NULL,std::string(str), local));
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
		globalTable.push_back(makeTableEl((*constNum)++,funcdefstmt->nameLoc->firstLine,_UTF8,1,type, local));
		// Добавляем в таблицу данные о переменной
		globalTable.push_back(makeTableEl((*constNum)++,funcdefstmt->nameLoc->firstLine,_UTF8,1,std::string(curHeader->functionName), local));
		// Делаем NameAndType
		char buf[50]="";
		sprintf(buf,"%d,%d",*constNum-2,*constNum-1);
		globalTable.push_back(makeTableEl((*constNum)++,funcdefstmt->nameLoc->firstLine,_NAMEnTYPE,1,std::string(buf), local));
		// Делаем methodRef
		buf[0]='\0';
		sprintf(buf,"%d,%d",ValNum,*constNum-1);
		globalTable.push_back(makeTableEl((*constNum)++,funcdefstmt->nameLoc->firstLine,_METHODREF,1,std::string(buf), local));
		local=*constNum-1;
		//currentFuncName=std::string(curHeader->functionName);

		std::vector<struct TableElement*> funcTable;
		int* funcConsts = new int;
		*funcConsts=1;
		// Проверяем ее тело
		parseStmtListForTable(funcdefstmt->body,constNum,local);//checkStatementList(funcdefstmt->body);
		//programm_table.insert(TablePair(std::string(curHeader->functionName),funcTable));
		//prog.push_back(TablePair(std::string(curHeader->functionName),funcTable));
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
		//currentFuncName="global.csv";
		local=NULL;
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
		// Считаем следующий элемент списка новым текущим
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
	// Если операнд
	if(expr->type==_OPERAND)
	{
		// Проверяем, что он уже объявлен
		std::string opName = std::string(expr->idName);
		if(!containsString(this->varNames,opName)&&!containsString(this->funcNames,opName))
		{
			// Если не объявлен, выдаем ошибку с именем операнда
			char* errstr=new char[35+strlen(expr->idName)];
			errstr[0]='\0';
			strcpy(errstr,"Can't use undefined operand - ");
			strcat(errstr,expr->idName);
			throw makeStringForException(errstr,expr->loc);
		}
	}
	// Если присвоение
	else if(expr->type==_ASSIGN)
	{
		// Если присваивание уже было, то кидаем исключение
		if(assign)
			throw makeStringForException("Assignment operation can not be more than 2 times in the expression.",expr->loc);
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
				if(containsString(this->funcNames,opName))
				{
					this->deleteString(this->funcNames,opName);
					std::vector<struct FunctionHeader*>::iterator iter;  // Объявляем итератор для списка функций
					// Для каждого элемента списка и пока не найдено значение..
					for(iter=this->funcHeaders.begin(); iter<this->funcHeaders.end(); iter++) 
					{
						// Проверяем, равна ли текущая функция нужной
						if(strcmp((*iter)->functionName,opName.c_str())==0)
						{
							// Удаляем нужное значение
							this->funcHeaders.erase(iter);
							// Выходим из цикла
							break;
						}
					}
				}
			}
			// Проверяем правую часть присвоения
			checkExpr(expr->right,true);
		}
		// Иначе если слева взятие по индексу, то меняем тип операции с _ASSIGN на _ARRID_AND_ASSIGN
		else if(expr->left->type==_ARRID)
		{
			// Проверяем правую часть
			checkExpr(expr->right, true);
			// Проверяем левую часть
			checkExpr(expr->left,true);
			
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
			throw makeStringForException("Can't assign value to anything except operand or array element.",expr->loc);
	}
	// Проверка на то, что бы во взятие элемента по индексу не было операции "="
	else if(expr->type==_ARRID)
	{
		// Проверяем левую часть
		checkExpr(expr->left,assign);
		// Проверяем правую часть
		if(exprContainsAssign(expr->right))
			throw makeStringForException("Can't use assignment operation in expression for array index.",expr->loc);

		checkExpr(expr->right,assign);
	}
	// Если действие - не, проверяем выражение
	else if(expr->type==_NOT)
	{
		checkExpr(expr->left,assign);
	}
	// Если действие над массивом - append/remove 
	else if(expr->type==_ARRACT)
	{
		// Проверяем, что бы действие было либо append либо remove
		if(!(strcmp(expr->idName,"append")==0 || strcmp(expr->idName,"remove")==0))
			throw makeStringForException("There is no such operation on the list.",expr->loc);
		// Проверяем левую часть
		checkExpr(expr->left,assign);
		// Проверяем правую часть
		if(exprContainsAssign(expr->right))
			throw makeStringForException("Assignment operation should not be an argument.",expr->loc);
		checkExpr(expr->right,assign);
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
			checkExpr(begining,assign);
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
		for(iter=this->funcHeaders.begin(); iter<this->funcHeaders.end()&&!contains; iter++) 
		{
			// Проверяем, равна ли текущая функция нужной
			contains=strcmp(name.c_str(),(*iter)->functionName)==0;
			if(contains)
				break;
		}
		// Если функция с таким именем объявлена..
		if(contains)
		{
			// Создаем указатель на элемент списка аргументов объявления функции
			struct DefFuncParamInfo* el = (*iter)->params->first;
			// Счетчик количества аргументов функции
			int count = 0;
			// Счетчик количества аргументов функции по умолчанию
			int count_of_def = 0;
			std::vector<std::string> def_defaults;
			// Пока не пройдены все параметры
			// Считаем количество обычных аргументов и по умолчанию
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
			// Создаем указатель на элемент списка аргументов вызова функции
			struct ExprInfo* call_el = expr->arglist->first;
			// Счетчик фактического количества аргументов
			int fact_count = 0;
			// Счетчик фактических параметров функции
			int fact_count_def = 0;
			std::vector<std::string> defaults;
			// Для каждого фактического аргумента
			while(call_el!=NULL)
			{
				// Если тип - присвоение
				if(call_el->type==_ASSIGN)
				{
					// Увеличиваем счетчик фактических параметров
					fact_count_def++;
					// Если слева - операнд, то прояеряем его имя
					if(call_el->left->type==_OPERAND)
					{
						// Проверить, что имя аргумента содержится в списке параметров функции
						if(!containsString(def_defaults,std::string(call_el->left->idName)))
							throw makeStringForException("Can't assign value to param that isn't default",call_el->loc); 
					}
					// Иначе выбрасываем исключение
					else
						throw makeStringForException("Can't assign value to anything except operand",call_el->loc); 
					if(exprContainsAssign(call_el->right))
						throw makeStringForException("Can't use multiple assignment",call_el->loc); 
					checkExpr(call_el->right,false);
				}
				// Иначе..
				else
				{
					//Увеличиваем счетчик фактических аргументов
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
			// Если количество аргументов совпадает
			if(count==fact_count)
			{
				// Если количество параметров по умолчанию в объявлении меньше, чем при вызове
				if(count_of_def<fact_count_def)
					throw makeStringForException("Wrong amount of func parametrs in call",expr->loc);					
			}
			else
				throw makeStringForException("Wrong amount of params for function call",expr->loc); // Выбросить исключение
		}
		else
			throw makeStringForException("Can't call undefined function",expr->loc); // Выбросить исключение
	}
	else if(expr->type==_UMINUS)
	{
		// Унарный минус может быть только перед целым числом или выражением, возвращающим целое число
		// Проверим первое
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
	/*Проверка иф_стмт на корректность*/
	if(exprContainsAssign(ifstmt->expr))
		throw makeStringForException("Assignment operation must not be in a conditional expression.",ifstmt->expr->loc);
	checkExpr(ifstmt->expr,false);//проверка условного выражения
	checkStatementList(ifstmt->stmtlist);//проверка тела if-а
	if(ifstmt->elsestmtlist!=NULL)
		checkStatementList(ifstmt->elsestmtlist);//проверка тела else
	// Проверка elif списка
	if(ifstmt->eliflist!=NULL)
	{
		struct ElifListInfo* begin=ifstmt->eliflist;
		while(begin!=NULL)
		{
			// Проверка условного выражения
			checkExpr(begin->expr,false);
			// Проверка тела
			checkStatementList(begin->stmtlist);
			begin=begin->next;
		}
	}

	/*Если все корректно, то преобразуем иф_стмт с elif-фами в иф_стмт без elif-ов*/
	struct StmtListInfo* lastElseMainIf=ifstmt->elsestmtlist;//сохраняем последний else блок(он будет самым нижним в дереве)
	if(ifstmt->eliflist!=NULL)
	{
		struct IfStmtInfo* currentIf = ifstmt;
		struct ElifListInfo* elifListElement=ifstmt->eliflist;
		while(elifListElement!=NULL)
		{
			// Создаем новый иф
			struct IfStmtInfo * newIf = (struct IfStmtInfo *)malloc(sizeof(struct IfStmtInfo));
			newIf->expr=elifListElement->expr;
			newIf->stmtlist=elifListElement->stmtlist;
			// Создать новый иф стмт
			struct StmtInfo* newIfStmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
			newIfStmt->type=_IF;
			newIfStmt->ifstmt=newIf;
			newIfStmt->next=NULL;
			// Создать новый стмт лист
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
	// Проверка установка флагов и состояний при входе в цикл
	bool cycle=false;
	if(lc_state==_REGULAR_STATE)
	{
		lc_state = _CYCLE_STATE;
		cycle=true;
	}
	
	// Код функции
	if(exprContainsAssign(whilestmt->expr))
		throw makeStringForException("Assignment operation must not be in a conditional expression.",whilestmt->expr->loc);
	checkExpr(whilestmt->expr,false);//проверка условного выражения продолжения цикла
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
	if(exprContainsAssign(forstmt->expr))
		throw makeStringForException("Assignment operation must not be in a conditional expression.",forstmt->expr->loc);
	checkExpr(forstmt->expr,false);//проверка выражения, по чем проходит цикл
	checkStatementList(forstmt->stmtlist);//проверка тела цикла
	if(forstmt->elsestmt!=NULL)
		checkStatementList(forstmt->elsestmt);//проверка блока else цикла

	// Установка состояний при выходе из цикла
	if(cycle==true)
	{
		lc_state = _REGULAR_STATE;
	}
}

void TreeTraversal::checkFuncDefStmt(struct FuncDefInfo * funcdefstmt) throw(char*)
{
	// Проверяем текущее состояние анализатора
	enum GlobalState lastState = gl_state;
	// Если вызов функции из глобального кода, меняем состояние
	if(lastState == _MAIN_STATE)
		gl_state = _FUNC_STATE;
	// Код функции
	// Проверяем параметры функции
	checkFuncParams(funcdefstmt->params);
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
		if(containsString(this->varNames,curHeader->functionName))
			deleteString(this->varNames,curHeader->functionName);
		this->funcNames.push_back(curHeader->functionName);
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
		// Если не объявлен, выдаем ошибку с именем операнда
		char* errStr = new char[30+strlen(curHeader->functionName)];
		strcpy(errStr,"Can't define same function: ");
		strcat(errStr,curHeader->functionName);
		throw makeStringForException(errStr,funcdefstmt->nameLoc);
	}
	// Если был вызов функции из глобального кода, меняем состояние
	if(lastState == _MAIN_STATE)
		gl_state = _MAIN_STATE;
}

void TreeTraversal::checkContinueBreakStmt(struct StmtInfo* contBreakStmt) throw(char*)
{
	// Если мы сейчас находимся не в цикле, бросаем исключение
	if(this->lc_state == _REGULAR_STATE)
	{
		// Соединяем все в строке сообщения
		char* errStr = new char[31];
		if(contBreakStmt->type==_BREAK)
			strcpy(errStr,"Found break in global code.");
		else if(contBreakStmt->type==_CONTINUE)
			strcpy(errStr,"Found continue in global code.");
		// Бросаем строку исключением
		throw makeStringForException(errStr,contBreakStmt->loc);
	}
}

void TreeTraversal::checkReturnStmt(struct StmtInfo* retStmt, struct ExprInfo * expr) throw(char*)
{
	// Если мы сейчас находимся в главном коде программы, бросаем исключение
	if(this->gl_state == _MAIN_STATE)
		throw makeStringForException("Found return in global code.",retStmt->loc);
	// Иначе проверяем возвращаемое выражение
	checkExpr(expr,false);
}

void TreeTraversal::checkDelStmt(struct ExprInfo * expr) throw(char*)
{
	// Из правил бизона известно, что выражением может быть только операнд
	// Поэтому проверяем, не удален ли уже операнд
	checkExpr(expr,false);
	if(containsString(this->varNames,std::string(expr->idName)))
		deleteString(this->varNames,std::string(expr->idName));
	if(containsString(this->funcNames,std::string(expr->idName)))
	{
		deleteString(this->funcNames,std::string(expr->idName));
		std::vector<struct FunctionHeader*>::iterator iter;  // Объявляем итератор для списка функций
		// Для каждого элемента списка и пока не найдено значение..
		for(iter=this->funcHeaders.begin(); iter<this->funcHeaders.end(); iter++) 
		{
			// Проверяем, равна ли текущая функция нужной
			if(strcmp((*iter)->functionName,expr->idName)==0)
			{
				// Удаляем нужное значение
				this->funcHeaders.erase(iter);
				// Выходим из цикла
				break;
			}
		}
	}
}

void TreeTraversal::checkFuncParams(struct DefFuncParamListInfo* params)
{
	// Проверка параметров функции при объявлении заключается лишь в том,
	// что аргументы по умолчанию должны быть указаны после всех аргументов без значения
	// Если у функции нет аргументов, то проверка не нужна
	// Иначе
	if(params!=NULL)
	{
		// Объявляем элемент для обхода списков
		struct DefFuncParamInfo* element = params->first;
		// Для каждого аргумента функции
		while(element!=NULL)
		{
			// Если есть следующий элемент..
			if(element->next!=NULL)
			{
				// Если текущий элемент - параметр по умолчанию, а следующий - простой операнд
				// То выбрасываем исключение
				if(element->paramVal!=NULL&&element->next->paramVal==NULL)
					throw makeStringForException("Wrong order of function params",element->loc);
			}
			// Переходим к следующему элементу
			element=element->next;
		}
	}
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