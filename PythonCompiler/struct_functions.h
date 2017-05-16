#include <stdio.h>
#include <string.h>
#include "tree_structs.h"

#pragma warning(disable : 4996)

/* �������� ������� �������� ���� ���������
* @author Kate
* \param[in] FL ������ ������ ��������
* \param[in] FC ������� ������ ��������
* \param[in] LL ������ ����� ��������
* \param[in] LC ������� ����� ��������
* \return ��������� �� ��������� ������� �������� ���� ���������
*/
struct CodeLocation* createCodeLocation(int FL, int FC, int LL, int LC);

/* �������� ����� ���������
* @author Kate
* \param[in] expr ��������� �� ����������� ���������
* \param[in] exprlist ��������� �� ����
* \return ��������� �� ����
*/
struct ExprListInfo * createExprList(struct ExprInfo * expr, struct ExprListInfo * exprlist);

/* �������� �������� ���� �� ��������� (�������� ��� ��������)
* @author Kate
* \param[in] type ��� ���������
* \param[in] opName ��� ��������
* \param[in] val �������� ��������
* \return ��������� �� ����
*/
struct ExprInfo* createSimpleExpr(enum ExprType type, char* opName, struct ValInfo* val, struct CodeLocation* loc);

/* �������� ���� ��������� �� ���������
* @author Kate
* \param[in] type ��� ���������
* \param[in] left ����� ���������
* \param[in] right ������ ���������
* \return ��������� �� ����� ����
*/
struct ExprInfo* createExprInfo(enum ExprType type, struct ExprInfo* left, struct ExprInfo* right, struct CodeLocation* operationLoc);

/* �������� ���� ��������� ������������� �������
* \param[in] type ��� ���������
* \param[in] exprlist ���� ���������
* \return ��������� �� ����� ����
*/
struct ExprInfo* createInitListInfo(enum ExprType type, struct ExprListInfo * exprlist);

/* �������� ���� ��������� �������� ��� ��������
* \param[in] type ��� ���������
* \param[in] left ����� ���������(� ���� ���������)
* \param[in] right ������ ���������
* \param[in] actName ��� ����������� �������� � �������
* \return ��������� �� ����� ����
*/
struct ExprInfo* createActListInfo(enum ExprType type, struct ExprInfo* left, struct ExprInfo* right, char * actName);

/* �������� ���� ��������� �� ������ �������
* @author Kate
* \param[in] type ��� ���������
* \param[in] funcName ��� �������
* \param[in] args ������ ����������
* \return ��������� �� ����� ����
*/
struct ExprInfo* createExprInfoFromFuncCall(enum ExprType type, char* funcName, struct ExprListInfo* args);

/* �������� ���� �� ���� ���������
* @author Kate
* \param[in] type ��� �������� 
* \param[in] logVal ���������� �������� 
* \param[in] stringVal ��������� �������� 
* \param[in] numVal ������������� �������� 
*/
struct ValInfo* createValNode(enum ValType type, bool logVal, char* stringVal, int numVal, struct CodeLocation* loc);

/* �������� ������ ���������� �������
* @author Kate
* \param[in] newParamName ����� �������� �������
* \param[in] params ������ ����������
* \return ��������� �� ����� ������
*/
struct DefFuncParamListInfo* createDefFuncParamListInfo(char* newParamName, struct ValInfo* newParamVal, struct DefFuncParamListInfo* params, struct CodeLocation* loc);

/* �������� ���� ���������� �������
* @author Kate
* \param[in] funcName ��� �������
* \param[in] params ��������� �������
* \param[in] funcBody ���� �������
* \return ��������� �� ���� ���������� �������
*/
struct FuncDefInfo* createFuncDef(char* funcName, struct DefFuncParamListInfo* params, struct StmtListInfo* funcBody, struct CodeLocation* nameL);

/* �������� ���������� ��� ����������� �������
* @author Kate
* \param[in] type ��� ����������
* \param[in] funcDef ��������� �� ���������� �������
* \return ��������� �� ���������
*/
struct StmtInfo* createFromFuncDefStatement(enum StmtType type, struct FuncDefInfo* funcDef);

/* �������� ��������� �����
* \param[in] stmt ��������� �� ����������� ���������
* \param[in] stmtlist ��������� �� ��������� ����
* \return ��������� �� ��������� ����
*/
struct StmtListInfo * createStatementList(struct StmtInfo * stmt, struct StmtListInfo * stmtlist);

/* �������� ���������� ��� ���������
* \param[in] type ��� ����������
* \param[in] ifstmt ��������� �� if ���������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromExprStatement(enum StmtType type, struct ExprInfo * exprstmt);

/* �������� ���������� ��� ��������
* \param[in] type ��� ����������
* \param[in] ifstmt ��������� �� if ���������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromIfStatement(enum StmtType type, struct IfStmtInfo * ifstmt);

/* �������� ���������� ��� ����� for
* \param[in] type ��� ����������
* \param[in] forstmt ��������� �� for ���������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromForStatement(enum StmtType type, struct ForStmtInfo * forstmt);

/* �������� ���������� ��� ����� while
* \param[in] type ��� ����������
* \param[in] whilestmt ��������� �� while ���������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromWhileStatement(enum StmtType type, struct WhileStmtInfo * whilestmt);

/* �������� ���������� ��� continue, break
* \param[in] type ��� ����������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromContinueBreakStatement(enum StmtType type, struct CodeLocation* loc);

/* �������� ���������� ��� return
* \param[in] type ��� ����������
* \param[in] expr ������������ ���������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromReturnStatement(enum StmtType type, struct ExprInfo * expr, struct CodeLocation* loc);

/* �������� ���������� ��� del
* \param[in] type ��� ����������
* \param[in] operand ��������� �������
* \return ��������� �� ���������
*/
struct StmtInfo * createFromDelStatement(enum StmtType type, char * operand, struct CodeLocation* delLoc, struct CodeLocation* opLoc);

/* �������� if
* \param[in] expr �������� ���������
* \param[in] stmtlist ����
* \param[in] eliflist ������ elif
* \param[in] elsestmtlist ���� ����� else
* \return ��������� �� if ���������
*/
struct IfStmtInfo * createIfStatement(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct ElifListInfo * eliflist, struct StmtListInfo * elsestmtlist);

/* �������� elif �����
* \param[in] expr �������� ���������
* \param[in] stmtlist ����
* \param[in] eliflist ��������� �� ������ elif-��
* \return ��������� �� ������
*/
struct ElifListInfo * createElifList(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct ElifListInfo * eliflist);

/* �������� for
* \param[in] counter ������� �����
* \param[in] expr ��������� �� ��� �������� ����
* \param[in] stmtlist ���� �����
* \param[in] elsestmt ���� else ����� �����
* \return ��������� for
*/
struct ForStmtInfo * createForStatement(char * counter, struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct StmtListInfo * elsestmt);

/* �������� while
* \param[in] expr ������� ����������
* \param[in] stmtlist ���� �����
* \param[in] elsestmt ���� else ����� �����
* \return ��������� while
*/
struct WhileStmtInfo * createWhileStatement(struct ExprInfo * expr, struct StmtListInfo * stmtlist, struct StmtListInfo * elsestmt);