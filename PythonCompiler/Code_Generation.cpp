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
	u2=htons(0);
	_write(this->fileDesc, (void*)&u2, 2);
	u2=htons(52);
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
	while(elem!=this->ct->last)
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
	while(field!=prog->lastField)
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