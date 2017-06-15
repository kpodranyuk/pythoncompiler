/*
* \file tables_struct.h Содержит необходимые для генерации кода и таблиц структуры
*/

#pragma once

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
	struct MethodTable_Elem * methodsFirst; // Указатель на первый метод класса(main)
	struct MethodTable_Elem * methodsLast; // Указатель на последний метод класса
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
		float val_float;	// Значение числа с плавающей точкой
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
/*struct FieldTable_List
{
	struct FieldTable_Elem* first;
	struct FieldTable_Elem* last;
	int count;
};*/

/*
* Хранит в себе информацию об элементе таблицы полей
*/
struct FieldTable_Elem
{
	static const unsigned short int access = FIELD_ACC_PUBLIC | FIELD_STATIC;	// Флаги доступа
	int fieldName;	// Ссылка на имя поля в таблице констант
	int fieldDesc;	// Ссылка на дескриптор поля в таблице констант
	int fieldRef;	// Ссылка на филдреф в таблице констант
	static const int attrs = 0;	// Количество аттрибутов
	struct FieldTable_Elem* next;	// Указатель на следующий элемент таблицы
};

/*
* Хранит в себе информацию о таблице полей класса
*/
/*struct MethodTable_List
{
	struct MethodTable_Elem* first;
	struct MethodTable_Elem* last;
	int count;
};*/

/*
* Хранит в себе информацию об элементе таблицы методов
*/
struct MethodTable_Elem
{
	static const unsigned short int access = METHOD_ACC_PUBLIC | METHOD_STATIC;	// Флаги доступа
	int methodName;	// Ссылка на имя поля в таблице констант
	int methodDesc;	// Ссылка на дескриптор поля в таблице констант
	int methodRef;	// Ссылка на методреф в таблице констант
	static const int attrs = 1;	// Количество аттрибутов метода
	int localVarsCount;	// Количество локальных переменных метода

	struct MethodTable_Elem* next;	// Указатель на следующий элемент таблицы
};

/*
* Хранит в себе возможный тип операции
*/
enum OperType
{
	__LDC = 18,				// Загружает на стек целую константу из таблицы констант. Аргумент - # на Constant_Integer - u1
	__LDC_W = 19,			// Загружает на стек дробную константу из таблицы констант. Аргумент - # на Constant_FLOAT - u1
	__ILOAD = 21,			// Загружает на стек целое число из таблицы лок-х переменных. Аргумент - # в таблице ЛП - u1
	__FLOAD = 23,			// Загружает на стек float из таблицы лок-х переменных. Аргумент - # в таблице ЛП - u1
	__ALOAD = 25,			// Загружает на стек ссылку из таблицы лок-х переменных. Аргумент - # в таблице ЛП - u1
	// -- Снимают со стека 1 значение --
	__ISTORE = 54,			// Сохраняет целое число с вершины стека в лок-ю переменную. Аргумент - # в таблице ЛП - u1
	__FSTORE = 56,			// Сохраняет float с вершины стека в лок-ю переменную. Аргумент - # в таблице ЛП - u1
	__ASTORE = 58,			// Сохраняет ссылку с вершины стека в лок-ю переменную. Аргумент - # в таблице ЛП - u1
	// --
	__POP = 87,				// Снимает с вершины стека содержащееся там значение
	__DUP = 89,				// Дублирует на вершине стека содержащееся там значение
	__GET_STATIC = 178,		// Получает значение статического поля класса. Аргумент - филдреф из таблицы констант - u2
	// -- Снимает со стека 1 значение --
	__PUT_STATIC = 179,		// Устанавливает значение статического поля класса. Аргумент - филдреф из таблицы констант - u2
	// -- Снимает со стека столько значений, сколько аргументов у метода --
	__INVOKESTATIC = 184,	// Вызывает статический метод класса. Аргумент - методреф из таблицы констант - u2. Кладет на стек результат
	// --
	// -- Снимает со стека 1 значение --
	__FRETURN = 174,		// Завершает выполнение метода с возвращением float
	__IRETURN = 172,		// Завершает выполнение метода с возвращением целого числа
	__ARETURN = 176,		// Завершает выполнение метода с возвращением ссылки на объект
	// --
	__RETURN = 177,			// Завершает выполнение текущего метода (возвращает void)
	// -- Снимает со стека 1 значение --
	__IF_EQ = 153,			// Условный переход, если условие верно. Аргумент - смещение s2
	// --
	__GOTO = 167			// Безусловный переход. Аргумент - смещение s2
};

/*
* Хранит в себе возможный тип операции
*/
enum LibOperations
{
	___TO_STRING=20,
	___PRINT=24,
	___INPUT=27,
	___TO_NUMBER=30,
	___TO_NUMBER_base=34,
	___SUB=37,
	___ADD=40,
	___MUL=43,
	___POW=46,
	___DIV=49,
	___MOD=52,
	___TO_INT_BOOL=56,
	___LIST_GET=59,
	___LIST_SET=63,
	___LIST_APPEND=66,
	___LIST_ADD_INITIALIZE=69,
	___LIST_REMOVE=72,
	___EQ=75,
	___LESS=78,
	___MORE=81,
	___LESS_OR_EQ=84,
	___MORE_OR_EQ=87,
	___NOT=90,
	___OR=93,
	___AND=96,
	___NOT_EQ=99,
	___VALUE_FROM_INT=103,
	___VALUE_FROM_NONE=107,
	___VALUE_FROM_LIST=110,
	___VALUE_FROM_BOOLEAN=113,
	___VALUE_FROM_FLOAT=117
};


/*
* Хранит описание операции
*/
struct Operation
{
	enum OperType type; // Тип операции

	/*Аргументы операции(каждая имеет свое значение или вовсе не имеет)*/
	unsigned char u1;
	unsigned short int u2;
	short int s2;

	int countByte; // Сколько данная операция вместе с ее аргументом занимает байт
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