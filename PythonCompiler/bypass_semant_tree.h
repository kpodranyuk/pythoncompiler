/*
* \file bypass_semant_tree.h ������ � ���� ��� ������ ������ �� ����� ���������
*/
#include "tree_structs.h"
#include <vector>

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
	* ������ � ���� ������ ������������������ ����������
	*/
	std::vector<std::string> varNames;
	/*
	* ������ � ���� ������ ����������� �������
	*/
	std::vector<struct FunctionHeader*> funcNames;


	/*
	*	---------- ������ ������ ----------
	*/

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

	/* ��������� ����-��������� ������
	* @author Kate
	* \param[in|out] expr ���� ���������
	*/
	void checkExpr(struct ExprInfo * expr) throw(char*);

	/* ��������� ����-�������� ������
	* @author Nikita
	* \param[in|out] ifstmt ����-��������
	*/
	void checkIfStmt(struct IfStmtInfo * ifstmt);

	/* ��������� ����-���� while ������
	* @author Nikita
	* \param[in|out] whilestmt ���� ����� while
	*/
	void checkWhileStmt(struct WhileStmtInfo * whilestmt);

	/* ��������� ����-���� for ������
	* @author Nikita
	* \param[in|out] forstmt ���� ����� for
	*/
	void checkForStmt(struct ForStmtInfo * forstmt);

	/* ��������� ���� ����������� ������� ������
	* @author Kate
	* \param[in|out] funcdefstmt ���� ����������� ������� ������
	*/
	void checkFuncDefStmt(struct FuncDefInfo * funcdefstmt);

	/* ��������� ���� ���������� ����������/����������� �����
	* @author Nikita
	* \param[in|out] type ��� ����
	*/
	void checkContinueBreakStmt(enum StmtType type);

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
	void checkDelStmt(struct ExprInfo * expr);

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
};