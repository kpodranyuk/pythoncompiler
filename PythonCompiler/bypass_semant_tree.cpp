#include "bypass_semant_tree.h"

// Конструктор по умолчанию
TreeTraversal::TreeTraversal()
{
}

bool TreeTraversal::containsString(std::vector<std::string>& vec, std::string str) const
{
	std::vector<std::string>::const_iterator iter;  // Объявляем итератор для списка строк
	bool contains = false;							// Объявляем флаг того, что строка найдена в списке
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
	std::vector<std::string>::const_iterator iter;  // Объявляем итератор для списка строк
	// Для каждого элемента списка и пока не найдено значение..
	for(iter=vec.begin(); iter<vec.end(); iter++) 
	{
		// Проверяем, равна ли текущая строка нужной
		if((*iter).compare(str)==0)
		{
			vec.erase(iter);
			break;
		}
	}
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

// Составление таблиц (второй обход)
void TreeTraversal::makeTables(const struct StmtListInfo* treeRoot)
{
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
			checkReturnStmt(begining->expr);
		}
		else if(begining->type==_BREAK || begining->type==_CONTINUE)
		{
			//printContinueBreakStmt(_BREAK,nodeCount,dotTree);
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
			// Если не объявлен, выдаем ошибку с именем операнда
			char* errstr=new char[35+strlen(expr->idName)];
			errstr[0]='\0';
			strcpy(errstr,"Can't use undefined operand - ");
			strcat(errstr,expr->idName);
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
		// Иначе если слева взятие по индексу
		else if(expr->left->type==_ARRID)
		{
			// Проверяем, что взятие производится уже по объявленному массиву
			struct ExprInfo* arr = expr->left;
			if(arr->left->type==_OPERAND)
				checkExpr(arr->left);
			// Иначе если взятие не относится к операнду, выбрасываем исключение
			else
				throw "Can't use [brackets] for anything except arrays.";
		}
		// Иначе бросаем исключение, что нельзя присвоить значение чему-то кроме операнда или элемента массива
		else
			throw "Can't assign value to anything except operand or array element.";
	}
	// Если действие - не, проверяем выражение
	else if(expr->type==_NOT)
	{
		checkExpr(expr->left);
	}
	// Если действие над массивом - append/remove 
	else if(expr->type==_ARRACT)
	{
		// Проверяем, чтобы массив был операндом и был в списке переменных
		if(expr->left->type==_OPERAND)
			checkExpr(expr->left);
		else
			throw "Can't use methods to anything except arrays.";
	}
	else if(expr->type==_FUNCCALL)
	{
		;
		// TODO Придумать функцию для сверки вызова и объявления 
	}
	else
	{
		if(expr->type!=_VARVAL)
		{
			checkExpr(expr->left);
			checkExpr(expr->right);
		}
	}
	//TODO Разобраться с реализацией присвоения элементу массива
	/*else if(expr->type==_ARRID_AND_ASSIGN)
	{
	}*/
}

void TreeTraversal::checkIfStmt(struct IfStmtInfo * ifstmt)
{
}

void TreeTraversal::checkWhileStmt(struct WhileStmtInfo * whilestmt)
{
	lc_state = _CYCLE_STATE;
	// Код функции
	lc_state = _REGULAR_STATE;
}

void TreeTraversal::checkForStmt(struct ForStmtInfo * forstmt)
{
	lc_state = _CYCLE_STATE;
	// Код функции
	lc_state = _REGULAR_STATE;
}

void TreeTraversal::checkFuncDefStmt(struct FuncDefInfo * funcdefstmt)
{
	enum GlobalState lastState = gl_state;
	if(lastState == _MAIN_STATE)
		gl_state = _FUNC_STATE;
	// Код функции
	if(lastState == _MAIN_STATE)
		gl_state = _MAIN_STATE;
}

void TreeTraversal::checkContinueBreakStmt(enum StmtType type)
{
}

void TreeTraversal::checkReturnStmt(struct ExprInfo * expr) throw(char*)
{
	// Если мы сейчас находимся в главном коде программы, бросаем исключение
	if(this->gl_state == _MAIN_STATE)
		throw "Found return in global code.";
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