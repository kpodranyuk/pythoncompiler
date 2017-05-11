/*
* \file bypass_semant_tree.h хранит в себе код обхода дерева на этапе семантики (1 обход)
*/
#include "tree_structs.h"

void checkExpr(struct ExprInfo * expr);
void checkIfStmt(struct IfStmtInfo * ifstmt);
void checkWhileStmt(struct WhileStmtInfo * whilestmt);
void checkForStmt(struct ForStmtInfo * forstmt);
void checkFuncDefStmt(struct FuncDefInfo * funcdefstmt);
void checkContinueBreakStmt(enum StmtType type);
void checkReturnStmt(struct ExprInfo * expr);
void checkDelStmt(struct ExprInfo * expr);
void checkFuncParams(struct DefFuncParamListInfo* params);

void printStatementList(struct StmtListInfo* root);