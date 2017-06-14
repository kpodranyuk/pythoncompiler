/*
* \file bypass_semant_tree.h ������ � ���� ��� ������ ������ �� ����� ���������
*/
#include "tree_structs.h"
#include "tables_structs.h"
#include "struct_functions.h"
#include <vector>
#include <map>
#include <string>

#pragma warning(disable : 4290)

/*
* ����� ������ ������ ���������
*/
class TreeTraversal{
	/*!
	*	!!!!! �������� ����� ������ !!!!!
	*/
private:
	/*
	*	---------- ���� ������ ----------
	*/

	/*
	* ������ � ���� ���������� ��������� �����������
	*/
	enum GlobalState{
		_MAIN_STATE,	// ���������� ��� (������� main)
		_FUNC_STATE		// ��������� ��� (������ �������, ������������ �������������)
	} gl_state;

	/*
	* ������ � ���� ��������� ��������� �����������
	*/
	enum LocalState{
		_CYCLE_STATE,	// ���������� ������ �����
		_REGULAR_STATE	// ���������� �� ������ �����
	} lc_state;

	/*
	* ������ � ���� ��������� �������
	*/
	struct FunctionHeader{
		char* functionName;						// ��� �������
		struct DefFuncParamListInfo* params;		// ��������� �������
	};

	/*
	* ������ � ���� ��� ���� ������ � ����� ��� fieldRef
	*/
	struct opInfo{
		char* opName;		// ��� ����
		int FR;				// ����� FieldRef ��� ����� ��������� ����������
		int local;			// ����� MethodRef, ��� �������� �������� ����������
	};

	/*
	* ������ � ���� ���������� ������ � ������ � ��� �����������
	*/
	struct mDescInfo{
		char* desc;		// ���������� ������
		int num;		// ����� ������ � �����������
	};

	/*
	* ������ � ���� ��� ������� � �� methodRef
	*/
	struct methodInfo{
		char* name;		// ��� ������
		int MR	;		// methodRef
	};

	/*
	* ������ � ���� ��������� � ������ �� ��� � �������
	*/
	struct constInfo{
		enum ValType t;	// ��� ���������
		int val;		// �������� ��������� ���/���
		char* strVal;	// ������� �������� ���������
		int num;		// ����� � ������� ��������
	};

	bool hasReturn;		// ���� ����, ��� ������� ������� ����� ������������ ��������

	std::string currentFuncName;
	int valClassDesc;	// ������ �� ��������� ������ Value � ������� ��������
	int typeDesc;		// ������ �� ��������� ����������� ���� LValue;

	struct ClassTable_Elem* prog;			// ��������� �� ������� ������
	struct ConstTable_List* ct;				// ��������� �� ������� ��������
	struct ConstTable_Consts* ct_consts;	// ��������� �� ��������� ������� ��������

	//struct FieldTable_List* fields;			// ��������� �� ������� ����� ������
	//struct MethodTable_List* methods;		// ��������� �� ������� ����� ������
	struct VariableTable_List* vars;		// ��������� �� ������� ��������� ���������� �������
	/*
	* ������ � ���� ������ ����� ������
	*/
	std::vector<struct opInfo*> ops;
	/*
	* ������ � ���� ������ ������������ �������
	*/
	std::vector<struct mDescInfo*> mDescs; 
	/*
	* ������ � ���� ������ ������������ �������
	*/
	std::vector<struct methodInfo*> MRs;
	/*
	* ������ � ���� ������ �������� � ������� ��������
	*/
	std::vector<struct constInfo*> consts;

	


	/*
	* ������ � ���� ������ ���� ����������� �������
	*/
	std::vector<std::string> funcNames;
	/*
	* ������ � ���� ������ ����������� �������
	*/
	std::vector<struct FunctionHeader*> funcHeaders;

	/*
	* ������ � ���� ������ ������������������ ����������
	* ���� - ��� �������
	* ���������� ��� - global
	*/
	std::map<std::string,std::vector<std::string>> varDecls;

	/*
	* ������ � ���� ������ ������������ ����������
	* ���� - ��� �������
	* �� ����������� � ���������� ����
	*/
	std::map<std::string,std::vector<std::string>> varUsage;

	/*
	* ������ � ���� ������ ����������� ������� �����
	*/
	std::vector<std::string> libFuncs;

	/*
	*							---------- ������ ������ ----------
	*/
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ������ ����� !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	/* ��������� ����-��������� ������
	* @author Kate, Nikita
	* \param[in|out] expr ���� ���������
	* \param[in] assign ����, ������������, ��� � ��������� ��� ���� ������������
	*/
	void checkExpr(struct ExprInfo * expr, bool assign) throw(char*);

	/* ��������� ����-�������� ������
	* @author Nikita
	* \param[in|out] ifstmt ����-��������
	*/
	void checkIfStmt(struct IfStmtInfo * ifstmt) throw(char*);

	/* ��������� ����-���� while ������
	* @author Nikita
	* \param[in|out] whilestmt ���� ����� while
	*/
	void checkWhileStmt(struct WhileStmtInfo * whilestmt) throw(char*);

	/* ��������� ����-���� for ������
	* @author Nikita
	* \param[in|out] forstmt ���� ����� for
	*/
	void checkForStmt(struct ForStmtInfo * forstmt) throw(char*);

	/* ��������� ���� ����������� ������� ������
	* @author Kate
	* \param[in|out] funcdefstmt ���� ����������� ������� ������
	*/
	void checkFuncDefStmt(struct FuncDefInfo * funcdefstmt) throw(char*);

	/* ��������� ���� ���������� ����������/����������� �����
	* @author Nikita
	* \param[in|out] type ��� ����
	*/
	void checkContinueBreakStmt(struct StmtInfo* contBreakStmt) throw(char*);

	/* ��������� ���� ��������� ����������� �� �������
	* @author Kate
	* \param[in|out] retStmt ���� ���������
	* \param[in|out] expr ���� ���������
	*/
	void checkReturnStmt(struct StmtInfo* retStmt, struct ExprInfo * expr) throw(char*);

	/* ��������� ���� ��������� �������� ��������
	* @author Kate
	* \param[in|out] expr ���� ���������
	*/
	void checkDelStmt(struct ExprInfo * expr) throw(char*);

	/* ��������� ���� ���������� ������� ��� ����������
	* @author Kate
	* \param[in|out] params ������ ���������� ������� ��� ����������
	*/
	void checkFuncParams(struct DefFuncParamListInfo* params);

	/* ������ ������ (������ �����������) � ��������� ��� �����������
	* @author Kate
	* \param[in|out] root ������ ������ ������
	*/
	void checkStatementList(struct StmtListInfo* root) throw(char*);

	/* ���������, ����� �� ��� ��������� �������
	* @author Kate
	* \param[in] first ��������� ������ �������
	* \param[in] second ��������� ������ �������
	*/
	void checkFuncCall(struct FunctionHeader* first, struct FunctionHeader* second) const;

	//!!!!!!!!!!!!!!!!!!!!!!!! ��������������� ������� ������� ������ !!!!!!!!!!!!!!!!!!!!!!!!!!!!

	/* ���������, �������� �� ���������� ������� ������������
	* @author Kate
	* \param[in] expr ��������� ������ �������
	* \return true, ���� ��������, ����� - false
	*/
	bool isLibFunctionCall(struct ExprInfo * expr);

	/* ���������, ����� �� ��� ��������� �������
	* @author Kate
	* \param[in] first ��������� ������ �������
	* \param[in] second ��������� ������ �������
	*/
	bool isEqualFuncHeaders(struct FunctionHeader* first, struct FunctionHeader* second) const;

	/* ���������, ����� �� ��� ������ ���������� ������� ��� ����������
	* @author Kate
	* \param[in] first ������ ������
	* \param[in] second ������ ������
	*/
	bool isEqualDefFuncParams(struct DefFuncParamListInfo* first, struct DefFuncParamListInfo* second) const;

	/* ���������, ����� �� ��� ��������
	* @author Kate
	* \param[in] first ������ ��������
	* \param[in] second ������ ��������
	*/
	bool isEqualVarVals(struct ValInfo* first, struct ValInfo* second) const;

	/* ������� ������� �� �������
	* @author Kate
	* \param[in|out] vec ������ �������
	* \param[in] func �������
	*/
	void deleteFuncHeader(std::vector<struct FunctionHeader*>& vec, struct FunctionHeader* func);

	/* ������� ������� �� �������
	* @author Kate
	* \param[in|out] vec ������ �������
	* \param[in] func ��� �������
	*/
	void deleteFuncHeader(std::vector<struct FunctionHeader*>& vec, std::string funcName);

	/* ��������� �������� �� ������ �������
	* @author Kate
	* \param[in] vec ������ �������
	* \param[in] func �������
	*/
	bool containsFuncHeader(std::vector<struct FunctionHeader*>& vec, struct FunctionHeader* func) const;

	/* ������� ������ �� �������
	* @author Kate
	* \param[in|out] vec ������ �����
	* \param[in] str ������
	*/
	void deleteString(std::vector<std::string>& vec, std::string str);

	/* ��������� �������� �� ������ ������
	* @author Kate
	* \param[in] vec ������ �����
	* \param[in] str ������
	*/
	bool containsString(std::vector<std::string>& vec, std::string str) const;

	/* ��������� ����-��������� ������ �� ������� �������� "������������"
	* @author Nikita
	* \param[in|out] expr ���� ���������
	* \return ������� ���� �� �������� "������������" ��� ���
	*/
	bool exprContainsAssign(struct ExprInfo * expr);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ������ ����� ������ !!!!!!!!!!!!!!!!!!!!!!!!!!!!

	/* \
	* @author Kate
	* \param[in] funcdefstmt ���� ���������
	* \param[in|out] constNum ����� ���������
	* \param[in] local ����� ���������, ��� ������� ��� ����������� �������� ������� ����� ����������
	*/
	void parseIfForTable(const struct IfStmtInfo * ifstmt, int local);

	/* \
	* @author Kate
	* \param[in] funcdefstmt ���� ���������
	* \param[in|out] constNum ����� ���������
	* \param[in] local ����� ���������, ��� ������� ��� ����������� �������� ������� ����� ����������
	*/
	void parseForForTable(const struct ForStmtInfo * forstmt, int local);

	/* \
	* @author Kate
	* \param[in] funcdefstmt ���� ���������
	* \param[in|out] constNum ����� ���������
	* \param[in] local ����� ���������, ��� ������� ��� ����������� �������� ������� ����� ����������
	*/
	void parseWhileForTable(const struct WhileStmtInfo * whilestmt, int local);

	/* ��������� ����-��������� ������ ��� ����������� �������
	* @author Kate, Nikita
	* \param[in] expr ���� ���������
	* \param[in|out] table �������
	* \param[in] local ����� ���������, ��� ������� ��� ����������� �������� ������� ����� ����������
	* \param[in] typeAboveExpression ��� ��� ���������
	*/
	void parseExprForTable(struct ExprInfo * expr, int local, enum ExprType typeAboveExpression);

	/* ������ ������ (������ �����������) � ��������� ��� �����������
	* @author Kate
	* \param[in] root ������ ������ ������
	* \param[in|out] constNum ����� ���������
	* \param[in] local ����� ���������, ��� ������� ��� ����������� �������� ������� ����� ����������
	*/
	void parseStmtListForTable(const struct StmtListInfo* root, int local);

	/* ��������� ����-���������� ������� ������ ��� ����������� �������
	* @author Kate
	* \param[in] funcdefstmt ���� ���������
	* \param[in|out] constNum ����� ���������
	* \param[in] local ����� ���������, ��� ������� ��� ����������� �������� ������� ����� ����������
	*/
	void parseFuncDefForTable(struct FuncDefInfo * funcdefstmt, int local);

	/* ��������� ����-���������� ������� ������ ��� ����������� �������
	* @author Kate
	* \param[in] funcdefstmt ���� ���������
	* \param[in|out] constNum ����� ���������
	* \param[in] local ����� ���������, ��� ������� ��� ����������� �������� ������� ����� ����������
	*/
	void parseValTypeForTable(struct ValInfo * val, int local);

	/* ������� ������� ������ ������� ��������
	* @author Nikita
	* \param[in] type ��� ���������
	* \param[in] numberInTable ����� ��������� � ������� ��������
	* \param[in] utf8 �������� ���������(������ ��� �������� ���������� ��� ����������)
	* \param[in] val_int ������� �������� ���������
	* \param[in] arg1 ������ �������� ������(�� ���)
	* \param[in] arg2 ������ �������� ������(�� ���)
	*/
	struct ConstTable_Elem* makeTableEl(enum ConstType type, int* numberInTable, char * utf8, int val_int, int arg1, int arg2);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ��������������� ������� ������� ������ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	/*
	* ���������������� ������� ��������
	* \param[in|out] ct ������� ��������
	*/
	void initializeConstTable();

	/*
	* �������� ������� ������� � ����� �������
	* \param[in] ce ������� ��� ����������
	*/
	void appendToConstTable(struct ConstTable_Elem* ce);

	/*
	* �������� ������� ������� � ����� �������
	* \param[in] ce ������� ��� ����������
	*/
	void appendToFieldTable(struct FieldTable_Elem* fe);

	/*
	* �������� ������� ������� � ����� �������
	* \param[in] ce ������� ��� ����������
	*/
	void appendToMethodTable(struct MethodTable_Elem* me);

	/*
	* �������� ������� ������� � ����� �������
	* \param[in] ce ������� ��� ����������
	*/
	void appendToVarsTable(struct Variable* v);

	/*
	* \
	* \param[in] ce ������� ��� ����������
	*/
	void appendToMDescs(char* desc, int num);

	/*
	* \
	* \param[in] ce ������� ��� ����������
	*/
	void appendMToMT(char* name, int methodRef);

	/*
	* \
	* \param[in] ce ������� ��� ����������
	*/
	void appendCToC(enum ValType type, int val, char* charVal, int number);

	void findOpInCT(struct ExprInfo* expr,int local);

	void findMInMT(struct ExprInfo* expr);
	
	bool findCInCT(struct ValInfo* val);

	int findMDesc(char* desc);


	// !!!!!!!!!!!!!!!!!!!!!!!! ������� ����������� ������ ���������� !!!!!!!!!!!!!!!!!!!!

	/*
	* ������� ������ ��� ������� ����������
	* \param[in] message ��������� �� ������
	* \param[in] location ������������ ������
	*/
	char* makeStringForException(char* message, struct CodeLocation* location);

	/*!
	*	!!!!! ��������� ����� ������ !!!!!
	*/
public:
	// ����������� �� ���������
	TreeTraversal();

	// �������� ������ (������ �����)
	void fixTree(struct StmtListInfo* root) throw(char*);

	// ����������� ������ (������ �����)
	void makeTables(const struct StmtListInfo* treeRoot);

	// ������ ������������������ ������ (������ ����� - ?)
	void printTree(const struct StmtListInfo* treeRoot);

	// ������ �� �������
	struct ClassTable_Elem* getClassTable();
	struct ConstTable_List* getConstTableList();
	struct ConstTable_Consts* getConstTableConsts();
	struct VariableTable_List* getVariableTableList();
};