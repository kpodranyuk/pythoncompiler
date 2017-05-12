/*
* \file bypass_semant_tree.h хранит в себе код обхода дерева на этапе семантики
*/
#include "tree_structs.h"
#include <vector>

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
	* Хранит в себе список инициализированных переменных
	*/
	std::vector<std::string> varNames;
	/*
	* Хранит в себе список объявленных функций
	*/
	std::vector<std::string> funcNames;


	/*
	*	---------- Методы класса ----------
	*/

	/* Удалить строку из вектора
	* @author Kate
	* \param[in] vec вектор строк
	* \param[in] str строка
	*/
	void TreeTraversal::deleteString(std::vector<std::string>& vec, std::string str);

	/* Проверить содержит ли вектор строку
	* @author Kate
	* \param[in] vec вектор строк
	* \param[in] str строка
	*/
	bool containsString(std::vector<std::string>& vec, std::string str) const;

	/* Проверить узел-выражение дерева
	* @author Kate
	* \param[in|out] expr узел выражения
	*/
	void checkExpr(struct ExprInfo * expr) throw(char*);

	/* Проверить узел-развилку дерева
	* @author Nikita
	* \param[in|out] ifstmt узел-развилка
	*/
	void checkIfStmt(struct IfStmtInfo * ifstmt);

	/* Проверить узел-цикл while дерева
	* @author Nikita
	* \param[in|out] whilestmt узел цикла while
	*/
	void checkWhileStmt(struct WhileStmtInfo * whilestmt);

	/* Проверить узел-цикл for дерева
	* @author Nikita
	* \param[in|out] forstmt узел цикла for
	*/
	void checkForStmt(struct ForStmtInfo * forstmt);

	/* Проверить узел определения функции дерева
	* @author Kate
	* \param[in|out] funcdefstmt узел определения функции дерева
	*/
	void checkFuncDefStmt(struct FuncDefInfo * funcdefstmt);

	/* Проверить узел операторов прерывания/продолжения цикла
	* @author Nikita
	* \param[in|out] type тип узла
	*/
	void checkContinueBreakStmt(enum StmtType type);

	/* Проверить узел оператора возвращения из функции
	* @author Kate
	* \param[in|out] expr узел выражения
	*/
	void checkReturnStmt(struct ExprInfo * expr) throw(char*);

	/* Проверить узел оператора удаления значения
	* @author Kate
	* \param[in|out] expr узел выражения
	*/
	void checkDelStmt(struct ExprInfo * expr);

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