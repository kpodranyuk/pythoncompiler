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
	// обращение по индексу массива
	_ARRID,
	// инициализация массива
	__ARRINIT,
	// действия над массивом
	__ARRACT,
	_VARVAL,	
	_OPERAND,
	_FUNCCALL
};
/*
* Хранит в себе информацию об узле типа выражения
*/
struct ExprInfo
{
	// Левый операнд выражения
	struct ExprInfo * left;
	// Правый операнд выражения
	struct ExprInfo * right;
	// Указатель на следующий элемент (выражение)
	struct ExprInfo * next;
	// Указатель на список аргументов выражения
	struct ExprListInfo * arglist;
	// Тип выражения
	enum ExprType type;

	// Значение выражения-строки
	char * opVal;
	// Значение выражения-целого числа
	int intVal;
	// Значение выражения-логической переменной
	bool logVal;
};
/*
* Хранит в себе указатель на начало и конец списка выражений
* (возможно следует изменить тип - хранить начало и следующий)
*/
struct ExprListInfo
{
	struct ExprInfo * first;
	struct ExprInfo * last;
};

/*
* Тип стейтмента
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
* Хранит в себе информацию об узле стейтмента
*/
struct StmtInfo
{
	// Тип стейтмента
	enum StmtType type;
	// Указатель на выражение
	struct ExprInfo * expr;
	// Указатель на развилку
	struct IfStmtInfo * ifstmt;
	// Указатель на цикл for
	struct ForStmtInfo * forstmt;
	// Указатель на цикл while
	struct WhileStmtInfo * whilestmt;
	// Указатель на объявление функции
	struct FuncDefStmtInfo * funcdefstmt;
	// Указатель на следующий стейтмент
	struct StmtInfo * next;
};

/*
* Хранит в себе информацию на начало и конец стейтмент листа
*/
struct StmtListInfo
{
	struct StmtInfo * first;
	struct StmtInfo * last;
};

/*
* Хранит в себе информацию об узле развилки
*/
struct IfStmtInfo
{
	// Указатель на условное выражение
	struct ExprInfo * expr;
	// Указатель на тело развилки
	struct StmtListInfo * stmtlist;
	// Указатель на стейтмент elif
	struct ElifStmtInfo * elifstmt;
	// Указатель на тело else
	struct StmtListInfo * elsestmtlist;
};

/*
* Хранит в себе информацию об узле elif
*/
struct ElifStmtInfo
{
	// Условное выражение
	struct ExprInfo * expr;
	// Тело развилки
	struct StmtListInfo * stmtlist;
	// Указатель на следующий elif
	struct ElifStmtInfo * next;
};