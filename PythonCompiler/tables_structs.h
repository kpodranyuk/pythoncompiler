/*
* \file tables_struct.h �������� ����������� ��� ��������� ���� � ������ ���������
*/
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
	//int list;
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
	struct MethodTable_Elem * methodsfirst; // ��������� �� ������ ����� ������(main)
	struct MethodTable_Elem * methodslast; // ��������� �� ��������� ����� ������
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
    } value;

	int numberInTable; // ����� ��������� � �������
    
    struct ConstTable_Elem * next;	// ��������� �� ��������� ������� �������
};

/*
* ������ � ���� ���������� �� �������� ������� �����
*/
struct FieldTable_Elem
{
	int fieldName;	// ������ �� ��� ���� � ������� ��������
	int fieldDesc;	// ������ �� ���������� ���� � ������� ���������

	struct FieldTable_Elem* next;	// ��������� �� ��������� ������� �������
};

/*
* ������ � ���� ���������� �� �������� ������� �������
*/
struct MethodTable_Elem
{
	int methodName;	// ������ �� ��� ���� � ������� ��������
	int methodDesc;	// ������ �� ���������� ���� � ������� ���������
	static const int attrs = 1;	// ���������� ���������� ������
	//TODO ������� ������ �� ������� ���������� ������
	struct CodeAttr* methodAttr;
	struct MethodTable_Elem* next;	// ��������� �� ��������� ������� �������
};

/*
* ������ � ���� ���������� �� ��������� ���� ������
*/
struct CodeAttr
{
	int attrName;	// ������ �� �������� Code � ������� ��������
	int attrLength;	// ����� �������� (����� ������ 6 ����, ����������� ��� � �����)
	static const int stackSize = 1200;	// ������ ����� ���������
	int localVarsCount;	// ���������� ��������� ���������� ������
	int byteCodeLength;	// ����� ����-���� (������ ���� ������ � ������)
	static const int exceptionsCount = 0;	// ���������� ������� � ������� ����������
	static const int attrsCount = 0;		// ���������� ����������
};

/*
* ������ � ���� ��������� ��� ��������
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
* ������ �������� ��������
*/
struct Operation
{
	enum OperType type; // ��� ��������
	/*����������, ������������ ��������*/
	unsigned char u1;
	unsigned short int u2;
	unsigned int u4;
	short int s2;
	int s4;

	struct Operation * next; // ��������� �� ��������� ���������
};

/*
* ������ �������� ����������
*/
struct Variable
{
	char * name; // ���
	int num; // ����� � ������� ��������� ���������� ������
	static const int type = 1; // ��� Value

	struct Variable * next; // ��������� ����������
};