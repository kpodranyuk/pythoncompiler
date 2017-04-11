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
	_ARRINIT,
	// действия над массивом
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
* Хранит в себе информацию об узле типа выражения
*/
struct ValInfo
{
	// Тип выражения
	enum ValType type;

	// Значение выражения-строки
	char* stringVal;
	// Значение выражения-целого числа
	int intVal;
	// Значение выражения-логической переменной
	bool logVal;
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
	//struct ExprInfo * next;
	// Указатель на список аргументов выражения
	struct ExprListInfo * arglist;
	// Тип выражения
	enum ExprType type;

	// Имя переменной
	char* idName;
	// Значение выражения-переменной
	struct ValInfo* exprVal;
};
/*
* Хранит в себе указатель на начало и конец списка выражений
* (возможно следует изменить тип - хранить начало и следующий)
*/
/*struct ExprListInfo
{
	struct ExprInfo * first;
	struct ExprInfo * last;
};*/

/*
* Тип стейтмента
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
* Хранит в себе информацию об узле типа стейтмента
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
* Хранит в себе информацию об узле типа развилки
*/
struct IfStmtInfo
{
	// Указатель на условное выражение
	struct ExprInfo * expr;
	// Указатель на тело развилки
	struct StmtListInfo * stmtlist;
	// Указатель на лист elif
	struct ElifListInfo * eliflist;
	// Указатель на тело else
	struct StmtListInfo * elsestmtlist;
};

/*
* Хранит в себе информацию об узле типа elif
*/
struct ElifListInfo
{
	// Условное выражение
	struct ExprInfo * expr;
	// Тело развилки
	struct StmtListInfo * stmtlist;
	// Указатель на следующий elif
	struct ElifListInfo * next;
};

/*
* Хранит в себе информацию об узле типа while
*/
struct WhileStmtInfo
{
	// Условное выражение выполнения цикла
	struct ExprInfo * expr;
	// Тело цикла
	struct StmtListInfo * stmtlist;
	// Указатель на тело блока else после цикла
	struct StmtListInfo * elsestmt;
};

/*
* Хранит в себе информацию об узле типа for
*/
struct ForStmtInfo
{
	// Счетчик цикла(переменная)
	char* counter;
	// Указатель на выражение, по чем проходит цикл
	struct ExprInfo * expr;
	// Тело цикла
	struct StmtListInfo * stmtlist;
	// Указатель на тело блока else после цикла
	struct StmtListInfo * elsestmt;
};