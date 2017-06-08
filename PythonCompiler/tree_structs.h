#include <stdio.h>
#include <malloc.h>

#pragma once

/*
* Хранит в себе позицию элемента кода программы
*/
struct CodeLocation
{
	int firstLine;		// Строка начала элемента
	int firstColumn;	// Столбец начала элемента
	int lastLine;		// Строка конца элемента
	int lastColumn;		// Столбец конца элемента
};

/*
* Хранит в себе типы выражения
*/
enum ExprType
{
	_OR,					// Или
	_AND,					// И
	_NOT,					// Не
	_NOT_EQUAL,				// Не равно
	_EQUAL,					// Равно
	_GREATER,				// Больше
	_GREATER_OR_EQUAL,		// Больше либо равно
	_LESS,					// Меньше
	_LESS_OR_EQUAL,			// Меньше либо равно
	_SUB,					// Разность
	_ADD,					// Сумма
	_INT,					// Целая часть от деления
	_MOD,					// Остаток от деления
	_DIV,					// Деление
	_MUL,					// Умножение
	_POW,					// Возведение в степень
	_ASSIGN,				// Присовение
	_ARRID,					// Обращение по индексу массива
	_ARRID_AND_ASSIGN,		// Присвоение нового значения элементу массива, взятого по индексу
	_ARRINIT,				// Инициализация массива
	_ARRACT,				// Действия над массивом
	_VARVAL,				// Значение операнда
	_OPERAND,				// Операнд (идентификатор)
	_FUNCCALL,				// Вызов функции
	_UMINUS,				// Унарный минус
	__COUNTER,
};

/*
* Хранит в себе значения операнда
*/
enum ValType
{
	_TRUE,		// Истина
	_FALSE,		// Ложь
	_STRING,	// Строка
	_NUMBER,	// Целое число
	_NONE		// Значение None
};

/*
* Хранит в себе информацию об узле типа выражения
*/
struct ValInfo
{
	// Тип выражения
	enum ValType type;

	// Значение выражения-строки
	char* stringVal;
	// Значение выражения-целого числа
	int intVal;
	// Значение выражения-логической переменной
	bool logVal;

	// Позиция в коде
	struct CodeLocation* loc;
};


/*
* Хранит в себе информацию об узле типа выражения
*/
struct ExprInfo
{
	// Левый операнд выражения
	struct ExprInfo * left;
	// Правый операнд выражения
	struct ExprInfo * right;
	// Указатель на следующий элемент (выражение)
	struct ExprInfo * next;
	// Указатель средний элемент(тернарная операция _ARRID_AND_ASSIGN)
	struct ExprInfo * middle;
	// Указатель на список аргументов выражения
	struct ExprListInfo * arglist;
	// Тип выражения
	enum ExprType type;

	// Имя переменной
	char* idName;
	// Значение выражения-переменной
	struct ValInfo* exprVal;

	// Позиция в коде
	struct CodeLocation* loc;
};
/*
* Хранит в себе указатель на начало и конец списка выражений
*/
struct ExprListInfo
{
	struct ExprInfo * first;	// Начало списка
	struct ExprInfo * last;		// Конец списка
};

/*
* Хранит в себе информацию о входных параметрах функции при ее объявлении
*/
struct DefFuncParamInfo
{
	// Указатель на следующий элемент (выражение)
	struct DefFuncParamInfo * next;

	// Имя параметра
	char* paramName;
	// Значение параметра
	struct ValInfo* paramVal;

	struct CodeLocation* loc;
};
/*
* Хранит в себе указатель на начало и конец списка параметров функции
*/
struct DefFuncParamListInfo
{
	struct DefFuncParamInfo * first;	// Начало списка
	struct DefFuncParamInfo * last;		// Конец списка
};
/*
* Хранит в себе информацию о функции при ее объявлении
*/
struct FuncDefInfo
{
	// Имя функции
	char* functionName;

	// Позиция имени функции
	struct CodeLocation* nameLoc;

	// Флаг того, имеется ли у функции возвращаемое значение
	bool hasRet;

	// Параметры функции
	struct DefFuncParamListInfo* params;
	// Тело функции
	struct StmtListInfo* body;
};


/*
* Тип стейтмента
*/
enum StmtType
{
	_EXPR, // Выражение
	_IF, // Развилки
	_FOR, // Цикл для
	_WHILE, // Цикл пока
	_FUNC_DEF, // Объявление функции
	_RETURN, // Возвращение значение
	_BREAK, // Прерывани
	_CONTINUE, // Продолжение
	_DEL // Удаление значения переменной из памяти
};

/*
* Хранит в себе информацию об узле типа стейтмента
*/
struct StmtInfo
{
	// Тип стейтмента
	enum StmtType type;
	// Указатель на выражение
	struct ExprInfo * expr;
	// Указатель на развилку
	struct IfStmtInfo * ifstmt;
	// Указатель на цикл for
	struct ForStmtInfo * forstmt;
	// Указатель на цикл while
	struct WhileStmtInfo * whilestmt;
	// Указатель на объявление функции
	struct FuncDefInfo* funcdefstmt;
	// Указатель на следующий стейтмент
	struct StmtInfo * next;

	struct CodeLocation* loc;
};

/*
* Хранит в себе информацию на начало и конец стейтмент листа
*/
struct StmtListInfo
{
	struct StmtInfo * first;
	struct StmtInfo * last;
};

/*
* Хранит в себе информацию об узле типа развилки
*/
struct IfStmtInfo
{
	// Указатель на условное выражение
	struct ExprInfo * expr;
	// Указатель на тело развилки
	struct StmtListInfo * stmtlist;
	// Указатель на лист elif
	struct ElifListInfo * eliflist;
	// Указатель на тело else(а так же else if)
	// !!! БЫЛО IfStmtInfo !!!
	struct StmtListInfo * elsestmtlist;
};

/*
* Хранит в себе информацию об узле типа elif
*/
struct ElifListInfo
{
	// Условное выражение
	struct ExprInfo * expr;
	// Тело развилки
	struct StmtListInfo * stmtlist;
	// Указатель на следующий elif
	struct ElifListInfo * next;
};

/*
* Хранит в себе информацию об узле типа while
*/
struct WhileStmtInfo
{
	// Условное выражение выполнения цикла
	struct ExprInfo * expr;
	// Тело цикла
	struct StmtListInfo * stmtlist;
	// Указатель на тело блока else после цикла
	struct StmtListInfo * elsestmt;
};

/*
* Хранит в себе информацию об узле типа for
*/
struct ForStmtInfo
{
	// Счетчик цикла(переменная)
	char* counter;
	// Указатель на выражение, по чем проходит цикл
	struct ExprInfo * expr;
	// Тело цикла
	struct StmtListInfo * stmtlist;
	// Указатель на тело блока else после цикла
	struct StmtListInfo * elsestmt;
};