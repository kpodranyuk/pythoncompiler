/*
* \file tables_struct.h �������� ����������� ��� ��������� ���� � ������ ���������
*/

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
	const int attrs = 1;	// ���������� ���������� ������
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
	const int stackSize = 1200;	// ������ ����� ���������
	int localVarsCount;	// ���������� ��������� ���������� ������
	int byteCodeLength;	// ����� ����-���� (������ ���� ������ � ������)
	const int exceptionsCount = 0;	// ���������� ������� � ������� ����������
	const int attrsCount = 0;		// ���������� ����������
};