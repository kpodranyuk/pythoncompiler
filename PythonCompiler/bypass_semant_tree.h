/*
* \file bypass_semant_tree.h хранит в себе код обхода дерева на этапе семантики
*/
#include "tree_structs.h"
#include "tables_structs.h"
#include "struct_functions.h"
#include <vector>
#include <map>
#include <string>

#pragma warning(disable : 4290)

/*
* Класс обхода дерева программы
*/
class TreeTraversal{
	/*!
	*	!!!!! Закрытая часть класса !!!!!
	*/
private:
	/*
	*	---------- Поля класса ----------
	*/

	/*
	* Хранит в себе глобальное состояние анализатора
	*/
	enum GlobalState{
		_MAIN_STATE,	// Глобальный код (функция main)
		_FUNC_STATE		// Локальный код (внутри функции, определяемой пользователем)
	} gl_state;

	/*
	* Хранит в себе локальное состояние анализатора
	*/
	enum LocalState{
		_CYCLE_STATE,	// Анализатор внутри цикла
		_REGULAR_STATE	// Анализатор не внутри цикла
	} lc_state;

	/*
	* Хранит в себе заголовок функции
	*/
	struct FunctionHeader{
		char* functionName;						// Имя функции
		struct DefFuncParamListInfo* params;		// Параметры функции
	};

	/*
	* Хранит в себе имя поля класса и номер его fieldRef
	*/
	struct opInfo{
		char* opName;		// Имя Поля
		int FR;				// Номер FieldRef или номер локальной переменной
		int local;			// Номер MethodRef, для которого локальна переменная
	};

	/*
	* Хранит в себе дескриптор метода и строку с его объявлением
	*/
	struct mDescInfo{
		char* desc;		// Дескриптор метода
		int num;		// Номер строки с объявлением
	};

	/*
	* Хранит в себе имя функции и ее methodRef
	*/
	struct methodInfo{
		char* name;		// Имя метода
		int MR	;		// methodRef
	};

	/*
	* Хранит в себе константу и ссылку на нее в таблице
	*/
	struct constInfo{
		enum ValType t;	// Тип константы
		int val;		// Значение константы инт/бул
		char* strVal;	// Строкое значение константы
		int num;		// Номер в таблице констант
	};

	bool hasReturn;		// Флаг того, что текущая функция имеет возвращаемое значение

	std::string currentFuncName;
	int valClassDesc;	// Ссылка на константу класса Value в таблице констант
	int typeDesc;		// Ссылка на константу дескриптора типа LValue;

	struct ClassTable_Elem* prog;			// Указатель на таблицу класса
	struct ConstTable_List* ct;				// Указатель на таблицу констант
	struct ConstTable_Consts* ct_consts;	// Указатель на константы таблицы констант

	//struct FieldTable_List* fields;			// Указатель на таблицу полей класса
	//struct MethodTable_List* methods;		// Указатель на таблицу полей класса
	struct VariableTable_List* vars;		// Указатель на таблицу локальных переменных методов
	/*
	* Хранит в себе список полей класса
	*/
	std::vector<struct opInfo*> ops;
	/*
	* Хранит в себе список дескрипторов методов
	*/
	std::vector<struct mDescInfo*> mDescs; 
	/*
	* Хранит в себе список дескрипторов методов
	*/
	std::vector<struct methodInfo*> MRs;
	/*
	* Хранит в себе список констант в таблице констант
	*/
	std::vector<struct constInfo*> consts;

	


	/*
	* Хранит в себе список имен объявленных функций
	*/
	std::vector<std::string> funcNames;
	/*
	* Хранит в себе список объявленных функций
	*/
	std::vector<struct FunctionHeader*> funcHeaders;

	/*
	* Хранит в себе список инициализированных переменных
	* Ключ - имя функции
	* Глобальный год - global
	*/
	std::map<std::string,std::vector<std::string>> varDecls;

	/*
	* Хранит в себе список используемых переменных
	* Ключ - имя функции
	* НЕ ЗАПОЛНЯЕТСЯ В ГЛОБАЛЬНОМ КОДЕ
	*/
	std::map<std::string,std::vector<std::string>> varUsage;

	/*
	* Хранит в себе список стандартных функций языка
	*/
	std::vector<std::string> libFuncs;

	/*
	*							---------- Методы класса ----------
	*/
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ПЕРВЫЙ ОБХОД !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	/* Проверить узел-выражение дерева
	* @author Kate, Nikita
	* \param[in|out] expr узел выражения
	* \param[in] assign флаг, показывающий, что в выражении уже было присваивание
	*/
	void checkExpr(struct ExprInfo * expr, bool assign) throw(char*);

	/* Проверить узел-развилку дерева
	* @author Nikita
	* \param[in|out] ifstmt узел-развилка
	*/
	void checkIfStmt(struct IfStmtInfo * ifstmt) throw(char*);

	/* Проверить узел-цикл while дерева
	* @author Nikita
	* \param[in|out] whilestmt узел цикла while
	*/
	void checkWhileStmt(struct WhileStmtInfo * whilestmt) throw(char*);

	/* Проверить узел-цикл for дерева
	* @author Nikita
	* \param[in|out] forstmt узел цикла for
	*/
	void checkForStmt(struct ForStmtInfo * forstmt) throw(char*);

	/* Проверить узел определения функции дерева
	* @author Kate
	* \param[in|out] funcdefstmt узел определения функции дерева
	*/
	void checkFuncDefStmt(struct FuncDefInfo * funcdefstmt) throw(char*);

	/* Проверить узел операторов прерывания/продолжения цикла
	* @author Nikita
	* \param[in|out] type тип узла
	*/
	void checkContinueBreakStmt(struct StmtInfo* contBreakStmt) throw(char*);

	/* Проверить узел оператора возвращения из функции
	* @author Kate
	* \param[in|out] retStmt узел стейтмент
	* \param[in|out] expr узел выражения
	*/
	void checkReturnStmt(struct StmtInfo* retStmt, struct ExprInfo * expr) throw(char*);

	/* Проверить узел оператора удаления значения
	* @author Kate
	* \param[in|out] expr узел выражения
	*/
	void checkDelStmt(struct ExprInfo * expr) throw(char*);

	/* Проверить узел параметров функции при объявлении
	* @author Kate
	* \param[in|out] params список параметров функции при объявлении
	*/
	void checkFuncParams(struct DefFuncParamListInfo* params);

	/* Обойти дерево (список стейтментов) и дополнить его аттрибутами
	* @author Kate
	* \param[in|out] root список корней дерева
	*/
	void checkStatementList(struct StmtListInfo* root) throw(char*);

	/* Проверить, равны ли два заголовка функции
	* @author Kate
	* \param[in] first заголовок первой функции
	* \param[in] second заголовок второй функции
	*/
	void checkFuncCall(struct FunctionHeader* first, struct FunctionHeader* second) const;

	//!!!!!!!!!!!!!!!!!!!!!!!! ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ПЕРВОГО ОБХОДА !!!!!!!!!!!!!!!!!!!!!!!!!!!!

	/* Проверить, является ли вызываемая функция библиотечной
	* @author Kate
	* \param[in] expr выражение вызова функции
	* \return true, если является, иначе - false
	*/
	bool isLibFunctionCall(struct ExprInfo * expr);

	/* Проверить, равны ли два заголовка функции
	* @author Kate
	* \param[in] first заголовок первой функции
	* \param[in] second заголовок второй функции
	*/
	bool isEqualFuncHeaders(struct FunctionHeader* first, struct FunctionHeader* second) const;

	/* Проверить, равны ли два списка параметров функции при объявлении
	* @author Kate
	* \param[in] first первый список
	* \param[in] second второй список
	*/
	bool isEqualDefFuncParams(struct DefFuncParamListInfo* first, struct DefFuncParamListInfo* second) const;

	/* Проверить, равны ли два значения
	* @author Kate
	* \param[in] first первое значение
	* \param[in] second второе значение
	*/
	bool isEqualVarVals(struct ValInfo* first, struct ValInfo* second) const;

	/* Удалить функцию из вектора
	* @author Kate
	* \param[in|out] vec вектор функций
	* \param[in] func функция
	*/
	void deleteFuncHeader(std::vector<struct FunctionHeader*>& vec, struct FunctionHeader* func);

	/* Удалить функцию из вектора
	* @author Kate
	* \param[in|out] vec вектор функций
	* \param[in] func имя функции
	*/
	void deleteFuncHeader(std::vector<struct FunctionHeader*>& vec, std::string funcName);

	/* Проверить содержит ли вектор функцию
	* @author Kate
	* \param[in] vec вектор функций
	* \param[in] func функция
	*/
	bool containsFuncHeader(std::vector<struct FunctionHeader*>& vec, struct FunctionHeader* func) const;

	/* Удалить строку из вектора
	* @author Kate
	* \param[in|out] vec вектор строк
	* \param[in] str строка
	*/
	void deleteString(std::vector<std::string>& vec, std::string str);

	/* Проверить содержит ли вектор строку
	* @author Kate
	* \param[in] vec вектор строк
	* \param[in] str строка
	*/
	bool containsString(std::vector<std::string>& vec, std::string str) const;

	/* Проверить узел-выражение дерева на наличие операции "присваивание"
	* @author Nikita
	* \param[in|out] expr узел выражения
	* \return признак есть ли операция "присваивание" или нет
	*/
	bool exprContainsAssign(struct ExprInfo * expr);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ВТОРОЙ ОБХОД ДЕРЕВА !!!!!!!!!!!!!!!!!!!!!!!!!!!!

	/* \
	* @author Kate
	* \param[in] funcdefstmt узел выражения
	* \param[in|out] constNum номер константы
	* \param[in] local номер константы, для которой все добавляемые элементы таблицы будут локальными
	*/
	void parseIfForTable(const struct IfStmtInfo * ifstmt, int local);

	/* \
	* @author Kate
	* \param[in] funcdefstmt узел выражения
	* \param[in|out] constNum номер константы
	* \param[in] local номер константы, для которой все добавляемые элементы таблицы будут локальными
	*/
	void parseForForTable(const struct ForStmtInfo * forstmt, int local);

	/* \
	* @author Kate
	* \param[in] funcdefstmt узел выражения
	* \param[in|out] constNum номер константы
	* \param[in] local номер константы, для которой все добавляемые элементы таблицы будут локальными
	*/
	void parseWhileForTable(const struct WhileStmtInfo * whilestmt, int local);

	/* Проверить узел-выражение дерева для составления таблицы
	* @author Kate, Nikita
	* \param[in] expr узел выражения
	* \param[in|out] table таблица
	* \param[in] local номер константы, для которой все добавляемые элементы таблицы будут локальными
	* \param[in] typeAboveExpression тип над выражения
	*/
	void parseExprForTable(struct ExprInfo * expr, int local, enum ExprType typeAboveExpression);

	/* Обойти дерево (список стейтментов) и дополнить его аттрибутами
	* @author Kate
	* \param[in] root список корней дерева
	* \param[in|out] constNum номер константы
	* \param[in] local номер константы, для которой все добавляемые элементы таблицы будут локальными
	*/
	void parseStmtListForTable(const struct StmtListInfo* root, int local);

	/* Проверить узел-объявление функции дерева для составления таблицы
	* @author Kate
	* \param[in] funcdefstmt узел выражения
	* \param[in|out] constNum номер константы
	* \param[in] local номер константы, для которой все добавляемые элементы таблицы будут локальными
	*/
	void parseFuncDefForTable(struct FuncDefInfo * funcdefstmt, int local);

	/* Проверить узел-объявление функции дерева для составления таблицы
	* @author Kate
	* \param[in] funcdefstmt узел выражения
	* \param[in|out] constNum номер константы
	* \param[in] local номер константы, для которой все добавляемые элементы таблицы будут локальными
	*/
	void parseValTypeForTable(struct ValInfo * val, int local);

	/* Создать элемент строки таблицы констант
	* @author Nikita
	* \param[in] type тип константы
	* \param[in] numberInTable номер константы в таблице констант
	* \param[in] utf8 значение константы(строка или название переменной или дескриптор)
	* \param[in] val_int интовое значение константы
	* \param[in] arg1 первый аргумент ссылки(на имя)
	* \param[in] arg2 второй аргумент ссылки(на тип)
	*/
	struct ConstTable_Elem* makeTableEl(enum ConstType type, int* numberInTable, char * utf8, int val_int, int arg1, int arg2);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ВТОРОГО ОБХОДА !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	/*
	* Инициализировать таблицу констант
	* \param[in|out] ct таблица констант
	*/
	void initializeConstTable();

	/*
	* Добавить элемент таблицы в конец таблицы
	* \param[in] ce элемент для добавления
	*/
	void appendToConstTable(struct ConstTable_Elem* ce);

	/*
	* Добавить элемент таблицы в конец таблицы
	* \param[in] ce элемент для добавления
	*/
	void appendToFieldTable(struct FieldTable_Elem* fe);

	/*
	* Добавить элемент таблицы в конец таблицы
	* \param[in] ce элемент для добавления
	*/
	void appendToMethodTable(struct MethodTable_Elem* me);

	/*
	* Добавить элемент таблицы в конец таблицы
	* \param[in] ce элемент для добавления
	*/
	void appendToVarsTable(struct Variable* v);

	/*
	* \
	* \param[in] ce элемент для добавления
	*/
	void appendToMDescs(char* desc, int num);

	/*
	* \
	* \param[in] ce элемент для добавления
	*/
	void appendMToMT(char* name, int methodRef);

	/*
	* \
	* \param[in] ce элемент для добавления
	*/
	void appendCToC(enum ValType type, int val, char* charVal, int number);

	void findOpInCT(struct ExprInfo* expr,int local);

	void findMInMT(struct ExprInfo* expr);
	
	bool findCInCT(struct ValInfo* val);

	int findMDesc(char* desc);


	// !!!!!!!!!!!!!!!!!!!!!!!! ФУНКЦИЯ СОСТАВЛЕНИЯ СТРОКИ ИСКЛЮЧЕНИЯ !!!!!!!!!!!!!!!!!!!!

	/*
	* Создать строку для выброса исключения
	* \param[in] message сообщение об ошибке
	* \param[in] location расположение ошибки
	*/
	char* makeStringForException(char* message, struct CodeLocation* location);

	/*!
	*	!!!!! Публичная часть класса !!!!!
	*/
public:
	// Конструктор по умолчанию
	TreeTraversal();

	// Проверка дерева (первый обход)
	void fixTree(struct StmtListInfo* root) throw(char*);

	// Составление таблиц (второй обход)
	void makeTables(const struct StmtListInfo* treeRoot);

	// Печать аттрибутированного дерева (третий обход - ?)
	void printTree(const struct StmtListInfo* treeRoot);

	// Гетеры на таблицы
	struct ClassTable_Elem* getClassTable();
	struct ConstTable_List* getConstTableList();
	struct ConstTable_Consts* getConstTableConsts();
	struct VariableTable_List* getVariableTableList();
};