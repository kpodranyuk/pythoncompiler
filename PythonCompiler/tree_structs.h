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
	_ARRINIT,
	// �������� ��� ��������
	_ARRACT,
	_VARVAL,	
	_OPERAND,
	_FUNCCALL
};

enum ValType
{
	_TRUE,
	_FALSE,
	_STRING,
	_NUMBER
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
	//struct ExprInfo * next;
	// ��������� �� ������ ���������� ���������
	struct ExprListInfo * arglist;
	// ��� ���������
	enum ExprType type;

	// ��� ����������
	char* idName;
	// �������� ���������-����������
	struct ValInfo* exprVal;
};
/*
* ������ � ���� ��������� �� ������ � ����� ������ ���������
* (�������� ������� �������� ��� - ������� ������ � ���������)
*/
/*struct ExprListInfo
{
	struct ExprInfo * first;
	struct ExprInfo * last;
};*/

/*
* ��� ����������
*/
enum StmtType
{
	_EXPR,
	_IF,
	_FOR,
	_WHILE,
	_FUNC_DEF,
	_RETURN,
	_BREAK,
	_CONTINUE,
	_DEL
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
	// ��������� �� ���� else
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