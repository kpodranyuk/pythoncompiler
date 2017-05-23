#include "struct_functions.h"

#pragma warning(disable : 4996)

/* Создание позиции элемента кода программы
* @author Kate
* \param[in] FL Строка начала элемента
* \param[in] FC Столбец начала элемента
* \param[in] LL Строка конца элемента
* \param[in] LC Столбец конца элемента
* \return указатель на структуру позиции элемента кода программы
*/
struct CodeLocation* createCodeLocation(int FL, int FC, int LL, int LC)
{
	struct CodeLocation* loc=(struct CodeLocation*)malloc(sizeof(struct CodeLocation));
	loc->firstLine=FL;
	loc->firstColumn=FC;
	loc->lastLine=LL;
	loc->lastColumn=LC;
	return loc;
}

/* Создание листа выражений
* @author Kate
* \param[in] expr указатель на добавляемое выражение
* \param[in] exprlist указатель на лист
* \return указатель на лист
*/
struct ExprListInfo * createExprList(struct ExprInfo * expr, struct ExprListInfo * exprlist)
{
	// Если список из пустых аргументов 
	if(exprlist==NULL&&expr==NULL)
	{
		// Создаем пустой список и возвращаем его
		struct ExprListInfo * newExprList=(struct ExprListInfo*)malloc(sizeof(struct ExprListInfo));
		newExprList->first=NULL;
		newExprList->last=NULL;
		return newExprList;
	}
	// Если ничего нового в узел не добавляется
	if(expr==NULL)
	{
		return exprlist;
	}
	// Считаем, что следующего элемента у выражения нет
	expr->next=NULL;
	// Если входного списка нет
	if(exprlist==NULL)
	{
		// Создаем новый список, единственным элементом считаем входной
		struct ExprListInfo * newExprList=(struct ExprListInfo*)malloc(sizeof(struct ExprListInfo));
		newExprList->first=expr;
		newExprList->last=expr;
		// Возвращаем его
		return newExprList;
	}
	// Иначе связываем входной список со входным элементом
	exprlist->last->next=expr;
	exprlist->last=expr;
	// Возвращаем обновленный входной список
	return exprlist;
}

/* Создание простого узла из выражения (операнда или значения)
* @author Kate
* \param[in] type тип выражения
* \param[in] opName имя операнда
* \param[in] val значение операнда
* \return указатель на узел
*/
struct ExprInfo* createSimpleExpr(enum ExprType type, char* opName, struct ValInfo* val, struct CodeLocation* loc)
{
	// Выделяем память под узел
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));
	// Присваиваем тип
	expr->type=type;
	// Если передано имя операнда..
	if(opName!=NULL)
	{
		// Выделяем память в поле под имя операнда и перекопируем
		expr->idName = (char*)malloc(sizeof(strlen(opName)+1));
		strcpy(expr->idName, opName);
	}
	// Иначе присваиваем NULL
	else
		expr->idName=NULL;
	// Присваиваем переданное значение операнда
	expr->exprVal=val;

	expr->loc=loc;
	expr->left=NULL;
	expr->right=NULL;
	expr->middle=NULL;

	// Возвращаем созданный узел
	return expr;
}

/* Создание узла выражения из выражений
* @author Kate
* \param[in] type тип выражения
* \param[in] left левое выражение
* \param[in] right правое выражение
* \return указатель на новый узел
*/
struct ExprInfo* createExprInfo(enum ExprType type, struct ExprInfo* left, struct ExprInfo* right, struct CodeLocation* operationLoc)
{
	// Выделяем память под новый узел-выражение
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));
	// Присваиваем тип
	expr->type=type;
	// Считаем, что у узла есть только левый и правый дочерний узлы
	expr->arglist=NULL;
	expr->exprVal=NULL;
	expr->idName=NULL;
	expr->left=left;
	expr->right=right;
	expr->loc=operationLoc;
	expr->middle=NULL;
	// Возвращаем новый созданный узел
	return expr;
}

/* Создание узла выражения инициализации массива
* \param[in] type тип выражения
* \param[in] exprlist лист выражений
* \return указатель на новый узел
*/
struct ExprInfo* createInitListInfo(enum ExprType type, struct ExprListInfo * exprlist, struct CodeLocation* bracket)
{
	// Выделяем память под новый узел-выражение
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));
	// Присваиваем тип
	expr->type=type;
	// Считаем, что у узла есть только список экспрешенов
	expr->arglist=exprlist;
	expr->exprVal=NULL;
	expr->idName=NULL;
	expr->left=NULL;
	expr->right=NULL;
	expr->loc=bracket;
	// Возвращаем новый созданный узел
	return expr;
}

/* Создание узла выражения действия над массивом
* \param[in] type тип выражения
* \param[in] left левое выражение(к чему применяем)
* \param[in] right правое выражение
* \param[in] actName имя применяемой операции к массиву
* \return указатель на новый узел
*/
struct ExprInfo* createActListInfo(enum ExprType type, struct ExprInfo* left, struct ExprInfo* right, char * actName, struct CodeLocation* dotPos)
{
	// Выделяем память под новый узел-выражение
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));
	// Присваиваем тип
	expr->type=type;
	expr->idName = (char*)malloc(sizeof(strlen(actName)+1));
	strcpy(expr->idName, actName);
	expr->left=left;
	expr->right=right;
	expr->arglist=NULL;
	expr->exprVal=NULL;
	expr->loc=dotPos;
	// Возвращаем новый созданный узел
	return expr;
}

/* Создание узла выражения из вызова функции
* @author Kate
* \param[in] type тип выражения
* \param[in] funcName имя функции
* \param[in] args список аргументов
* \return указатель на новый узел
*/
struct ExprInfo* createExprInfoFromFuncCall(enum ExprType type, char* funcName, struct ExprListInfo* args, struct CodeLocation* callL)
{
	// Выделяем память под узел
	struct ExprInfo* expr = (struct ExprInfo*)malloc(sizeof(struct ExprInfo));
	// Переприсваиваем тип и аргументы
	expr->type=type;
	expr->arglist=args;
	// Выделяем память под имя функции
	expr->idName=(char*)malloc(sizeof(strlen(funcName)+1));
	strcpy(expr->idName,funcName);
	// Считаем остальное отсутствующим
	expr->exprVal=NULL;
	expr->left=NULL;
	expr->right=NULL;
	expr->next=NULL;
	expr->loc=callL;
	// Возвращаем узел
	return expr;
}

/* Создание узла из типа выражения
* @author Kate
* \param[in] type тип значения 
* \param[in] logVal логическое значение 
* \param[in] stringVal строковое значение 
* \param[in] numVal целочисленное значение 
*/
struct ValInfo* createValNode(enum ValType type, bool logVal, char* stringVal, int numVal, struct CodeLocation* loc)
{
	// Выделяем память под узел
	struct ValInfo* val = (struct ValInfo*)malloc(sizeof(struct ValInfo));
	// Переприсваиваем тип и логическое значение
	val->type = type;
	val->logVal = logVal;
	// Если задано строковое значение
	/*if(stringVal!=NULL)
	{
		// Выделяем память и перекопируем значение
		val->stringVal = (char*)malloc(sizeof(strlen(stringVal)+1));
		strcpy(val->stringVal, stringVal);
	}
	// Иначе переприсваиваем
	else*/
		val->stringVal = stringVal;
	// Переприсваиваем целочисленное значение
	val->intVal = numVal;
	val->loc=loc;
	// Возвращаем созданный узел
	return val;
}


/* Создание списка параметров функции
* @author Kate
* \param[in] newParamName новый параметр функции
* \param[in] params список параметров
* \return указатель на новый список
*/
struct DefFuncParamListInfo* createDefFuncParamListInfo(char* newParamName, struct ValInfo* newParamVal, struct DefFuncParamListInfo* params, struct CodeLocation* loc)
{
	// Если список из пустых аргументов 
	if(newParamName==NULL&&params==NULL&&newParamVal==NULL)
	{
		// Создаем пустой список и возвращаем его
		struct DefFuncParamListInfo * newParamList=(struct DefFuncParamListInfo*)malloc(sizeof(struct DefFuncParamListInfo));
		newParamList->first=NULL;
		newParamList->last=NULL;
		return newParamList;
	}
	// Если ничего нового в узел не добавляется
	if(newParamName==NULL)
	{
		return params;
	}
	// Создаем новый узел-элемент
	struct DefFuncParamInfo* newParam = (struct DefFuncParamInfo*)malloc(sizeof(struct DefFuncParamInfo));
	newParam->paramName = (char*)malloc(strlen(newParamName)+1);
	strcpy(newParam->paramName,newParamName);
	// Считаем, что следующего элемента у выражения нет
	newParam->next=NULL;
	newParam->paramVal=newParamVal;
	newParam->loc=loc;
	// Если входного списка нет
	if(params==NULL)
	{
		// Создаем новый список, единственным элементом считаем входной
		struct DefFuncParamListInfo * newParamList=(struct DefFuncParamListInfo*)malloc(sizeof(struct DefFuncParamListInfo));
		newParamList->first=newParam;
		newParamList->last=newParam;
		// Возвращаем его
		return newParamList;
	}
	// Иначе связываем входной список со входным элементом
	params->last->next=newParam;
	params->last=newParam;
	// Возвращаем обновленный входной список
	return params;
}

/* Создание узла объявления функции
* @author Kate
* \param[in] funcName имя функции
* \param[in] params параметры функции
* \param[in] funcBody тело функции
* \return указатель на узел объявления функции
*/
struct FuncDefInfo* createFuncDef(char* funcName, struct DefFuncParamListInfo* params, struct StmtListInfo* funcBody, struct CodeLocation* nameL)
{
	if(funcName==NULL||funcBody==NULL) 
		return NULL;
	struct FuncDefInfo* func = (struct FuncDefInfo*)malloc(sizeof(struct FuncDefInfo));
	func->functionName = (char*)malloc(strlen(funcName)+1);
	strcpy(func->functionName,funcName);
	func->params=params;
	func->body=funcBody;
	func->nameLoc=nameL;
	return func;
}

/* Создание стейтмента для определения функции
* @author Kate
* \param[in] type тип стейтмента
* \param[in] funcDef указатель на объявление функции
* \return указатель на стейтмент
*/
struct StmtInfo* createFromFuncDefStatement(enum StmtType type, struct FuncDefInfo* funcDef)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->funcdefstmt=funcDef;
	stmt->next=NULL;
	return stmt;
}

/* Создание стейтмент листа
* \param[in] stmt указатель на добавляемый стейтмент
* \param[in] stmtlist указатель на стейтмент лист
* \return указатель на стейтмент лист
*/
struct StmtListInfo * createStatementList(struct StmtInfo * stmt, struct StmtListInfo * stmtlist)
{
	//если ничего нового в узел не добавляется(stmt_list и после него идет line_sep, то возвращаем тот же  stmtlist)
	if(stmt==NULL)
	{
		return stmtlist;
	}

	stmt->next=NULL;
	//если новый узел
	if(stmtlist==NULL)
	{
		//создаем новый узел
		struct StmtListInfo * newStmtList=(struct StmtListInfo*)malloc(sizeof(struct StmtListInfo));
		newStmtList->first=stmt;
		newStmtList->last=stmt;
		return newStmtList;
	}

	stmtlist->last->next=stmt;
	stmtlist->last=stmt;
	return stmtlist;
}

/* Создание стейтмента для выражения
* \param[in] type тип стейтмента
* \param[in] ifstmt указатель на if стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createFromExprStatement(enum StmtType type, struct ExprInfo * exprstmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->expr=exprstmt;
	stmt->next=NULL;
	return stmt;
}

/* Создание стейтмента для развилки
* \param[in] type тип стейтмента
* \param[in] ifstmt указатель на if стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createFromIfStatement(enum StmtType type, struct IfStmtInfo * ifstmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->ifstmt=ifstmt;
	stmt->next=NULL;
	return stmt;
}

/* Создание стейтмента для цикла for
* \param[in] type тип стейтмента
* \param[in] forstmt указатель на for стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createFromForStatement(enum StmtType type, struct ForStmtInfo * forstmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->forstmt=forstmt;
	stmt->next=NULL;
	return stmt;
}

/* Создание стейтмента для цикла while
* \param[in] type тип стейтмента
* \param[in] whilestmt указатель на while стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createFromWhileStatement(enum StmtType type, struct WhileStmtInfo * whilestmt)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->whilestmt=whilestmt;
	stmt->next=NULL;
	return stmt;
}

/* Создание стейтмента для continue, break
* \param[in] type тип стейтмента
* \return указатель на стейтмент
*/
struct StmtInfo * createFromContinueBreakStatement(enum StmtType type, struct CodeLocation* loc)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->next=NULL;
	stmt->loc=loc;
	return stmt;
}

/* Создание стейтмента для return
* \param[in] type тип стейтмента
* \param[in] expr возвращаемое выражение
* \return указатель на стейтмент
*/
struct StmtInfo * createFromReturnStatement(enum StmtType type, struct ExprInfo * expr, struct CodeLocation* loc)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->expr=expr;
	stmt->next=NULL;
	stmt->loc=loc;
	return stmt;
}

/* Создание стейтмента для del
* \param[in] type тип стейтмента
* \param[in] operand очищаемый операнд
* \return указатель на стейтмент
*/
struct StmtInfo * createFromDelStatement(enum StmtType type, char * operand, struct CodeLocation* delLoc, struct CodeLocation* opLoc)
{
	struct StmtInfo * stmt = (struct StmtInfo *)malloc(sizeof(struct StmtInfo));
	stmt->type=type;
	stmt->expr=createSimpleExpr(_OPERAND, operand, NULL,opLoc);
	stmt->next=NULL;
	stmt->loc=delLoc;
	return stmt;
}

/* Создание if
* \param[in] expr условное выражение
* \param[in] stmtlist тело
* \param[in] eliflist список elif
* \param[in] elsestmtlist тело блока else
* \return указатель на if стейтмент
*/
struct IfStmtInfo * createIfStatement(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct ElifListInfo * eliflist, struct StmtListInfo * elsestmtlist)
{
	struct IfStmtInfo * ifstmt = (struct IfStmtInfo *)malloc(sizeof(struct IfStmtInfo));
	ifstmt->expr=expr;
	ifstmt->stmtlist=stmtlist;
	ifstmt->eliflist=eliflist;
	ifstmt->elsestmtlist=elsestmtlist;
	return ifstmt;
}

/* Создание elif листа
* \param[in] expr условное выражение
* \param[in] stmtlist тело
* \param[in] eliflist указатель на список elif-ов
* \return указатель на список
*/
struct ElifListInfo * createElifList(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct ElifListInfo * eliflist)
{
	struct ElifListInfo * elif = (struct ElifListInfo *)malloc(sizeof(struct ElifListInfo));
	elif->expr=expr;
	elif->stmtlist=stmtlist;
	elif->next=NULL;

	if(eliflist==NULL)
	{
		return elif;
	}
	else 
	{
		eliflist->next=elif;
		return eliflist;
	}
}


/* Создание for
* \param[in] counter счетчик цикла
* \param[in] expr выражение по чем проходит цикл
* \param[in] stmtlist тело цикла
* \param[in] elsestmt тело else после цикла
* \return указатель for
*/
struct ForStmtInfo * createForStatement(char * counter, struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct StmtListInfo * elsestmt)
{
	struct ForStmtInfo * forstmt = (struct ForStmtInfo *)malloc(sizeof(struct ForStmtInfo));
	forstmt->counter=(char*)malloc(sizeof(strlen(counter)+1));
	strcpy(forstmt->counter, counter);
	forstmt->expr=expr;
	forstmt->stmtlist=stmtlist;
	forstmt->elsestmt=elsestmt;
	return forstmt; 
}

/* Создание while
* \param[in] expr условие выполнения
* \param[in] stmtlist тело цикла
* \param[in] elsestmt тело else после цикла
* \return указатель while
*/
struct WhileStmtInfo * createWhileStatement(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct StmtListInfo * elsestmt)
{
	struct WhileStmtInfo * whilestmt = (struct WhileStmtInfo *)malloc(sizeof(struct WhileStmtInfo));
	whilestmt->expr=expr;
	whilestmt->stmtlist=stmtlist;
	whilestmt->elsestmt=elsestmt;
	return whilestmt; 
}