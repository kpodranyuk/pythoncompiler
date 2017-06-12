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
	struct ClassTable_Elem* prog;			// ”казатель на таблицу класса
	struct ConstTable_List* ct;				// ”казатель на таблицу констант
	struct ConstTable_Consts* ct_consts;	// ”казатель на константы таблицы констант
	struct FieldTable_List* fields;			// ”казатель на таблицу полей класса
	struct MethodTable_List* methods;		// ”казатель на таблицу методов класса
	struct VariableTable_List* vars;		// ”казатель на таблицу локальных переменных методов
	struct StmtListInfo* treeRoot;			// ”казатель на дерево

	int fileDesc;	// ƒескриптор class файла

	/*ƒанные дл€ записи байт в class файл*/
	unsigned char u1;
	unsigned short int u2;
	unsigned int u4;
	short int s2;
	int s4;


	/*---ќ—Ќќ¬Ќџ≈ ћ≈“ќƒџ ƒЋя √≈Ќ≈–ј÷»»---*/

	/* √енераци€ кода дл€ таблицы констант
	*/
	void generateConstsTable();

	/* √енераци€ кода дл€ таблицы полей класса
	*/
	void generateFieldsTable();

	/* √енераци€ кода дл€ таблицы методов класса
	*/
	void generateMethodsTable();


	/* —генерировать код дл€ списка стейтментов
	* \param[in] stmtList узел стейтмент листа
	*/
	void generateCodeForStatementList(struct StmtListInfo* stmtList);

	/* —генерировать код дл€ выражени€
	* \param[in] expr выражение
	*/
	void generateCodeForExpr(struct ExprInfo * expr);

	/* —генерировать код дл€ узла развилки
	* \param[in] ifstmt узел развилки
	*/
	void generateCodeForIfStmt(struct IfStmtInfo * ifstmt);

	/* —генерировать код дл€ узла цикла while
	* \param[in] whilestmt узел цикла while
	*/
	void generateCodeForWhileStmt(struct WhileStmtInfo * whilestmt);

	/* —генерировать код дл€ узла цикла for
	* \param[in] forstmt узел цикла for
	*/
	void generateCodeForForStmt(struct ForStmtInfo * forstmt);

	/* —генерировать код дл€ узлов прерывани€ и продолжени€ цикла
	* \param[in] contBreakStmt узел break или continue
	*/
	void generateCodeForContinueBreakStmt(struct StmtInfo* contBreakStmt);

	/* —генерировать код дл€ узла удалени€ переменной
	* \param[in] expr узел выражени€ удалени€ переменной
	*/
	void generateCodeForDelStmt(struct ExprInfo * expr);

	/* —генерировать код дл€ узла объ€влени€ функции(там тело будем генерить)
	* \param[in] funcDef узел объ€влени€ функции
	*/
	void generateCodeForFuncDef(struct FuncDefInfo * funcDef);



public:

	//  онструктор, принимающий все таблицы
	CodeGeneration(struct ClassTable_Elem* prog, struct ConstTable_List* ct, struct ConstTable_Consts* ct_consts, struct FieldTable_List* fields, struct MethodTable_List* methods, struct VariableTable_List* vars);
	~CodeGeneration(void);

	// ћетод генерации байт-кода class файла
	void generateCode(struct StmtListInfo* treeRoot);
};

