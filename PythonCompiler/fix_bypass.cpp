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

// Проверка дерева (первый обход)
void TreeTraversal::fixTree(struct StmtListInfo* root) throw(char*)
{
	if(root == NULL)
		throw makeStringForException("Root of tree is a null pointer\n",NULL);
	// Задаем начальные состояния
	this->gl_state=_MAIN_STATE;
	this->lc_state=_REGULAR_STATE;
	currentFuncName="global";
	this->varDecls["global"].push_back("");
	this->varDecls["global"].clear();
	// Запускаем проверку дерева
	checkStatementList(root);
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

void TreeTraversal::checkExpr(struct ExprInfo * expr, bool assign) throw(char*)
{
	// Если операнд (использование)
	if(expr->type==_OPERAND)
	{
		// Получаем имя операнда
		std::string opName = std::string(expr->idName);
		// Проверяем, что имя операнда не совпадает с именем библиотечной функции
		if(containsString(this->libFuncs,opName))
		{
			// Если не объявлен, выдаем ошибку с именем операнда
			char* errstr=new char[40+strlen(expr->idName)];
			errstr[0]='\0';
			strcpy(errstr,"Can't name operand as lib function - ");
			strcat(errstr,expr->idName);
			throw makeStringForException(errstr,expr->loc);
		}
		bool operandExists=false;
		// Проверяем, что он уже объявлен
		// Если операнд встретился в глобальном коде
		// Проверяем только его наличие в списке объявлений по ключу global
		if(this->gl_state==_MAIN_STATE)
			operandExists=containsString(this->varDecls.at("global"),opName);
		// Иначе если операнд встретился внутри функции
		else
		{
			// 1. Проверяем его наличие в списке объявлений данной локальной функции
			operandExists=containsString(this->varDecls.at(currentFuncName),opName);
			// 2. Если переменная не локальная, то ищем ее в глобальном коде..
			if(!operandExists)
			{
				operandExists=containsString(this->varDecls.at("global"),opName);
				// Если найдено использование глобальной переменной, то добавляем ее имя в usage
				if(operandExists)
					if(!containsString(varUsage[currentFuncName],opName))
						varUsage[currentFuncName].push_back(opName);
			}
		}
		// 3. Если переменная не найдена, выбрасываем исключение
		if(!operandExists)
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
			throw makeStringForException("Can't use assignment in the expression.",expr->loc);//("Assignment operation can not be more than 2 times in the expression.",expr->loc);
		// Слева от присовения может быть только либо операнд, либо взятие по индексу массива
		// Если слева операнд
		if(expr->left->type==_OPERAND)
		{
			std::string opName = std::string(expr->left->idName);
			// Проверяем, что имя операнда не совпадает с именем библиотечной функции
			if(containsString(this->libFuncs,opName))
			{
				// Если не объявлен, выдаем ошибку с именем операнда
				char* errstr=new char[40+strlen(expr->idName)];
				errstr[0]='\0';
				strcpy(errstr,"Can't name operand as lib function - ");
				strcat(errstr,expr->idName);
				throw makeStringForException(errstr,expr->loc);
			}
			// 1. Если код глобальный..
			if(this->gl_state==_MAIN_STATE)
			{
				// 1.1. Проверяем, объявлена ли уже такая переменная
				// 1.2. Если переменная не объявлена, то добавляем информацию о ее объявлении
				if(!containsString(this->varDecls.at("global"),opName))
					this->varDecls["global"].push_back(opName);
				// Удаляем функцию
				if(containsString(this->funcNames,opName))
				{
					deleteString(this->funcNames,opName);
					deleteFuncHeader(this->funcHeaders,opName);
				}
			}
			// 2. Если код локальный..
			else
			{
				// 2.1. Проверяем, есть ли локальная переменная с таким именем
				// 2.2. Если переменной с таким именем нет
				if(!containsString(this->varDecls.at(currentFuncName),opName))
				{
					// 2.2.1. Проверяем, есть ли глобальная переменная с таким именем
					// 2.2.2. Если такая переменная есть в глобальном коде, то проверяем, использовалась ли она до этого в этой функции
					if(containsString(this->varDecls.at("global"),opName))
					{
						// 2.2.2.1. Если переменная использовалась, выдаем ошибку
						if(containsString(this->varUsage.at(currentFuncName),opName))
						{
							// Если не объявлен, выдаем ошибку с именем операнда
							char* errstr=new char[35+strlen(opName.c_str())];
							errstr[0]='\0';
							strcpy(errstr,"Variable referenced before assignment - ");
							strcat(errstr,opName.c_str());
							throw makeStringForException(errstr,expr->loc);
						}
						else
							this->varDecls[currentFuncName].push_back(opName);
						// 2.2.2.2. Иначе добавляем переменную в список локальных переменных
					}
					else
						this->varDecls[currentFuncName].push_back(opName);
					// Удаляем функцию
					if(containsString(this->funcNames,opName))
					{
						deleteString(this->funcNames,opName);
						deleteFuncHeader(this->funcHeaders,opName);
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
		// Проверка на присваивание внутри отрицания(питон так не разрешает)
		if(exprContainsAssign(expr->left))
			throw makeStringForException("Can't use assignment operation in NOT expression.",expr->loc);
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
			// Проверка на присваивание внутри выражения
			if(exprContainsAssign(begining))
				throw makeStringForException("Can't use assignment operation in expression for array initialize.",expr->loc);
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
		if(!isLibFunctionCall(expr))
		{
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
	// 1. Если код глобальный..
	if(this->gl_state==_MAIN_STATE)
	{
		// 1.1. Проверяем, объявлена ли уже такая переменная
		// 1.2. Если переменная не объявлена, то добавляем информацию о ее объявлении
		if(!containsString(this->varDecls.at("global"),opName))
			this->varDecls.at("global").push_back(opName);
		// Удаляем функцию
		if(containsString(this->funcNames,opName))
		{
			deleteString(this->funcNames,opName);
			deleteFuncHeader(this->funcHeaders,opName);
		}
	}
	// 2. Если код локальный..
	else
	{
		// 2.1. Проверяем, есть ли локальная переменная с таким именем
		// 2.2. Если переменной с таким именем нет
		if(!containsString(this->varDecls.at(currentFuncName),opName))
		{
			// 2.2.1. Проверяем, есть ли глобальная переменная с таким именем
			// 2.2.2. Если такая переменная есть в глобальном коде, то проверяем, использовалась ли она до этого в этой функции
			if(containsString(this->varDecls.at("global"),opName))
			{
				// 2.2.2.1. Если переменная использовалась, выдаем ошибку
				if(containsString(this->varUsage.at(currentFuncName),opName))
				{
					// Если не объявлен, выдаем ошибку с именем операнда
					char* errstr=new char[35+strlen(opName.c_str())];
					errstr[0]='\0';
					strcpy(errstr,"Variable referenced before assignment - ");
					strcat(errstr,opName.c_str());
					throw makeStringForException(errstr,forstmt->expr->loc);
				}
				else
					this->varDecls.at(currentFuncName).push_back(opName);
				// 2.2.2.2. Иначе добавляем переменную в список локальных переменных
			}
			else
				this->varDecls.at(currentFuncName).push_back(opName);
			// Удаляем функцию
			if(containsString(this->funcNames,opName))
			{
				deleteString(this->funcNames,opName);
				deleteFuncHeader(this->funcHeaders,opName);
			}
		}
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
	// Если объявление функции из глобального кода, меняем состояние
	if(lastState == _MAIN_STATE)
		gl_state = _FUNC_STATE;
	// Иначе выдаем ошибку
	else if(lastState == _FUNC_STATE)
		throw makeStringForException("Can't define function inside function",funcdefstmt->nameLoc);
	// Проверяем, что имя операнда не совпадает с именем библиотечной функции
	// Код функции
	currentFuncName=std::string(funcdefstmt->functionName);
	if(containsString(this->libFuncs,currentFuncName))
	{
		// Если не объявлен, выдаем ошибку с именем операнда
		char* errstr=new char[50+strlen(currentFuncName.c_str())];
		errstr[0]='\0';
		strcpy(errstr,"Can't define function with lib function name - ");
		strcat(errstr,currentFuncName.c_str());
		throw makeStringForException(errstr,funcdefstmt->nameLoc);
	}
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
	if(!containsFuncHeader(this->funcHeaders,curHeader)&&!containsString(this->funcNames,std::string(curHeader->functionName)))
	{
		// Добавляем ее заголовок в массив функций
		this->funcHeaders.push_back(curHeader);
		// Удаляем переменную с таким именем
		deleteString(this->varDecls.at("global"),curHeader->functionName);
		this->funcNames.push_back(curHeader->functionName);
		// Добавляем аргументы функции в список констант
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
		// Проверяем ее тело
		checkStatementList(funcdefstmt->body);
		if(!hasReturn)
		{
			// Создать узел return с экспром из варВала None и поставить его в конец стейтментов тела функции
			struct ValInfo* ret = createValNode(_NONE,NULL,NULL,NULL,funcdefstmt->body->last->loc);
			struct ExprInfo* retExpr = createSimpleExpr(_VARVAL, NULL, ret, ret->loc);
			struct StmtInfo* retStmtm = createFromReturnStatement(_RETURN,retExpr,retExpr->loc);
			struct StmtInfo* lastInFunc = funcdefstmt->body->last;
			funcdefstmt->body->last=retStmtm;
			lastInFunc->next=funcdefstmt->body->last;
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
	hasReturn=true;
}

void TreeTraversal::checkDelStmt(struct ExprInfo * expr) throw(char*)
{
	// Из правил бизона известно, что выражением может быть только операнд
	// Поэтому проверяем, не удален ли уже операнд
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