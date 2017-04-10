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
	// инициализация массива
	// действия над массивом
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