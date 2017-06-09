/*
* \file tables_struct.h Содержит необходимые для генерации кода и таблиц структуры
*/
/*
* Хранит в себе ссылки основных элементов таблицы констант
*/
struct ConstTable_Consts
{
	int* constnumber;	// Счетчик констант в таблице констант
	int objectClass;	// Номер класса Code
	int rtlClass;		// Номер класса rtl
	int valueType;		// Номер класса value
	int valueClass;		// Номер первой операции-обертки конструктора
	int arifmetic;		// Номер первой арифметической операции
	int list;
	int logic;			// Номер первой логической операции
	int nameconstid;	// Номер метода main
	int init;			// Номер строки с конструктором по умолчанию
	int descId;			// Номер строки с основным типом LValue;
};
/*
* Хранить описание класса
*/
struct ClassTable_Elem
{
	int classConst; // Номер класса в таблице констант
	int fieldCount; // Количество полей класса
	int methodCount; // Количество методов в классе
	struct FieldTable_Elem * firstField; // Указатель на первое поле класса
	struct FieldTable_Elem * lastField; // Указатель на последнее поле класса
	struct MethodTable_Elem * methodsfirst; // Указатель на первый метод класса(main)
	struct MethodTable_Elem * methodslast; // Указатель на последний метод класса
};


/*
* Хранит в себе возможный тип константы
*/
enum ConstType
{
    CONST_UTF8      = 1,    
    CONST_INT       = 3,
    CONST_FLOAT     = 4,
    CONST_CLASS     = 7,
    CONST_STRING    = 8,
    CONST_FIELDREF  = 9,
    CONST_METHODREF = 10,
    CONST_NAMETYPE  = 12
};

/*
* Хранит в себе информацию о таблице констант
*/
struct ConstTable_List
{
	struct ConstTable_Elem* first;
	struct ConstTable_Elem* last;
};

/*
* Хранит в себе информацию об элементе таблицы констант
*/
struct ConstTable_Elem
{
    enum ConstType type;	// Тип константы - u1
	/*
	* Хранит в себе значение константы
	*/
    union 
	{
        char * utf8;	// Значение utf-8
		/*
		* Хранит в себе значение аргументов для ссылок на имя и тип
		*/
        struct 
		{
            int arg1;	// Первый аргумент ссылки (на имя)
            int arg2;	// Второй аргумент ссылки (на тип)
        } args;

        int val_int;	// Значение целого числа
    } value;

	int numberInTable; // Номер константы в таблице
    
    struct ConstTable_Elem * next;	// Указатель на следующий элемент таблицы
};

/*
* Хранит в себе влаги доступа для поля
*/
enum Field_ACC {
	FIELD_ACC_PUBLIC = 0x0001,
	FIELD_ACC_PRIVATE = 0x0002,
	FIELD_ACC_PROTECTED = 0x0004,

	FIELD_STATIC = 0x0008
};

/*
* Хранит в себе флаги доступа для метода класса
*/
enum Method_ACC {
	METHOD_ACC_PUBLIC = 0x0001,
	METHOD_ACC_PRIVATE = 0x0002,
	METHOD_ACC_PROTECTED = 0x0004,

	METHOD_STATIC = 0x0008,
	METHOD_FINAL = 0x0010,
	METHOD_ABSTRACT = 0x0400
};

/*
* Хранит в себе информацию о таблице полей класса
*/
struct FieldTable_List
{
	struct FieldTable_Elem* first;
	struct FieldTable_Elem* last;
};

/*
* Хранит в себе информацию об элементе таблицы полей
*/
struct FieldTable_Elem
{
	static const unsigned short int access = FIELD_ACC_PUBLIC | FIELD_STATIC;	// Флаги доступа
	int fieldName;	// Ссылка на имя поля в таблице констант
	int fieldDesc;	// Ссылка на дескриптор поля в таблице константс
	static const int attrs = 0;	// Количество аттрибутов
	struct FieldTable_Elem* next;	// Указатель на следующий элемент таблицы
};

/*
* Хранит в себе информацию о таблице полей класса
*/
struct MethodTable_List
{
	struct MethodTable_Elem* first;
	struct MethodTable_Elem* last;
};

/*
* Хранит в себе информацию об элементе таблицы методов
*/
struct MethodTable_Elem
{
	static const unsigned short int access = METHOD_ACC_PUBLIC | METHOD_STATIC;	// Флаги доступа
	int methodName;	// Ссылка на имя поля в таблице констант
	int methodDesc;	// Ссылка на дескриптор поля в таблице константс
	static const int attrs = 1;	// Количество аттрибутов метода
	//TODO сделать ссылку на таблицу аттрибутов метода
	struct CodeAttr* methodAttr;
	struct MethodTable_Elem* next;	// Указатель на следующий элемент таблицы
};

/*
* Хранит в себе информацию об аттрибуте кода метода
*/
struct CodeAttr
{
	int attrName;	// Ссылка на аттрибут Code в таблице констант
	int attrLength;	// Длина атрибута (кроме первых 6 байт, описывающих имя и длину)
	static const int stackSize = 1200;	// Размер стека операндов
	int localVarsCount;	// Количество локальных переменных метода
	int byteCodeLength;	// Длина байт-кода (размер кода метода в байтах)
	static const int exceptionsCount = 0;	// Количество записей в таблице исключений
	static const int attrsCount = 0;		// Количество аттрибутов
};

/*
* Хранит в себе возможный тип операции
*/
enum OperType
{
	__INVOKESTATIC = 184,
	__RETURN = 177,
	__LDC = 18,
	__ILOAD = 21,
	__FLOAD = 23,
	__IF_ICMPNE = 160,
	__GOTO = 167
};

/*
* Хранит описание операции
*/
struct Operation
{
	enum OperType type; // Тип операции
	/*Переменные, оперделенных размеров*/
	unsigned char u1;
	unsigned short int u2;
	unsigned int u4;
	short int s2;
	int s4;

	struct Operation * next; // Указатель на следующую константу
};

/*
* Хранит в себе информацию о таблице локальных переменных методов
*/
struct VariableTable_List
{
	struct Variable* first;
	struct Variable* last;
	int count;
};

/*
* Хранит описание переменной
*/
struct Variable
{
	char * name; // Имя
	int num; // Номер в таблице локальных переменных метода
	static const int type = 1; // Наш Value
	int localFor;	// Номер методРефа, которому принадлежит функция
	struct Variable * next; // Следующая переменная
};