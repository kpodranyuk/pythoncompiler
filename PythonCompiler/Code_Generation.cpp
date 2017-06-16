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

	/*Создание файла*/
	this->fileDesc=_open("Program.class",  O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 644);
	
	/*Запись магического числа*/
	u4=htonl(0xCAFEBABE);
	_write(this->fileDesc, (void*)&u4, 4);

	/*Запись подверсии и версии*/
	u2=htons(3);
	_write(this->fileDesc, (void*)&u2, 2);
	u2=htons(45);
	_write(this->fileDesc, (void*)&u2, 2);

	/*Генерация таблицы констант*/
	generateConstsTable();

	/*Генерация флагов доступа*/
	u2=htons(33); //ACC_PUBLIC | ACC_SUPER
	_write(this->fileDesc,(void*)&u2, 2);

	/*Текущий класс*/
	u2=htons(prog->classConst);
	_write(this->fileDesc,(void*)&u2, 2);

	/*Класс-родитель*/
	u2=htons(ct_consts->objectClass);
	_write(this->fileDesc,(void*)&u2, 2);

	/*Кол-во интерфейсов*/
	u2=htons(0);
	_write(this->fileDesc,(void*)&u2, 2);

	/*Генерация таблицы полей класса*/
	generateFieldsTable();

	/*Генерация таблицы методов класса*/
	generateMethodsTable();

	/*Кол-во атрибутов класса*/
	u2=htons(0);
	_write(this->fileDesc,(void*)&u2, 2);

	/*Закрытие файла*/
	_close(this->fileDesc);
}


void CodeGeneration::generateConstsTable()
{
	// Подсчитать ко-во элементов в таблице констант
	int count=1;
	ConstTable_Elem* elem=this->ct->first;

	while(elem!=this->ct->last)
	{
		elem=elem->next;
		count++;
	}
	
	// Записать это кол-во+1(по методичке)
	u2=htons(count+1);
	_write(this->fileDesc,(void*)&u2, 2);

	// Для каждой константы сгенерировать байт-код
	elem=this->ct->first;
	for(int i = 0; i < count; ++i)
	{
		// Пишем тип константы
		u1=elem->type;
		_write(this->fileDesc,(void*)&u1, 1);

		// Для каждого типа свой формат записи
		if(elem->type==CONST_UTF8)
		{
			u2=htons(strlen(elem->value.utf8));
			_write(this->fileDesc,(void*)&u2, 2);// длина строки в байтах
			_write(this->fileDesc,(void*)elem->value.utf8, strlen(elem->value.utf8));// символы строки в виде последовательности байт
		}
		else if(elem->type==CONST_INT)
		{
			s4=htonl(elem->value.val_int);
			_write(this->fileDesc,(void*)&s4, 4);// целое число со знаком
		}
		// На будущее
		else if(elem->type==CONST_FLOAT)
		{
			sf4 = reverseFloatBytes(elem->value.val_float);
            _write(this->fileDesc,(void *)&sf4, 4);
		}
		else if(elem->type==CONST_CLASS || elem->type==CONST_STRING)
		{
			u2=htons(elem->value.args.arg1);
			_write(this->fileDesc,(void*)&u2, 2);// номер константы utf-8, содержащей имя
		}
		else if(elem->type==CONST_FIELDREF || elem->type==CONST_METHODREF || elem->type==CONST_NAMETYPE)
		{
			u2=htons(elem->value.args.arg1);
			_write(this->fileDesc,(void*)&u2, 2);// номер константы utf-8, содержащей имя
			u2=htons(elem->value.args.arg2);
			_write(this->fileDesc,(void*)&u2, 2);// номер константы utf-8, содержащей дескриптор
		}
		else
			throw "UNKNOWN CONSTANT TYPE";

		elem=elem->next;
	}
}


void CodeGeneration::generateFieldsTable()
{
	// Записываем кол-во полей класса
	u2=htons(prog->fieldCount);
	_write(this->fileDesc,(void*)&u2, 2);

	FieldTable_Elem* field=prog->firstField;
	// Для каждого поля
	for (int i = 0; i < prog->fieldCount; ++i)
	{
		u2=htons(field->access);
		_write(this->fileDesc,(void*)&u2, 2);// флаги доступа

		u2=htons(field->fieldName);
		_write(this->fileDesc,(void*)&u2, 2);// номер константы utf-8, содержащей имя поля
		
		u2=htons(field->fieldDesc);
		_write(this->fileDesc,(void*)&u2, 2);// номер константы utf-8, содержащей дескриптор поля

		u2=htons(field->attrs);
		_write(this->fileDesc,(void*)&u2, 2);// кол-во атрибутов(0)

		field=field->next;
	}
}


void CodeGeneration::generateMethodsTable()
{
	// Записываем кол-во методов класса
	u2=htons(prog->methodCount+1);
	_write(this->fileDesc,(void*)&u2, 2);

	// КОНСТРУКТОР
	u2=htons(1); // Флаг acc_public
	_write(this->fileDesc,(void*)&u2, 2);

	u2=htons(this->ct_consts->objectClass + 1);
	_write(this->fileDesc,(void*)&u2, 2);// Имя метода

	u2=htons(this->ct_consts->objectClass + 2);
	_write(this->fileDesc,(void*)&u2, 2);// Дескриптор

	u2=htons(1); // 1 
	_write(this->fileDesc,(void*)&u2, 2);// Количество атрибутов

	u2=htons(1); //1я константа Code
	_write(this->fileDesc,(void*)&u2, 2);// Имя атрибута

	u4=htonl(5 + 12);
	_write(this->fileDesc,(void*)&u4, 4);// Длина атрибута

	u2=htons(2);
	_write(this->fileDesc,(void*)&u2, 2);// Максимальный размер стека операндов

	u2=htons(1);
	_write(this->fileDesc,(void*)&u2, 2);// Количество локальных переменных

	u4=htonl(5);
	_write(this->fileDesc,(void*)&u4, 4);// Длина кода

	// Код
	u1=42;
	_write(this->fileDesc,(void*)&u1, 1);
	u1=183;
	_write(this->fileDesc,(void*)&u1, 1);
	u2=htons(this->ct_consts->objectClass + 4);
	_write(this->fileDesc,(void*)&u2, 2);
	u1=177;
	_write(this->fileDesc,(void*)&u1, 1);

	u2=0;
	_write(this->fileDesc,(void*)&u2, 2);// Длина таблицы обработчиков исключений

	u2=0;
	_write(this->fileDesc,(void*)&u2, 2);// Количество атрибутов


	// Для кадого метода генерим байт-код
	MethodTable_Elem* method = prog->methodsFirst;

	while(method!=NULL)
	{
		currentLocal=method->methodRef;

		u2=htons(method->access);
		_write(this->fileDesc,(void*)&u2, 2);// флаги доступа

		u2=htons(method->methodName);
		_write(this->fileDesc,(void*)&u2, 2);// номер константы utf-8, содержащей имя метода

		u2=htons(method->methodDesc);
		_write(this->fileDesc,(void*)&u2, 2);// номер константы utf-8, содержащей дескриптор метода

		u2=htons(method->attrs);
		_write(this->fileDesc,(void*)&u2, 2);// кол-во атрибутов(1)

		/* Далее вычисляем и записываем атрибут Code метода*/
		u2=htons(1); //1я константа Code(имя атрибута)
		_write(this->fileDesc,(void*)&u2, 2);

		// Генерим в массив весь байт-код метода
		generateCodeForStatementList(this->treeRoot);
		// Для мейна надо вызвать ретерн для завершения выполнения текущего метода
		if(method->methodRef==NULL)
		{
			Operation* ret=new Operation;
			ret->type=__RETURN;
			ret->countByte=1;
			oper.push_back(ret);
		}
		// Получаем размер сгенериного байт-кода
		int length=getCodeLengthMethod();

		u4=htonl(length+12);
		_write(this->fileDesc,(void*)&u4, 4);// Длина атрибута

		u2=htons(1000);
		_write(this->fileDesc,(void*)&u2, 2);// Размер стека операндов

		u2=htons(method->localVarsCount+1);
		_write(this->fileDesc,(void*)&u2, 2);// Количество локальных переменных

		u4=htonl(length);
		_write(this->fileDesc,(void*)&u4, 4);// Длина байт-кода
		writeByteCode();// Сам байт-код 
		oper.clear();

		u2=htons(0);
		_write(this->fileDesc,(void*)&u2, 2);// Количество исключений

		u2=htons(0);
		_write(this->fileDesc,(void*)&u2, 2);// Количество атрибутов


		method=method->next;
	}
}


void CodeGeneration::generateCodeForStatementList(struct StmtListInfo* stmtList)
{
	StmtInfo* begining = stmtList->first;
	// Пока текущий элемент списка не последний..
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

		// Считаем следующий элемент списка новым текущим
		begining = begining->next;
	}
}


void CodeGeneration::generateCodeForExpr(struct ExprInfo * expr, bool left)
{
	struct Operation* curOp;
	// если экспр - константа или лок. переменная (НЕ левая часть)
	if(expr->type==_VARVAL)
	{
		// генерируем команды, загружающие значение на стек
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
		// генерируем команды, загружающие значение на стек
		// если операнд - глобальный, то гетстатик
		// если локальный - алоад
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
	// экспр - унарная операция
	else if(expr->type==_NOT||expr->type==_UMINUS)
	{
		// вызваем функцию генерации экспра
		generateCodeForExpr(expr->left,false);
		if(expr->type==_NOT)
		{
			// генерируем команду выполнения операции
			curOp = new struct Operation;
			curOp->type=__INVOKESTATIC;
			curOp->u2=___NOT;
			curOp->countByte=3;
			oper.push_back(curOp);
		}

	}
	// экспр - обращение к полю класса ???
	// экспр - вызов метода
	else if(expr->type==_FUNCCALL)
	{
		int parCount=0;
		// рекурсивные вызовы для загрузки аргументов на стек
		struct ExprInfo* elem = expr->arglist->first;
		while(elem!=NULL)
		{
			generateCodeForExpr(elem,false);
			elem=elem->next;
			parCount++;
		}
		// генерируем команду для вызова метода (инвок статик)
		curOp = new struct Operation;
		curOp->type=__INVOKESTATIC;

		curOp->u2=expr->numberInTable;
		curOp->countByte=3;
		oper.push_back(curOp);
		stackSize-=parCount;
		// все положат на стек что-то
		stackSize++;

	}
	// эспр - присваивание
	else if(expr->type==_ASSIGN)
	{
		// генерируем код для правого операнда
		generateCodeForExpr(expr->right,false);
		// загружаем в переменную
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
	// экспр - присваивание элементу массива
	else if(expr->type==_ARRID_AND_ASSIGN)
	{
		// вызовы генерации для левого, среднего и правого операнда
		generateCodeForExpr(expr->left,false);
		generateCodeForExpr(expr->middle,false);
		generateCodeForExpr(expr->right,false);
		// вызываем команду загрузки в индекс
		curOp = new struct Operation;
		curOp->type=__INVOKESTATIC;
		curOp->u2=___LIST_SET;
		curOp->countByte=3;
		oper.push_back(curOp);
		stackSize-=3;
	}
	// экспр - взятие по индексу массива
	else if(expr->type==_ARRID)
	{
		// вызовы генерации для левого и правого операнда
		generateCodeForExpr(expr->left,false);
		generateCodeForExpr(expr->right,false);
		// вызываем команду получения по индексу
		curOp = new struct Operation;
		curOp->type=__INVOKESTATIC;
		curOp->u2=___LIST_GET;
		curOp->countByte=3;
		oper.push_back(curOp);
		stackSize-=1;
	}
	// экспр - аппенд или ремув
	else if(expr->type==_ARRACT)
	{
		// вызовы генерации для левого и правого операнда
		generateCodeForExpr(expr->left,false);
		generateCodeForExpr(expr->right,false);
		// вызываем команду 
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
		// Создаем пустой лист
		curOp = new struct Operation;
		curOp->type=__INVOKESTATIC;
		curOp->u2=___VALUE_FROM_LIST;
		curOp->countByte=3;
		oper.push_back(curOp);
		stackSize++;
		// Для каждого элемента списка..
		struct ExprInfo* elem = expr->arglist->first;
		while(elem!=NULL)
		{
			// Генерируем экспр
			generateCodeForExpr(elem,false);
			// Вызываем аддинишлайз
			curOp = new struct Operation;
			curOp->type=__INVOKESTATIC;
			curOp->u2=___LIST_ADD_INITIALIZE;
			curOp->countByte=3;
			oper.push_back(curOp);
			elem=elem->next;
			stackSize--;
		}
	}
	// Иначе идут уже обычные двуместные операции
	else
	{
		// вызовы генерации для левого и правого операнда
		generateCodeForExpr(expr->left,false);
		generateCodeForExpr(expr->right,false);
		// вызываем команду получения по индексу
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
	//1.Вызываем generateCodeForExpr для выражения
	generateCodeForExpr(ifstmt->expr,false);// На стеке будет какое то значение Value
	// Вызываем toIntBool. На стеке будет 0 или 1
	Operation* toIntBool=new Operation;
	toIntBool->type=__INVOKESTATIC;
	toIntBool->u2=___TO_INT_BOOL;
	toIntBool->countByte=3;
	oper.push_back(toIntBool);

	//2.Генерим ifeq для перехода в случае лжи, и запоминаем адрес для уточнения смещения
	Operation* ifeq=new Operation;
	ifeq->type=__IF_EQ;
	ifeq->countByte=3;
	oper.push_back(ifeq);
	int addrIfeq=oper.size()-1;

	//3.Генерим тело
	generateCodeForStatementList(ifstmt->stmtlist);
	//4.Если есть ветка иначе, то генерим безусловный переход и запоминаем адрес для уточнения смещения
	int addrGoto=0;
	if(ifstmt->elsestmtlist!=NULL)
	{
		Operation* go_to=new Operation;
		go_to->type=__GOTO;
		go_to->countByte=3;
		oper.push_back(go_to);
	}
	addrGoto=oper.size()-1;

	//5.Вычисляем смещение для ifeq(п.2)
	int offset=calcOffset(addrIfeq,addrGoto);
	oper[addrIfeq]->s2=offset;

	//6.Генерим ветку else если она есть
	if(ifstmt->elsestmtlist!=NULL)
	{
		generateCodeForStatementList(ifstmt->elsestmtlist);

		//7.Вычисляем смещение для п.4
		int finish=oper.size()-1;
		offset=calcOffset(addrGoto,finish);
		oper[addrGoto]->s2=offset;
	}
}


void CodeGeneration::generateCodeForWhileStmt(struct WhileStmtInfo * whilestmt)
{
	//Проверяем, есть ли брейк или континью в данном цикле
	findBreakContinue=false;
	findInStmtList(whilestmt->stmtlist);
	//Если есть, то создаем структуру данного цикла
	int indexInLoops=-1;
	if(findBreakContinue)
	{
		LoopData* curLoop=new LoopData;
		loops.push_back(curLoop);
		indexInLoops=loops.size()-1;
		currentLoop=indexInLoops;
	}

	//1.Генерим безусловный переход
	Operation* go_to=new Operation;
	go_to->type=__GOTO;
	go_to->countByte=3;
	oper.push_back(go_to);
	int addrGoto=oper.size()-1;

	//2.Генерим тело
	generateCodeForStatementList(whilestmt->stmtlist);
	if(indexInLoops!=-1)
		currentLoop=indexInLoops;
	oper[addrGoto]->s2=calcOffset(addrGoto,oper.size()-1);

	//3.Генерим выражение
	generateCodeForExpr(whilestmt->expr,false);// На стеке будет какое то значение Value
	// Вызываем toIntBool. На стеке будет 0 или 1
	Operation* toIntBool=new Operation;
	toIntBool->type=__INVOKESTATIC;
	toIntBool->u2=___TO_INT_BOOL;
	toIntBool->countByte=3;
	oper.push_back(toIntBool);


	//4.Генерим операцию ifne для перехода к началу тела цикла
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

	//5.Сгенерировать else блок если он есть
	if(whilestmt->elsestmt!=NULL)
	{
		generateCodeForStatementList(whilestmt->elsestmt);
		if(indexInLoops!=-1)
			loops[indexInLoops]->finishLoop=oper.size()-1;
	}


	//Для каждого брейка или континью определить смещения
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
	//Сгенерировать безусловный переход
	Operation* go_to=new Operation;
	go_to->type=__GOTO;
	go_to->countByte=3;
	oper.push_back(go_to);
	int addrGoto=oper.size()-1;

	//Записть этот стейтмент в массив
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
	// Поместить на стек значение Ноун и записать в текущий экспр
	Operation* curOp = new struct Operation;
	curOp->type=__INVOKESTATIC;
	curOp->u2=___VALUE_FROM_NONE;
	curOp->countByte=3;
	oper.push_back(curOp);
	stackSize++;
	// загружаем в переменную
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
		// Пишем код операции
		u1=oper[i]->type;
		_write(this->fileDesc,(void*)&u1, 1);

		// Пишем аргументы операции
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
		return ___INT_DIV;	// СДЕЛАТЬ В РТЛ МЕТОД ЦЕЛОЙ ЧАСТИ ОТ ДЕЛЕНИЯ
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