#include "code_generation_.h"


CodeGeneration::CodeGeneration(struct ClassTable_Elem* prog, struct ConstTable_List* ct, struct ConstTable_Consts* ct_consts, struct VariableTable_List* vars)
{
	this->prog=prog;
	this->ct=ct;
	this->ct_consts=ct_consts;
	this->vars=vars;
	stackSize=0;
	needCountInStack=0;
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
	u2=htons(prog->methodCount+1);
	_write(this->fileDesc,(void*)&u2, 2);

	// �����������
	u2=htons(1); // ���� acc_public
	_write(this->fileDesc,(void*)&u2, 2);

	u2=htons(this->ct_consts->objectClass + 1);
	_write(this->fileDesc,(void*)&u2, 2);// ��� ������

	u2=htons(this->ct_consts->objectClass + 2);
	_write(this->fileDesc,(void*)&u2, 2);// ����������

	u2=htons(1); // 1 
	_write(this->fileDesc,(void*)&u2, 2);// ���������� ���������

	u2=htons(1); //1� ��������� Code
	_write(this->fileDesc,(void*)&u2, 2);// ��� ��������

	u4=htonl(5 + 12);
	_write(this->fileDesc,(void*)&u4, 4);// ����� ��������

	u2=htons(2);
	_write(this->fileDesc,(void*)&u2, 2);// ������������ ������ ����� ���������

	u2=htons(1);
	_write(this->fileDesc,(void*)&u2, 2);// ���������� ��������� ����������

	u4=htonl(5);
	_write(this->fileDesc,(void*)&u4, 4);// ����� ����

	// ���
	u1=42;
	_write(this->fileDesc,(void*)&u1, 1);
	u1=183;
	_write(this->fileDesc,(void*)&u1, 1);
	u2=htons(this->ct_consts->objectClass + 4);
	_write(this->fileDesc,(void*)&u2, 2);
	u1=177;
	_write(this->fileDesc,(void*)&u1, 1);

	u2=0;
	_write(this->fileDesc,(void*)&u2, 2);// ����� ������� ������������ ����������

	u2=0;
	_write(this->fileDesc,(void*)&u2, 2);// ���������� ���������


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

		// ������� � ������ ���� ����-��� ������
		generateCodeForStatementList(this->treeRoot);
		// ��� ����� ���� ������� ������ ��� ���������� ���������� �������� ������
		if(method->methodRef==NULL)
		{
			Operation* ret=new Operation;
			ret->type=__RETURN;
			ret->countByte=1;
			oper.push_back(ret);
		}
		// �������� ������ ����������� ����-����
		int length=getCodeLengthMethod();

		u4=htonl(length+12);
		_write(this->fileDesc,(void*)&u4, 4);// ����� ��������

		u2=htons(1000);
		_write(this->fileDesc,(void*)&u2, 2);// ������ ����� ���������

		u2=htons(method->localVarsCount+1);
		_write(this->fileDesc,(void*)&u2, 2);// ���������� ��������� ����������

		u4=htonl(length);
		_write(this->fileDesc,(void*)&u4, 4);// ����� ����-����
		writeByteCode();// ��� ����-��� 
		oper.clear();

		u2=htons(0);
		_write(this->fileDesc,(void*)&u2, 2);// ���������� ����������

		u2=htons(0);
		_write(this->fileDesc,(void*)&u2, 2);// ���������� ���������


		method=method->next;
	}
}


void CodeGeneration::generateCodeForStatementList(struct StmtListInfo* stmtList)
{
	StmtInfo* begining = stmtList->first;
	// ���� ������� ������� ������ �� ���������..
	while(begining!=NULL)
	{
		if(begining->localFor==currentLocal)
		{
			if(begining->type==_EXPR)
			{
				generateCodeForExpr(begining->expr,false);
				if(begining->expr->type!=_ASSIGN && begining->expr->type!=_ARRID_AND_ASSIGN && begining->expr->type!=_ARRACT /*&& begining->expr->type!=_FUNCCALL*/)
				{
					Operation* pop=new Operation;
					pop->type=__POP;
					pop->countByte=1;
					oper.push_back(pop);
				}
			}
			else if(begining->type==_IF)
			{
				generateCodeForIfStmt(begining->ifstmt);
			}
			else if(begining->type==_FOR)
			{
				generateCodeForForStmt(begining->forstmt);
			}
			else if(begining->type==_WHILE)
			{
				generateCodeForWhileStmt(begining->whilestmt);
			}
			else if(begining->type==_FUNC_DEF)
			{
				//generateCodeForFuncDef(begining->funcdefstmt);
				generateCodeForStatementList(begining->funcdefstmt->body);
			}
			else if(begining->type==_RETURN)
			{
				generateCodeForReturnStmt(begining->expr);
			}
			else if(begining->type==_BREAK || begining->type==_CONTINUE)
			{
				generateCodeForContinueBreakStmt(begining);
			}
			else if(begining->type==_DEL)
			{
				generateCodeForDelStmt(begining->expr);
			}
		}

		// ������� ��������� ������� ������ ����� �������
		begining = begining->next;
	}
}


void CodeGeneration::generateCodeForExpr(struct ExprInfo * expr, bool left)
{
	struct Operation* curOp;
	// ���� ����� - ��������� ��� ���. ���������� (�� ����� �����)
	if(expr->type==_VARVAL)
	{
		// ���������� �������, ����������� �������� �� ����
		curOp = new struct Operation;
		if(expr->exprVal->type==_TRUE||expr->exprVal->type==_FALSE)//||expr->exprVal->type==_NUMBER||expr->exprVal->type==_STRING)
		{
			curOp->type=__LDC;
			curOp->u1=expr->exprVal->numberInTable;
			curOp->countByte=2;
			oper.push_back(curOp);
			stackSize++;

			curOp = new struct Operation;
			curOp->type=__INVOKESTATIC;
			curOp->u2=___VALUE_FROM_BOOLEAN;
			curOp->countByte=3;
			oper.push_back(curOp);
		}
		else if (expr->exprVal->type==_FLOAT)
		{

			curOp->type=__LDC_W;
			curOp->u2=expr->exprVal->numberInTable;
			curOp->countByte=3;
			oper.push_back(curOp);
			stackSize++;

			curOp = new struct Operation;
			curOp->type=__INVOKESTATIC;
			curOp->u2=___VALUE_FROM_FLOAT;
			curOp->countByte=3;
			oper.push_back(curOp);
		}
		else if (expr->exprVal->type==_NUMBER)
		{
			curOp->type=__LDC;
			curOp->u1=expr->exprVal->numberInTable;
			curOp->countByte=2;
			oper.push_back(curOp);
			stackSize++;

			curOp = new struct Operation;
			curOp->type=__INVOKESTATIC;
			curOp->u2=___VALUE_FROM_INT;
			curOp->countByte=3;
			oper.push_back(curOp);
		}
		else if (expr->exprVal->type==_STRING)
		{
			curOp->type=__LDC;
			curOp->u1=expr->exprVal->numberInTable;
			curOp->countByte=2;
			oper.push_back(curOp);
			stackSize++;

			curOp = new struct Operation;
			curOp->type=__INVOKESTATIC;
			curOp->u2=___VALUE_FROM_STRING;
			curOp->countByte=3;
			oper.push_back(curOp);
		}
		//oper.push_back(curOp);
	}
	else if(expr->type==_OPERAND&&!left)
	{
		// ���������� �������, ����������� �������� �� ����
		// ���� ������� - ����������, �� ���������
		// ���� ��������� - �����
		curOp = new struct Operation;
		if(expr->locFor==NULL)
		{
			curOp->type=__GET_STATIC;
			curOp->u2=expr->numberInTable;
			curOp->countByte=3;
			stackSize++;
		}
		else
		{
			curOp->type=__ALOAD;
			curOp->u1=expr->numberInTable;
			curOp->countByte=2;
			stackSize++;
		}
		oper.push_back(curOp);
	}
	// ����� - ������� ��������
	else if(expr->type==_NOT||expr->type==_UMINUS)
	{
		// ������� ������� ��������� ������
		generateCodeForExpr(expr->left,false);
		if(expr->type==_NOT)
		{
			// ���������� ������� ���������� ��������
			curOp = new struct Operation;
			curOp->type=__INVOKESTATIC;
			curOp->u2=___NOT;
			curOp->countByte=3;
			oper.push_back(curOp);
		}

	}
	// ����� - ��������� � ���� ������ ???
	// ����� - ����� ������
	else if(expr->type==_FUNCCALL)
	{
		int parCount=0;
		// ����������� ������ ��� �������� ���������� �� ����
		struct ExprInfo* elem = expr->arglist->first;
		while(elem!=NULL)
		{
			generateCodeForExpr(elem,false);
			elem=elem->next;
			parCount++;
		}
		// ���������� ������� ��� ������ ������ (����� ������)
		curOp = new struct Operation;
		curOp->type=__INVOKESTATIC;

		curOp->u2=expr->numberInTable;
		curOp->countByte=3;
		oper.push_back(curOp);
		stackSize-=parCount;
		// ��� ������� �� ���� ���-��
		stackSize++;

	}
	// ���� - ������������
	else if(expr->type==_ASSIGN)
	{
		// ���������� ��� ��� ������� ��������
		generateCodeForExpr(expr->right,false);
		// ��������� � ����������
		curOp = new struct Operation;
		if(expr->left->locFor==NULL)
		{
			curOp->type=__PUT_STATIC;
			curOp->u2=expr->left->numberInTable;
			curOp->countByte=3;
			stackSize-=2;
		}
		else 
		{
			curOp->type=__ASTORE;
			curOp->u1=expr->left->numberInTable;
			curOp->countByte=2;
			stackSize--;
		}
		oper.push_back(curOp);
	}
	// ����� - ������������ �������� �������
	else if(expr->type==_ARRID_AND_ASSIGN)
	{
		// ������ ��������� ��� ������, �������� � ������� ��������
		generateCodeForExpr(expr->left,false);
		generateCodeForExpr(expr->middle,false);
		generateCodeForExpr(expr->right,false);
		// �������� ������� �������� � ������
		curOp = new struct Operation;
		curOp->type=__INVOKESTATIC;
		curOp->u2=___LIST_SET;
		curOp->countByte=3;
		oper.push_back(curOp);
		stackSize-=3;
	}
	// ����� - ������ �� ������� �������
	else if(expr->type==_ARRID)
	{
		// ������ ��������� ��� ������ � ������� ��������
		generateCodeForExpr(expr->left,false);
		generateCodeForExpr(expr->right,false);
		// �������� ������� ��������� �� �������
		curOp = new struct Operation;
		curOp->type=__INVOKESTATIC;
		curOp->u2=___LIST_GET;
		curOp->countByte=3;
		oper.push_back(curOp);
		stackSize-=1;
	}
	// ����� - ������ ��� �����
	else if(expr->type==_ARRACT)
	{
		// ������ ��������� ��� ������ � ������� ��������
		generateCodeForExpr(expr->left,false);
		generateCodeForExpr(expr->right,false);
		// �������� ������� 
		curOp = new struct Operation;
		curOp->type=__INVOKESTATIC;
		if(strcmp(expr->idName,"append")==0)
			curOp->u2=___LIST_APPEND;
		else
			curOp->u2=___LIST_REMOVE;
		curOp->countByte=3;
		oper.push_back(curOp);
		stackSize--;
	}
	else if(expr->type==_ARRINIT)
	{
		// ������� ������ ����
		curOp = new struct Operation;
		curOp->type=__INVOKESTATIC;
		curOp->u2=___VALUE_FROM_LIST;
		curOp->countByte=3;
		oper.push_back(curOp);
		stackSize++;
		// ��� ������� �������� ������..
		struct ExprInfo* elem = expr->arglist->first;
		while(elem!=NULL)
		{
			// ���������� �����
			generateCodeForExpr(elem,false);
			// �������� �����������
			curOp = new struct Operation;
			curOp->type=__INVOKESTATIC;
			curOp->u2=___LIST_ADD_INITIALIZE;
			curOp->countByte=3;
			oper.push_back(curOp);
			elem=elem->next;
			stackSize--;
		}
	}
	// ����� ���� ��� ������� ���������� ��������
	else
	{
		// ������ ��������� ��� ������ � ������� ��������
		generateCodeForExpr(expr->left,false);
		generateCodeForExpr(expr->right,false);
		// �������� ������� ��������� �� �������
		curOp = new struct Operation;
		curOp->type=__INVOKESTATIC;
		curOp->u2=getLibOperationNumber(expr);
		curOp->countByte=3;
		oper.push_back(curOp);
		stackSize--;
	}
}


void CodeGeneration::generateCodeForIfStmt(struct IfStmtInfo * ifstmt)
{
	//1.�������� generateCodeForExpr ��� ���������
	generateCodeForExpr(ifstmt->expr,false);// �� ����� ����� ����� �� �������� Value
	// �������� toIntBool. �� ����� ����� 0 ��� 1
	Operation* toIntBool=new Operation;
	toIntBool->type=__INVOKESTATIC;
	toIntBool->u2=___TO_INT_BOOL;
	toIntBool->countByte=3;
	oper.push_back(toIntBool);

	//2.������� ifeq ��� �������� � ������ ���, � ���������� ����� ��� ��������� ��������
	Operation* ifeq=new Operation;
	ifeq->type=__IF_EQ;
	ifeq->countByte=3;
	oper.push_back(ifeq);
	int addrIfeq=oper.size()-1;

	//3.������� ����
	generateCodeForStatementList(ifstmt->stmtlist);
	//4.���� ���� ����� �����, �� ������� ����������� ������� � ���������� ����� ��� ��������� ��������
	int addrGoto=0;
	if(ifstmt->elsestmtlist!=NULL)
	{
		Operation* go_to=new Operation;
		go_to->type=__GOTO;
		go_to->countByte=3;
		oper.push_back(go_to);
	}
	addrGoto=oper.size()-1;

	//5.��������� �������� ��� ifeq(�.2)
	int offset=calcOffset(addrIfeq,addrGoto);
	oper[addrIfeq]->s2=offset;

	//6.������� ����� else ���� ��� ����
	if(ifstmt->elsestmtlist!=NULL)
	{
		generateCodeForStatementList(ifstmt->elsestmtlist);

		//7.��������� �������� ��� �.4
		int finish=oper.size()-1;
		offset=calcOffset(addrGoto,finish);
		oper[addrGoto]->s2=offset;
	}
}


void CodeGeneration::generateCodeForWhileStmt(struct WhileStmtInfo * whilestmt)
{
	//���������, ���� �� ����� ��� �������� � ������ �����
	findBreakContinue=false;
	findInStmtList(whilestmt->stmtlist);
	//���� ����, �� ������� ��������� ������� �����
	int indexInLoops=-1;
	if(findBreakContinue)
	{
		LoopData* curLoop=new LoopData;
		loops.push_back(curLoop);
		indexInLoops=loops.size()-1;
		currentLoop=indexInLoops;
	}

	//1.������� ����������� �������
	Operation* go_to=new Operation;
	go_to->type=__GOTO;
	go_to->countByte=3;
	oper.push_back(go_to);
	int addrGoto=oper.size()-1;

	//2.������� ����
	generateCodeForStatementList(whilestmt->stmtlist);
	if(indexInLoops!=-1)
		currentLoop=indexInLoops;
	oper[addrGoto]->s2=calcOffset(addrGoto,oper.size()-1);

	//3.������� ���������
	generateCodeForExpr(whilestmt->expr,false);// �� ����� ����� ����� �� �������� Value
	// �������� toIntBool. �� ����� ����� 0 ��� 1
	Operation* toIntBool=new Operation;
	toIntBool->type=__INVOKESTATIC;
	toIntBool->u2=___TO_INT_BOOL;
	toIntBool->countByte=3;
	oper.push_back(toIntBool);


	//4.������� �������� ifne ��� �������� � ������ ���� �����
	Operation* ifne=new Operation;
	ifne->type=__IF_NE;
	ifne->countByte=3;
	oper.push_back(ifne);
	int if_ne=oper.size()-1;
	int offset=calcOffset(if_ne,addrGoto+2);
	oper[if_ne]->s2=offset;

	if(indexInLoops!=-1)
	{
		loops[indexInLoops]->startLoop=addrGoto+1;
		loops[indexInLoops]->finishLoop=if_ne;
	}

	//5.������������� else ���� ���� �� ����
	if(whilestmt->elsestmt!=NULL)
	{
		generateCodeForStatementList(whilestmt->elsestmt);
		if(indexInLoops!=-1)
			loops[indexInLoops]->finishLoop=oper.size()-1;
	}


	//��� ������� ������ ��� �������� ���������� ��������
	if(indexInLoops!=-1)
	{
		LoopData* curLoop=loops[indexInLoops];
		for(int i=0; i<curLoop->contBreak.size(); i++)
		{
			if(curLoop->contBreak[i]->type==BR)
			{
				int off=calcOffset(curLoop->contBreak[i]->indexGoTo, curLoop->finishLoop);
				oper[curLoop->contBreak[i]->indexGoTo]->s2=off;
			}
			else if(curLoop->contBreak[i]->type==CON)
			{
				int off=calcOffset(curLoop->contBreak[i]->indexGoTo, curLoop->startLoop);
				oper[curLoop->contBreak[i]->indexGoTo]->s2=off;
			}
		}
	}
	findBreakContinue=false;
	findInStmtList(whilestmt->stmtlist);
	if(findBreakContinue)
		currentLoop--;

}


void CodeGeneration::generateCodeForForStmt(struct ForStmtInfo * forstmt)
{
}


void CodeGeneration::generateCodeForContinueBreakStmt(struct StmtInfo* contBreakStmt)
{
	//������������� ����������� �������
	Operation* go_to=new Operation;
	go_to->type=__GOTO;
	go_to->countByte=3;
	oper.push_back(go_to);
	int addrGoto=oper.size()-1;

	//������� ���� ��������� � ������
	ContinueBreak* conBr=new ContinueBreak;
	if(contBreakStmt->type==_BREAK)
		conBr->type=BR;
	else
		conBr->type=CON;

	conBr->indexGoTo=addrGoto;
	loops[currentLoop]->contBreak.push_back(conBr);
}


void CodeGeneration::generateCodeForDelStmt(struct ExprInfo * expr)
{
	// ��������� �� ���� �������� ���� � �������� � ������� �����
	Operation* curOp = new struct Operation;
	curOp->type=__INVOKESTATIC;
	curOp->u2=___VALUE_FROM_NONE;
	curOp->countByte=3;
	oper.push_back(curOp);
	stackSize++;
	// ��������� � ����������
	curOp = new struct Operation;
	if(expr->locFor==NULL)
	{
		curOp->type=__PUT_STATIC;
		curOp->u2=expr->numberInTable;
		curOp->countByte=3;
		stackSize--;
	}
	else 
	{
		curOp->type=__ASTORE;
		curOp->u1=expr->numberInTable;
		curOp->countByte=2;
		stackSize--;
	}
	oper.push_back(curOp);

}


void CodeGeneration::generateCodeForFuncDef(struct FuncDefInfo * funcDef)
{
}

void CodeGeneration::generateCodeForReturnStmt(struct ExprInfo * expr)
{
	if(expr->type==_VARVAL&&expr->exprVal->type==_NONE)
	{
		Operation* curOp = new struct Operation;
		curOp->type=__INVOKESTATIC;
		curOp->u2=___VALUE_FROM_NONE;
		curOp->countByte=3;
		oper.push_back(curOp);
	}
	else
		generateCodeForExpr(expr, false);
	Operation* retOp = new struct Operation;
	retOp->type=__ARETURN;
	retOp->countByte=1;
	oper.push_back(retOp);

}


int CodeGeneration::getCodeLengthMethod()
{
	int length=0;

	for(int i=0; i<oper.size(); i++)
		length+=oper[i]->countByte;

	return length;
}

void CodeGeneration::writeByteCode()
{
	for(int i=0; i<oper.size(); i++)
	{
		// ����� ��� ��������
		u1=oper[i]->type;
		_write(this->fileDesc,(void*)&u1, 1);

		// ����� ��������� ��������
		switch(oper[i]->type)
		{
			case __LDC:
			case __ILOAD:
			case __FLOAD:
			case __ALOAD:
			case __ISTORE:
			case __FSTORE:
			case __ASTORE:
				u1=(oper[i]->u1);
				_write(this->fileDesc,(void*)&u1, 1);
				break;
			case __GET_STATIC:
			case __PUT_STATIC:
			case __INVOKESTATIC:
			case __LDC_W:
				u2=htons(oper[i]->u2);
				_write(this->fileDesc,(void*)&u2, 2);
				break;
			case __IF_EQ:
			case __IF_NE:
			case __SIPUSH:
			case __GOTO:
				s2=htons(oper[i]->s2);
				_write(this->fileDesc,(void*)&s2, 2);
				break;
		}
	}
}


int CodeGeneration::calcOffset(int start, int finish)
{
	int offset=0;
	if(start<finish)
	{
		for(int i=start; i<=finish; i++)
			offset+=oper[i]->countByte;
	}
	else
	{
		for(int i=finish; i<=start; i++)
			offset+=oper[i]->countByte;
		offset=(-1)*offset;
	}
	return offset;
}


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

enum LibOperations CodeGeneration::getLibOperationNumber(struct ExprInfo * expr)
{
	switch(expr->type)
	{
	case _OR:
		return ___OR;
	case _AND:
		return ___AND;
	case _NOT_EQUAL:
		return ___NOT_EQ;
	case _EQUAL:
		return ___EQ;
	case _GREATER:
		return ___MORE;
	case _GREATER_OR_EQUAL:
		return ___MORE_OR_EQ;
	case _LESS:
		return ___LESS;
	case _LESS_OR_EQUAL:
		return ___LESS_OR_EQ;
	case _SUB:
		return ___SUB;
	case _ADD:
		return ___ADD;
	case _INT:
		return ___INT_DIV;	// ������� � ��� ����� ����� ����� �� �������
	case _MOD:
		return ___MOD;
	case _DIV:
		return ___DIV;
	case _MUL:
		return ___MUL;
	case _POW:
		return ___POW;
	}
}


void CodeGeneration::findInIf(struct IfStmtInfo * ifstmt)
{
	findInStmtList(ifstmt->stmtlist);
	if(ifstmt->elsestmtlist!=NULL)
		findInStmtList(ifstmt->elsestmtlist);
}

void CodeGeneration::findInStmtList(struct StmtListInfo* stmtList)
{
	StmtInfo* begining = stmtList->first;
	while(begining!=NULL)
	{
		if(begining->type==_IF)
			findInIf(begining->ifstmt);
		else if(begining->type==_BREAK || begining->type==_CONTINUE)
			findBreakContinue=true;

		begining=begining->next;
	}
}