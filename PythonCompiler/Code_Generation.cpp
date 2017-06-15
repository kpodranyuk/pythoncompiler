#include "code_generation_.h"


CodeGeneration::CodeGeneration(struct ClassTable_Elem* prog, struct ConstTable_List* ct, struct ConstTable_Consts* ct_consts, struct VariableTable_List* vars)
{
	this->prog=prog;
	this->ct=ct;
	this->ct_consts=ct_consts;
	this->vars=vars;
	stackSize=0;
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

	u2=htons(1);
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
		// Получаем размер сгенериного байт-кода
		int length=getCodeLengthMethod();

		u4=htonl(length+12);
		_write(this->fileDesc,(void*)&u4, 4);// Длина атрибута

		u2=htons(1200);
		_write(this->fileDesc,(void*)&u2, 2);// Размер стека операндов

		u2=htons(method->localVarsCount);
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
				generateCodeForExpr(begining->expr, true);
				if(stackSize>0)
				{
					for(int i=0;i<stackSize;i++)
					{
						//TODO сделать pop
					}
					stackSize=0;
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
				generateCodeForFuncDef(begining->funcdefstmt);
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


void CodeGeneration::generateCodeForExpr(struct ExprInfo * expr, bool inStmt)
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

void CodeGeneration::generateCodeForReturnStmt(struct ExprInfo * expr)
{
}


int CodeGeneration::getCodeLengthMethod()
{
	int length=0;

	for(int i=0; i<oper.size(); i++)
		length+=oper[i].countByte;

	return length;
}

void CodeGeneration::writeByteCode()
{
	for(int i=0; i<oper.size(); i++)
	{
		// Пишем код операции
		u1=oper[i].type;
		_write(this->fileDesc,(void*)&u1, 1);

		// Пишем аргументы операции
		switch(oper[i].type)
		{
			case __LDC:
			case __LDC_W:
			case __ILOAD:
			case __FLOAD:
			case __ALOAD:
			case __ISTORE:
			case __FSTORE:
			case __ASTORE:
				u1=htons(oper[i].u1);
				_write(this->fileDesc,(void*)&u1, 1);
				break;
			case __GET_STATIC:
			case __PUT_STATIC:
			case __INVOKESTATIC:
				u2=htons(oper[i].u2);
				_write(this->fileDesc,(void*)&u2, 2);
				break;
			case __IF_EQ:
			case __GOTO:
				s2=htons(oper[i].s2);
				_write(this->fileDesc,(void*)&s2, 2);
				break;
		}
	}
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