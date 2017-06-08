#include <stdio.h>
#include <malloc.h>

#pragma once

/*
* ������ � ���� ������� �������� ���� ���������
*/
struct CodeLocation
{
	int firstLine;		// ������ ������ ��������
	int firstColumn;	// ������� ������ ��������
	int lastLine;		// ������ ����� ��������
	int lastColumn;		// ������� ����� ��������
};

/*
* ������ � ���� ���� ���������
*/
enum ExprType
{
	_OR,					// ���
	_AND,					// �
	_NOT,					// ��
	_NOT_EQUAL,				// �� �����
	_EQUAL,					// �����
	_GREATER,				// ������
	_GREATER_OR_EQUAL,		// ������ ���� �����
	_LESS,					// ������
	_LESS_OR_EQUAL,			// ������ ���� �����
	_SUB,					// ��������
	_ADD,					// �����
	_INT,					// ����� ����� �� �������
	_MOD,					// ������� �� �������
	_DIV,					// �������
	_MUL,					// ���������
	_POW,					// ���������� � �������
	_ASSIGN,				// ����������
	_ARRID,					// ��������� �� ������� �������
	_ARRID_AND_ASSIGN,		// ���������� ������ �������� �������� �������, ������� �� �������
	_ARRINIT,				// ������������� �������
	_ARRACT,				// �������� ��� ��������
	_VARVAL,				// �������� ��������
	_OPERAND,				// ������� (�������������)
	_FUNCCALL,				// ����� �������
	_UMINUS,				// ������� �����
	__COUNTER,
};

/*
* ������ � ���� �������� ��������
*/
enum ValType
{
	_TRUE,		// ������
	_FALSE,		// ����
	_STRING,	// ������
	_NUMBER,	// ����� �����
	_NONE		// �������� None
};

/*
* ������ � ���� ���������� �� ���� ���� ���������
*/
struct ValInfo
{
	// ��� ���������
	enum ValType type;

	// �������� ���������-������
	char* stringVal;
	// �������� ���������-������ �����
	int intVal;
	// �������� ���������-���������� ����������
	bool logVal;

	// ������� � ����
	struct CodeLocation* loc;
};


/*
* ������ � ���� ���������� �� ���� ���� ���������
*/
struct ExprInfo
{
	// ����� ������� ���������
	struct ExprInfo * left;
	// ������ ������� ���������
	struct ExprInfo * right;
	// ��������� �� ��������� ������� (���������)
	struct ExprInfo * next;
	// ��������� ������� �������(��������� �������� _ARRID_AND_ASSIGN)
	struct ExprInfo * middle;
	// ��������� �� ������ ���������� ���������
	struct ExprListInfo * arglist;
	// ��� ���������
	enum ExprType type;

	// ��� ����������
	char* idName;
	// �������� ���������-����������
	struct ValInfo* exprVal;

	// ������� � ����
	struct CodeLocation* loc;
};
/*
* ������ � ���� ��������� �� ������ � ����� ������ ���������
*/
struct ExprListInfo
{
	struct ExprInfo * first;	// ������ ������
	struct ExprInfo * last;		// ����� ������
};

/*
* ������ � ���� ���������� � ������� ���������� ������� ��� �� ����������
*/
struct DefFuncParamInfo
{
	// ��������� �� ��������� ������� (���������)
	struct DefFuncParamInfo * next;

	// ��� ���������
	char* paramName;
	// �������� ���������
	struct ValInfo* paramVal;

	struct CodeLocation* loc;
};
/*
* ������ � ���� ��������� �� ������ � ����� ������ ���������� �������
*/
struct DefFuncParamListInfo
{
	struct DefFuncParamInfo * first;	// ������ ������
	struct DefFuncParamInfo * last;		// ����� ������
};
/*
* ������ � ���� ���������� � ������� ��� �� ����������
*/
struct FuncDefInfo
{
	// ��� �������
	char* functionName;

	// ������� ����� �������
	struct CodeLocation* nameLoc;

	// ���� ����, ������� �� � ������� ������������ ��������
	bool hasRet;

	// ��������� �������
	struct DefFuncParamListInfo* params;
	// ���� �������
	struct StmtListInfo* body;
};


/*
* ��� ����������
*/
enum StmtType
{
	_EXPR, // ���������
	_IF, // ��������
	_FOR, // ���� ���
	_WHILE, // ���� ����
	_FUNC_DEF, // ���������� �������
	_RETURN, // ����������� ��������
	_BREAK, // ���������
	_CONTINUE, // �����������
	_DEL // �������� �������� ���������� �� ������
};

/*
* ������ � ���� ���������� �� ���� ���� ����������
*/
struct StmtInfo
{
	// ��� ����������
	enum StmtType type;
	// ��������� �� ���������
	struct ExprInfo * expr;
	// ��������� �� ��������
	struct IfStmtInfo * ifstmt;
	// ��������� �� ���� for
	struct ForStmtInfo * forstmt;
	// ��������� �� ���� while
	struct WhileStmtInfo * whilestmt;
	// ��������� �� ���������� �������
	struct FuncDefInfo* funcdefstmt;
	// ��������� �� ��������� ���������
	struct StmtInfo * next;

	struct CodeLocation* loc;
};

/*
* ������ � ���� ���������� �� ������ � ����� ��������� �����
*/
struct StmtListInfo
{
	struct StmtInfo * first;
	struct StmtInfo * last;
};

/*
* ������ � ���� ���������� �� ���� ���� ��������
*/
struct IfStmtInfo
{
	// ��������� �� �������� ���������
	struct ExprInfo * expr;
	// ��������� �� ���� ��������
	struct StmtListInfo * stmtlist;
	// ��������� �� ���� elif
	struct ElifListInfo * eliflist;
	// ��������� �� ���� else(� ��� �� else if)
	// !!! ���� IfStmtInfo !!!
	struct StmtListInfo * elsestmtlist;
};

/*
* ������ � ���� ���������� �� ���� ���� elif
*/
struct ElifListInfo
{
	// �������� ���������
	struct ExprInfo * expr;
	// ���� ��������
	struct StmtListInfo * stmtlist;
	// ��������� �� ��������� elif
	struct ElifListInfo * next;
};

/*
* ������ � ���� ���������� �� ���� ���� while
*/
struct WhileStmtInfo
{
	// �������� ��������� ���������� �����
	struct ExprInfo * expr;
	// ���� �����
	struct StmtListInfo * stmtlist;
	// ��������� �� ���� ����� else ����� �����
	struct StmtListInfo * elsestmt;
};

/*
* ������ � ���� ���������� �� ���� ���� for
*/
struct ForStmtInfo
{
	// ������� �����(����������)
	char* counter;
	// ��������� �� ���������, �� ��� �������� ����
	struct ExprInfo * expr;
	// ���� �����
	struct StmtListInfo * stmtlist;
	// ��������� �� ���� ����� else ����� �����
	struct StmtListInfo * elsestmt;
};