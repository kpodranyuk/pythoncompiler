enum ExprType
{
	_OR,
	_AND,
	_NOT,
	_NOT_EQUAL,
	_EQUAL,
	_GREATER,
	_GREATER_OR_EQUAL,
	_LESS,
	_LESS_OR_EQUAL,
	_SUB,
	_ADD,
	_INT,
	_MOD,
	_DIV,
	_MUL,
	_POW,
	_ASSIGN,
	// ��������� �� ������� �������
	_ARRID,
	// ������������� �������
	__ARRINIT,
	// �������� ��� ��������
	__ARRACT,
	_VARVAL,	
	_OPERAND,
	_FUNCCALL
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
	// ��������� �� ������ ���������� ���������
	struct ExprListInfo * arglist;
	// ��� ���������
	enum ExprType type;

	// �������� ���������-������
	char * opVal;
	// �������� ���������-������ �����
	int intVal;
	// �������� ���������-���������� ����������
	bool logVal;
};
/*
* ������ � ���� ��������� �� ������ � ����� ������ ���������
* (�������� ������� �������� ��� - ������� ������ � ���������)
*/
struct ExprListInfo
{
	struct ExprInfo * first;
	struct ExprInfo * last;
};

/*
* ��� ����������
*/
enum StmtType
{
	__EXPR,
	__IF,
	__FOR,
	__WHILE,
	__FUNC_DEF,
	__RETURN,
	__BREAK,
	__CONTINUE,
	__DEL
};

/*
* ������ � ���� ���������� �� ���� ����������
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
	struct FuncDefStmtInfo * funcdefstmt;
	// ��������� �� ��������� ���������
	struct StmtInfo * next;
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
* ������ � ���� ���������� �� ���� ��������
*/
struct IfStmtInfo
{
	// ��������� �� �������� ���������
	struct ExprInfo * expr;
	// ��������� �� ���� ��������
	struct StmtListInfo * stmtlist;
	// ��������� �� ��������� elif
	struct ElifStmtInfo * elifstmt;
	// ��������� �� ���� else
	struct StmtListInfo * elsestmtlist;
};

/*
* ������ � ���� ���������� �� ���� elif
*/
struct ElifStmtInfo
{
	// �������� ���������
	struct ExprInfo * expr;
	// ���� ��������
	struct StmtListInfo * stmtlist;
	// ��������� �� ��������� elif
	struct ElifStmtInfo * next;
};