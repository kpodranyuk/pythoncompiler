/*
* \file tables_struct.h �������� ����������� ��� ��������� ���� � ������ ���������
*/

#pragma once

/*
* ������ � ���� ������ �������� ��������� ������� ��������
*/
struct ConstTable_Consts
{
	int* constnumber;	// ������� �������� � ������� ��������
	int objectClass;	// ����� ������ Code
	int rtlClass;		// ����� ������ rtl
	int valueType;		// ����� ������ value
	int valueClass;		// ����� ������ ��������-������� ������������
	int arifmetic;		// ����� ������ �������������� ��������
	int list;
	int logic;			// ����� ������ ���������� ��������
	int nameconstid;	// ����� ������ main
	int init;			// ����� ������ � ������������� �� ���������
	int descId;			// ����� ������ � �������� ����� LValue;
};
/*
* ������� �������� ������
*/
struct ClassTable_Elem
{
	int classConst; // ����� ������ � ������� ��������
	int fieldCount; // ���������� ����� ������
	int methodCount; // ���������� ������� � ������
	struct FieldTable_Elem * firstField; // ��������� �� ������ ���� ������
	struct FieldTable_Elem * lastField; // ��������� �� ��������� ���� ������
	struct MethodTable_Elem * methodsFirst; // ��������� �� ������ ����� ������(main)
	struct MethodTable_Elem * methodsLast; // ��������� �� ��������� ����� ������
};


/*
* ������ � ���� ��������� ��� ���������
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
* ������ � ���� ���������� � ������� ��������
*/
struct ConstTable_List
{
	struct ConstTable_Elem* first;
	struct ConstTable_Elem* last;
};

/*
* ������ � ���� ���������� �� �������� ������� ��������
*/
struct ConstTable_Elem
{
    enum ConstType type;	// ��� ��������� - u1
	/*
	* ������ � ���� �������� ���������
	*/
    union 
	{
        char * utf8;	// �������� utf-8
		/*
		* ������ � ���� �������� ���������� ��� ������ �� ��� � ���
		*/
        struct 
		{
            int arg1;	// ������ �������� ������ (�� ���)
            int arg2;	// ������ �������� ������ (�� ���)
        } args;

        int val_int;	// �������� ������ �����
		float val_float;	// �������� ����� � ��������� ������
    } value;

	int numberInTable; // ����� ��������� � �������
    
    struct ConstTable_Elem * next;	// ��������� �� ��������� ������� �������
};

/*
* ������ � ���� ����� ������� ��� ����
*/
enum Field_ACC {
	FIELD_ACC_PUBLIC = 0x0001,
	FIELD_ACC_PRIVATE = 0x0002,
	FIELD_ACC_PROTECTED = 0x0004,

	FIELD_STATIC = 0x0008
};

/*
* ������ � ���� ����� ������� ��� ������ ������
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
* ������ � ���� ���������� � ������� ����� ������
*/
/*struct FieldTable_List
{
	struct FieldTable_Elem* first;
	struct FieldTable_Elem* last;
	int count;
};*/

/*
* ������ � ���� ���������� �� �������� ������� �����
*/
struct FieldTable_Elem
{
	static const unsigned short int access = FIELD_ACC_PUBLIC | FIELD_STATIC;	// ����� �������
	int fieldName;	// ������ �� ��� ���� � ������� ��������
	int fieldDesc;	// ������ �� ���������� ���� � ������� ��������
	int fieldRef;	// ������ �� ������� � ������� ��������
	static const int attrs = 0;	// ���������� ����������
	struct FieldTable_Elem* next;	// ��������� �� ��������� ������� �������
};

/*
* ������ � ���� ���������� � ������� ����� ������
*/
/*struct MethodTable_List
{
	struct MethodTable_Elem* first;
	struct MethodTable_Elem* last;
	int count;
};*/

/*
* ������ � ���� ���������� �� �������� ������� �������
*/
struct MethodTable_Elem
{
	static const unsigned short int access = METHOD_ACC_PUBLIC | METHOD_STATIC;	// ����� �������
	int methodName;	// ������ �� ��� ���� � ������� ��������
	int methodDesc;	// ������ �� ���������� ���� � ������� ��������
	int methodRef;	// ������ �� �������� � ������� ��������
	static const int attrs = 1;	// ���������� ���������� ������
	int localVarsCount;	// ���������� ��������� ���������� ������

	struct MethodTable_Elem* next;	// ��������� �� ��������� ������� �������
};

/*
* ������ � ���� ��������� ��� ��������
*/
enum OperType
{
	__LDC = 18,				// ��������� �� ���� ����� ��������� �� ������� ��������. �������� - # �� Constant_Integer - u1
	__LDC_W = 19,			// ��������� �� ���� ������� ��������� �� ������� ��������. �������� - # �� Constant_FLOAT - u1
	__ILOAD = 21,			// ��������� �� ���� ����� ����� �� ������� ���-� ����������. �������� - # � ������� �� - u1
	__FLOAD = 23,			// ��������� �� ���� float �� ������� ���-� ����������. �������� - # � ������� �� - u1
	__ALOAD = 25,			// ��������� �� ���� ������ �� ������� ���-� ����������. �������� - # � ������� �� - u1
	// -- ������� �� ����� 1 �������� --
	__ISTORE = 54,			// ��������� ����� ����� � ������� ����� � ���-� ����������. �������� - # � ������� �� - u1
	__FSTORE = 56,			// ��������� float � ������� ����� � ���-� ����������. �������� - # � ������� �� - u1
	__ASTORE = 58,			// ��������� ������ � ������� ����� � ���-� ����������. �������� - # � ������� �� - u1
	// --
	__POP = 87,				// ������� � ������� ����� ������������ ��� ��������
	__DUP = 89,				// ��������� �� ������� ����� ������������ ��� ��������
	__GET_STATIC = 178,		// �������� �������� ������������ ���� ������. �������� - ������� �� ������� �������� - u2
	// -- ������� �� ����� 1 �������� --
	__PUT_STATIC = 179,		// ������������� �������� ������������ ���� ������. �������� - ������� �� ������� �������� - u2
	// -- ������� �� ����� ������� ��������, ������� ���������� � ������ --
	__INVOKESTATIC = 184,	// �������� ����������� ����� ������. �������� - �������� �� ������� �������� - u2. ������ �� ���� ���������
	// --
	// -- ������� �� ����� 1 �������� --
	__FRETURN = 174,		// ��������� ���������� ������ � ������������ float
	__IRETURN = 172,		// ��������� ���������� ������ � ������������ ������ �����
	__ARETURN = 176,		// ��������� ���������� ������ � ������������ ������ �� ������
	// --
	__RETURN = 177,			// ��������� ���������� �������� ������ (���������� void)
	// -- ������� �� ����� 1 �������� --
	__IF_EQ = 153,			// �������� �������, ���� ������� �����. �������� - �������� s2
	// --
	__GOTO = 167			// ����������� �������. �������� - �������� s2
};

/*
* ������ � ���� ��������� ��� ��������
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
* ������ �������� ��������
*/
struct Operation
{
	enum OperType type; // ��� ��������

	/*��������� ��������(������ ����� ���� �������� ��� ����� �� �����)*/
	unsigned char u1;
	unsigned short int u2;
	short int s2;

	int countByte; // ������� ������ �������� ������ � �� ���������� �������� ����
};

/*
* ������ � ���� ���������� � ������� ��������� ���������� �������
*/
struct VariableTable_List
{
	struct Variable* first;
	struct Variable* last;
	int count;
};

/*
* ������ �������� ����������
*/
struct Variable
{
	char * name; // ���
	int num; // ����� � ������� ��������� ���������� ������
	static const int type = 1; // ��� Value
	int localFor;	// ����� ���������, �������� ����������� �������
	struct Variable * next; // ��������� ����������
};