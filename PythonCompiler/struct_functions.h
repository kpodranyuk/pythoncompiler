#include <stdio.h>
#include <string.h>
#include "tree_structs.h"

#pragma warning(disable : 4996)

/* Создание позиции элемента кода программы
* @author Kate
* \param[in] FL Строка начала элемента
* \param[in] FC Столбец начала элемента
* \param[in] LL Строка конца элемента
* \param[in] LC Столбец конца элемента
* \return указатель на структуру позиции элемента кода программы
*/
struct CodeLocation* createCodeLocation(int FL, int FC, int LL, int LC);

/* Создание листа выражений
* @author Kate
* \param[in] expr указатель на добавляемое выражение
* \param[in] exprlist указатель на лист
* \return указатель на лист
*/
struct ExprListInfo * createExprList(struct ExprInfo * expr, struct ExprListInfo * exprlist);

/* Создание простого узла из выражения (операнда или значения)
* @author Kate
* \param[in] type тип выражения
* \param[in] opName имя операнда
* \param[in] val значение операнда
* \return указатель на узел
*/
struct ExprInfo* createSimpleExpr(enum ExprType type, char* opName, struct ValInfo* val, struct CodeLocation* loc);

/* Создание узла выражения из выражений
* @author Kate
* \param[in] type тип выражения
* \param[in] left левое выражение
* \param[in] right правое выражение
* \return указатель на новый узел
*/
struct ExprInfo* createExprInfo(enum ExprType type, struct ExprInfo* left, struct ExprInfo* right, struct CodeLocation* operationLoc);

/* Создание узла выражения инициализации массива
* \param[in] type тип выражения
* \param[in] exprlist лист выражений
* \return указатель на новый узел
*/
struct ExprInfo* createInitListInfo(enum ExprType type, struct ExprListInfo * exprlist);

/* Создание узла выражения действия над массивом
* \param[in] type тип выражения
* \param[in] left левое выражение(к чему применяем)
* \param[in] right правое выражение
* \param[in] actName имя применяемой операции к массиву
* \return указатель на новый узел
*/
struct ExprInfo* createActListInfo(enum ExprType type, struct ExprInfo* left, struct ExprInfo* right, char * actName);

/* Создание узла выражения из вызова функции
* @author Kate
* \param[in] type тип выражения
* \param[in] funcName имя функции
* \param[in] args список аргументов
* \return указатель на новый узел
*/
struct ExprInfo* createExprInfoFromFuncCall(enum ExprType type, char* funcName, struct ExprListInfo* args);

/* Создание узла из типа выражения
* @author Kate
* \param[in] type тип значения 
* \param[in] logVal логическое значение 
* \param[in] stringVal строковое значение 
* \param[in] numVal целочисленное значение 
*/
struct ValInfo* createValNode(enum ValType type, bool logVal, char* stringVal, int numVal, struct CodeLocation* loc);

/* Создание списка параметров функции
* @author Kate
* \param[in] newParamName новый параметр функции
* \param[in] params список параметров
* \return указатель на новый список
*/
struct DefFuncParamListInfo* createDefFuncParamListInfo(char* newParamName, struct ValInfo* newParamVal, struct DefFuncParamListInfo* params, struct CodeLocation* loc);

/* Создание узла объявления функции
* @author Kate
* \param[in] funcName имя функции
* \param[in] params параметры функции
* \param[in] funcBody тело функции
* \return указатель на узел объявления функции
*/
struct FuncDefInfo* createFuncDef(char* funcName, struct DefFuncParamListInfo* params, struct StmtListInfo* funcBody, struct CodeLocation* nameL);

/* Создание стейтмента для определения функции
* @author Kate
* \param[in] type тип стейтмента
* \param[in] funcDef указатель на объявление функции
* \return указатель на стейтмент
*/
struct StmtInfo* createFromFuncDefStatement(enum StmtType type, struct FuncDefInfo* funcDef);

/* Создание стейтмент листа
* \param[in] stmt указатель на добавляемый стейтмент
* \param[in] stmtlist указатель на стейтмент лист
* \return указатель на стейтмент лист
*/
struct StmtListInfo * createStatementList(struct StmtInfo * stmt, struct StmtListInfo * stmtlist);

/* Создание стейтмента для выражения
* \param[in] type тип стейтмента
* \param[in] ifstmt указатель на if стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createFromExprStatement(enum StmtType type, struct ExprInfo * exprstmt);

/* Создание стейтмента для развилки
* \param[in] type тип стейтмента
* \param[in] ifstmt указатель на if стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createFromIfStatement(enum StmtType type, struct IfStmtInfo * ifstmt);

/* Создание стейтмента для цикла for
* \param[in] type тип стейтмента
* \param[in] forstmt указатель на for стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createFromForStatement(enum StmtType type, struct ForStmtInfo * forstmt);

/* Создание стейтмента для цикла while
* \param[in] type тип стейтмента
* \param[in] whilestmt указатель на while стейтмент
* \return указатель на стейтмент
*/
struct StmtInfo * createFromWhileStatement(enum StmtType type, struct WhileStmtInfo * whilestmt);

/* Создание стейтмента для continue, break
* \param[in] type тип стейтмента
* \return указатель на стейтмент
*/
struct StmtInfo * createFromContinueBreakStatement(enum StmtType type, struct CodeLocation* loc);

/* Создание стейтмента для return
* \param[in] type тип стейтмента
* \param[in] expr возвращаемое выражение
* \return указатель на стейтмент
*/
struct StmtInfo * createFromReturnStatement(enum StmtType type, struct ExprInfo * expr, struct CodeLocation* loc);

/* Создание стейтмента для del
* \param[in] type тип стейтмента
* \param[in] operand очищаемый операнд
* \return указатель на стейтмент
*/
struct StmtInfo * createFromDelStatement(enum StmtType type, char * operand, struct CodeLocation* delLoc, struct CodeLocation* opLoc);

/* Создание if
* \param[in] expr условное выражение
* \param[in] stmtlist тело
* \param[in] eliflist список elif
* \param[in] elsestmtlist тело блока else
* \return указатель на if стейтмент
*/
struct IfStmtInfo * createIfStatement(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct ElifListInfo * eliflist, struct StmtListInfo * elsestmtlist);

/* Создание elif листа
* \param[in] expr условное выражение
* \param[in] stmtlist тело
* \param[in] eliflist указатель на список elif-ов
* \return указатель на список
*/
struct ElifListInfo * createElifList(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct ElifListInfo * eliflist);

/* Создание for
* \param[in] counter счетчик цикла
* \param[in] expr выражение по чем проходит цикл
* \param[in] stmtlist тело цикла
* \param[in] elsestmt тело else после цикла
* \return указатель for
*/
struct ForStmtInfo * createForStatement(char * counter, struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct StmtListInfo * elsestmt);

/* Создание while
* \param[in] expr условие выполнения
* \param[in] stmtlist тело цикла
* \param[in] elsestmt тело else после цикла
* \return указатель while
*/
struct WhileStmtInfo * createWhileStatement(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct StmtListInfo * elsestmt);