#ifndef CODE_GENERATION_H
#define CODE_GENERATION_H

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include "tables_structs.h"

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

class CodeGeneration
{
private:

	struct ClassTable_Elem* prog;			// ��������� �� ������� ������
	struct ConstTable_List* ct;				// ��������� �� ������� ��������
	struct ConstTable_Consts* ct_consts;	// ��������� �� ��������� ������� ��������
	struct VariableTable_List* vars;		// ��������� �� ������� ��������� ���������� �������
	struct StmtListInfo* treeRoot;			// ��������� �� ������

	int fileDesc;	// ���������� class �����

	/*������ ��� ������ ���� � class ����*/
	unsigned char u1;
	unsigned short int u2;
	unsigned int u4;
	short int s2;
	int s4;


	/*---�������� ������ ��� ���������---*/

	/* ��������� ���� ��� ������� ��������
	*/
	void generateConstsTable();

	/* ��������� ���� ��� ������� ����� ������
	*/
	void generateFieldsTable();

	/* ��������� ���� ��� ������� ������� ������
	*/
	void generateMethodsTable();


	/* ������������� ��� ��� ������ �����������
	* \param[in] stmtList ���� ��������� �����
	*/
	void generateCodeForStatementList(struct StmtListInfo* stmtList);

	/* ������������� ��� ��� ���������
	* \param[in] expr ���������
	*/
	void generateCodeForExpr(struct ExprInfo * expr);

	/* ������������� ��� ��� ���� ��������
	* \param[in] ifstmt ���� ��������
	*/
	void generateCodeForIfStmt(struct IfStmtInfo * ifstmt);

	/* ������������� ��� ��� ���� ����� while
	* \param[in] whilestmt ���� ����� while
	*/
	void generateCodeForWhileStmt(struct WhileStmtInfo * whilestmt);

	/* ������������� ��� ��� ���� ����� for
	* \param[in] forstmt ���� ����� for
	*/
	void generateCodeForForStmt(struct ForStmtInfo * forstmt);

	/* ������������� ��� ��� ����� ���������� � ����������� �����
	* \param[in] contBreakStmt ���� break ��� continue
	*/
	void generateCodeForContinueBreakStmt(struct StmtInfo* contBreakStmt);

	/* ������������� ��� ��� ���� �������� ����������
	* \param[in] expr ���� ��������� �������� ����������
	*/
	void generateCodeForDelStmt(struct ExprInfo * expr);

	/* ������������� ��� ��� ���� ���������� �������(��� ���� ����� ��������)
	* \param[in] funcDef ���� ���������� �������
	*/
	void generateCodeForFuncDef(struct FuncDefInfo * funcDef);



public:

	// �����������, ����������� ��� �������
	CodeGeneration(struct ClassTable_Elem* prog, struct ConstTable_List* ct, struct ConstTable_Consts* ct_consts, struct VariableTable_List* vars);
	~CodeGeneration(void);

	// ����� ��������� ����-���� class �����
	void generateCode(struct StmtListInfo* treeRoot);
};

#endif