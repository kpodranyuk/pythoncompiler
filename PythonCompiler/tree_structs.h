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
	// ������������� �������
	// �������� ��� ��������
	_VARVAL,	
	_OPERAND,
	_FUNCCALL
};