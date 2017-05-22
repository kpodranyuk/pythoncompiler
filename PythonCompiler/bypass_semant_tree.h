/*
* \file bypass_semant_tree.h хранит в себе код обхода дерева на этапе семантики
*/
#include "tree_structs.h"
#include <vector>
#include <map>

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
	* Хранит в себе возможный тип константы элемента таблицы
	*/
	enum TableElemType{
		_UTF8,	
		_INT,
		_STRING,
		_NAMEnTYPE,
		_FIELDREF,
		_METHODREF,
		_CLASS
	};

	/*
	* Хранит в себе элемент таблицы констант
	*/
	struct TableElement{
		int number;						// Номер константы
		int strNumber;					// Номер строки
		enum TableElemType type;		// Тип элемента
		int isStatic;					// Является ли статичной
		std::string val;				// Значение
		int localFor;					// Номер константы на методРеф для которого этот элемент является локальным
	};

	/*
	* Регистрация типа пары для map
	* ключ - строка - область видимости (global или имя функции)
	* значение - вектор элементов таблицы констант
	*/
	/*typedef std::map<std::string, std::vector<struct TableElement*> > Table;
	typedef std::pair<std::string, std::vector<struct TableElement*> > TablePair;
	std::vector<TablePair> prog;
	Table programm_table;*/

	// Создаем вектор элементов таблиц глобального кода
	std::vector<struct TableElement*> globalTable;
	std::string currentFuncName;
	int ValNum;
	int TypeNum;
	int MassTypeNum;

	/*
	* Хранит в себе список инициализированных переменных
	*/
	std::vector<std::string> varNames;
	/*
	* Хранит в себе список объявленных функций
	*/
	std::vector<struct FunctionHeader*> funcNames;


	/*
	*	---------- Методы класса ----------
	*/

	/* Тип элемента в строковом представлении
	* @author Kate
	* \param[in] type Тип элемента
	*/
	std::string convertTypeToString(enum TableElemType type);

	/* Создать элемент строки таблицы констант
	* @author Kate
	* \param[in] num Номер константы
	* \param[in] strNum Номер строки
	* \param[in] type Тип элемента
	* \param[in] val Значение
	*/
	struct TableElement* makeTableEl(int num, int strNum, enum TableElemType type, int isStatic, std::string val, int local);

	/* Проверить, равны ли два заголовка функции
	* @author Kate
	* \param[in] first заголовок первой функции
	* \param[in] second заголовок второй функции
	*/
	void checkFuncCall(struct FunctionHeader* first, struct FunctionHeader* second) const;

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

	/* Проверить узел-выражение дерева для составления таблицы
	* @author Kate, Nikita
	* \param[in] expr узел выражения
	* \param[in|out] table таблица
	* \param[in] local номер константы, для которой все добавляемые элементы таблицы будут локальными
	* \param[in] typeAboveExpression тип над выражения
	*/
	void parseExprForTable(const struct ExprInfo * expr, int* constNum, int local, enum ExprType typeAboveExpression);

	/* Обойти дерево (список стейтментов) и дополнить его аттрибутами
	* @author Kate
	* \param[in] root список корней дерева
	* \param[in|out] constNum номер константы
	* \param[in] local номер константы, для которой все добавляемые элементы таблицы будут локальными
	*/
	void parseStmtListForTable(const struct StmtListInfo* root, int* constNum, int local);

	/* Проверить узел-объявление функции дерева для составления таблицы
	* @author Kate
	* \param[in] funcdefstmt узел выражения
	* \param[in|out] constNum номер константы
	* \param[in] local номер константы, для которой все добавляемые элементы таблицы будут локальными
	*/
	void parseFuncDefForTable(const struct FuncDefInfo * funcdefstmt, int* constNum, int local);

	/* Проверить узел-объявление функции дерева для составления таблицы
	* @author Kate
	* \param[in] funcdefstmt узел выражения
	* \param[in|out] constNum номер константы
	* \param[in] local номер константы, для которой все добавляемые элементы таблицы будут локальными
	*/
	void parseValTypeForTable(const struct ValInfo * val, int* constNum, int local);

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
};