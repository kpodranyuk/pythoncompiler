#include "code_generation_.h"


CodeGeneration::CodeGeneration(struct ClassTable_Elem* prog, struct ConstTable_List* ct, struct ConstTable_Consts* ct_consts, struct VariableTable_List* vars)
{
	this->prog=prog;
	this->ct=ct;
	this->ct_consts=ct_consts;
	this->vars=vars;
}

CodeGeneration::~CodeGeneration(void)
{
}


void CodeGeneration::generateCode(struct StmtListInfo* treeRoot)
{
	this->treeRoot=treeRoot;

	/*�������� �����*/
	this->fileDesc=_open("Program.class",  O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 644);
	
	/*������ ����������� �����*/
	u4=htonl(0xCAFEBABE);
	_write(this->fileDesc, (void*)&u4, 4);

	/*������ ��������� � ������*/
	u2=htons(3);
	_write(this->fileDesc, (void*)&u2, 2);
	u2=htons(45);
	_write(this->fileDesc, (void*)&u2, 2);

	/*��������� ������� ��������*/
	generateConstsTable();

	/*��������� ������ �������*/
	u2=htons(33); //ACC_PUBLIC | ACC_SUPER
	_write(this->fileDesc,(void*)&u2, 2);

	/*������� �����*/
	u2=htons(prog->classConst);
	_write(this->fileDesc,(void*)&u2, 2);

	/*�����-��������*/
	u2=htons(ct_consts->objectClass);
	_write(this->fileDesc,(void*)&u2, 2);

	/*���-�� �����������*/
	u2=htons(0);
	_write(this->fileDesc,(void*)&u2, 2);

	/*��������� ������� ����� ������*/
	generateFieldsTable();

	/*��������� ������� ������� ������*/
	generateMethodsTable();

	/*���-�� ��������� ������*/
	u2=htons(0);
	_write(this->fileDesc,(void*)&u2, 2);

	/*�������� �����*/
	_close(this->fileDesc);
}


void CodeGeneration::generateConstsTable()
{
	// ���������� ��-�� ��������� � ������� ��������
	int count=1;
	ConstTable_Elem* elem=this->ct->first;

	while(elem!=this->ct->last)
	{
		elem=elem->next;
		count++;
	}
	
	// �������� ��� ���-��+1(�� ���������)
	u2=htons(count+1);
	_write(this->fileDesc,(void*)&u2, 2);

	// ��� ������ ��������� ������������� ����-���
	elem=this->ct->first;
	for(int i = 0; i < count; ++i)
	{
		// ����� ��� ���������
		u1=elem->type;
		_write(this->fileDesc,(void*)&u1, 1);

		// ��� ������� ���� ���� ������ ������
		if(elem->type==CONST_UTF8)
		{
			u2=htons(strlen(elem->value.utf8));
			_write(this->fileDesc,(void*)&u2, 2);// ����� ������ � ������
			_write(this->fileDesc,(void*)elem->value.utf8, strlen(elem->value.utf8));// ������� ������ � ���� ������������������ ����
		}
		else if(elem->type==CONST_INT)
		{
			s4=htonl(elem->value.val_int);
			_write(this->fileDesc,(void*)&s4, 4);// ����� ����� �� ������
		}
		// �� �������
		else if(elem->type==CONST_FLOAT)
		{
			sf4 = reverseFloatBytes(elem->value.val_float);
            _write(this->fileDesc,(void *)&sf4, 4);
		}
		else if(elem->type==CONST_CLASS || elem->type==CONST_STRING)
		{
			u2=htons(elem->value.args.arg1);
			_write(this->fileDesc,(void*)&u2, 2);// ����� ��������� utf-8, ���������� ���
		}
		else if(elem->type==CONST_FIELDREF || elem->type==CONST_METHODREF || elem->type==CONST_NAMETYPE)
		{
			u2=htons(elem->value.args.arg1);
			_write(this->fileDesc,(void*)&u2, 2);// ����� ��������� utf-8, ���������� ���
			u2=htons(elem->value.args.arg2);
			_write(this->fileDesc,(void*)&u2, 2);// ����� ��������� utf-8, ���������� ����������
		}
		else
			throw "UNKNOWN CONSTANT TYPE";

		elem=elem->next;
	}
}


void CodeGeneration::generateFieldsTable()
{
	// ���������� ���-�� ����� ������
	u2=htons(prog->fieldCount);
	_write(this->fileDesc,(void*)&u2, 2);

	FieldTable_Elem* field=prog->firstField;
	// ��� ������� ����
	for (int i = 0; i < prog->fieldCount; ++i)
	{
		u2=htons(field->access);
		_write(this->fileDesc,(void*)&u2, 2);// ����� �������

		u2=htons(field->fieldName);
		_write(this->fileDesc,(void*)&u2, 2);// ����� ��������� utf-8, ���������� ��� ����
		
		u2=htons(field->fieldDesc);
		_write(this->fileDesc,(void*)&u2, 2);// ����� ��������� utf-8, ���������� ���������� ����

		u2=htons(field->attrs);
		_write(this->fileDesc,(void*)&u2, 2);// ���-�� ���������(0)

		field=field->next;
	}
}


void CodeGeneration::generateMethodsTable()
{
	// ���������� ���-�� ������� ������
	u2=htons(prog->methodCount);
	_write(this->fileDesc,(void*)&u2, 2);

	// ��� ������ ������ ������� ����-���
	MethodTable_Elem* method = prog->methodsFirst;
	while(method!=NULL)
	{
		currentLocal=method->methodRef;

		u2=htons(method->access);
		_write(this->fileDesc,(void*)&u2, 2);// ����� �������

		u2=htons(method->methodName);
		_write(this->fileDesc,(void*)&u2, 2);// ����� ��������� utf-8, ���������� ��� ������

		u2=htons(method->methodDesc);
		_write(this->fileDesc,(void*)&u2, 2);// ����� ��������� utf-8, ���������� ���������� ������

		u2=htons(method->attrs);
		_write(this->fileDesc,(void*)&u2, 2);// ���-�� ���������(1)

		/* ����� ��������� � ���������� ������� Code ������*/
		u2=htons(1); //1� ��������� Code(��� ��������)
		_write(this->fileDesc,(void*)&u2, 2);

		// ������� � ������ ���� ����-��� ������(��������)
		generateCodeForStatementList(this->treeRoot);
		// �������� ������ ����������� ����-����
		int length=getCodeLengthMethod();

		u4=htonl(length+12);
		_write(this->fileDesc,(void*)&u4, 4);// ����� ��������

		u2=htons(1200);
		_write(this->fileDesc,(void*)&u2, 2);// ������ ����� ���������

		u2=htons(0);// ���� ��� 0, ��������� �������� ��� �� ����������
		_write(this->fileDesc,(void*)&u2, 2);// ���������� ��������� ����������

		u4=htonl(length);
		_write(this->fileDesc,(void*)&u4, 4);// ����� ����-����
		writeByteCode();// ��� ����-���  

		u2=htons(0);
		_write(this->fileDesc,(void*)&u2, 2);// ���������� ����������

		u2=htons(0);
		_write(this->fileDesc,(void*)&u2, 2);// ���������� ���������


		method=method->next;
	}
}


void CodeGeneration::generateCodeForStatementList(struct StmtListInfo* stmtList)
{
}


void CodeGeneration::generateCodeForExpr(struct ExprInfo * expr)
{
}


void CodeGeneration::generateCodeForIfStmt(struct IfStmtInfo * ifstmt)
{
}


void CodeGeneration::generateCodeForWhileStmt(struct WhileStmtInfo * whilestmt)
{
}


void CodeGeneration::generateCodeForForStmt(struct ForStmtInfo * forstmt)
{
}


void CodeGeneration::generateCodeForContinueBreakStmt(struct StmtInfo* contBreakStmt)
{
}


void CodeGeneration::generateCodeForDelStmt(struct ExprInfo * expr)
{
}


void CodeGeneration::generateCodeForFuncDef(struct FuncDefInfo * funcDef)
{
}


int CodeGeneration::getCodeLengthMethod()
{
	int length=0;

	return length;
}

void CodeGeneration::writeByteCode()
{

float CodeGeneration::reverseFloatBytes(float f)
{
	float retVal;
	char *floatToConvert = ( char* ) & f;
	char *returnFloat = ( char* ) & retVal;

	// swap the bytes into a temporary buffer
	returnFloat[0] = floatToConvert[3];
	returnFloat[1] = floatToConvert[2];
	returnFloat[2] = floatToConvert[1];
	returnFloat[3] = floatToConvert[0];

	return retVal;
}