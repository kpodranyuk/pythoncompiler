/*
* \file Code_Generation.h хранит в себе код генерации кода
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include "tree_structs.h"
#include "tables_structs.h"


class CodeGeneration
{
private:
	struct ClassTable_Elem* prog;			// Указатель на таблицу класса
	struct ConstTable_List* ct;				// Указатель на таблицу констант
	struct ConstTable_Consts* ct_consts;	// Указатель на константы таблицы констант
	struct VariableTable_List* vars;		// Указатель на таблицу локальных переменных методов
	struct StmtListInfo* treeRoot;			// Указатель на дерево

	int fileDesc;	// Дескриптор class файла

	/*Данные для записи байт в class файл*/
	unsigned char u1;
	unsigned short int u2;
	unsigned int u4;
	short int s2;
	int s4;


	/*---ОСНОВНЫЕ МЕТОДЫ ДЛЯ ГЕНЕРАЦИИ---*/

	/* Генерация кода для таблицы констант
	*/
	void generateConstsTable();

	/* Генерация кода для таблицы полей класса
	*/
	void generateFieldsTable();

	/* Генерация кода для таблицы методов класса
	*/
	void generateMethodsTable();


	/* Сгенерировать код для списка стейтментов
	* \param[in] stmtList узел стейтмент листа
	*/
	void generateCodeForStatementList(struct StmtListInfo* stmtList);

	/* Сгенерировать код для выражения
	* \param[in] expr выражение
	*/
	void generateCodeForExpr(struct ExprInfo * expr);

	/* Сгенерировать код для узла развилки
	* \param[in] ifstmt узел развилки
	*/
	void generateCodeForIfStmt(struct IfStmtInfo * ifstmt);

	/* Сгенерировать код для узла цикла while
	* \param[in] whilestmt узел цикла while
	*/
	void generateCodeForWhileStmt(struct WhileStmtInfo * whilestmt);

	/* Сгенерировать код для узла цикла for
	* \param[in] forstmt узел цикла for
	*/
	void generateCodeForForStmt(struct ForStmtInfo * forstmt);

	/* Сгенерировать код для узлов прерывания и продолжения цикла
	* \param[in] contBreakStmt узел break или continue
	*/
	void generateCodeForContinueBreakStmt(struct StmtInfo* contBreakStmt);

	/* Сгенерировать код для узла удаления переменной
	* \param[in] expr узел выражения удаления переменной
	*/
	void generateCodeForDelStmt(struct ExprInfo * expr);

	/* Сгенерировать код для узла объявления функции(там тело будем генерить)
	* \param[in] funcDef узел объявления функции
	*/
	void generateCodeForFuncDef(struct FuncDefInfo * funcDef);



public:

	// Конструктор, принимающий все таблицы
	CodeGeneration(struct ClassTable_Elem* prog, struct ConstTable_List* ct, struct ConstTable_Consts* ct_consts, struct VariableTable_List* vars);
	~CodeGeneration(void);

	// Метод генерации байт-кода class файла
	void generateCode(struct StmtListInfo* treeRoot);
};

