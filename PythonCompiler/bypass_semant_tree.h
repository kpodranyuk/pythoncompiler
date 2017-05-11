/*
* \file bypass_semant_tree.h хранит в себе код обхода дерева на этапе семантики (1 обход)
*/
#include "tree_structs.h"
#include "semant_state.h"

void checkExpr(struct ExprInfo * expr);
void checkIfStmt(struct IfStmtInfo * ifstmt);
void checkWhileStmt(struct WhileStmtInfo * whilestmt);
void checkForStmt(struct ForStmtInfo * forstmt);
void checkFuncDefStmt(struct FuncDefInfo * funcdefstmt);
void checkContinueBreakStmt(enum StmtType type);
void checkReturnStmt(struct ExprInfo * expr);
void checkDelStmt(struct ExprInfo * expr);
void checkFuncParams(struct DefFuncParamListInfo* params);

/* Обойти дерево и дополнить его аттрибутами
* @author Kate
* \param[in|out] root список корней дерева
*/
void checkStatementList(struct StmtListInfo* root);