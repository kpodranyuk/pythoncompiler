/*
* \file bypass_semant_tree.h ������ � ���� ��� ������ ������ �� ����� ���������
*/
#include "tree_structs.h"
#include <vector>

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
	* ������ � ���� ������ ������������������ ����������
	*/
	std::vector<std::string> varNames;
	/*
	* ������ � ���� ������ ����������� �������
	*/
	std::vector<std::string> funcNames;


	/*
	*	---------- ������ ������ ----------
	*/

	/* ������� ������ �� �������
	* @author Kate
	* \param[in] vec ������ �����
	* \param[in] str ������
	*/
	void TreeTraversal::deleteString(std::vector<std::string>& vec, std::string str);

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
	* \param[in|out] expr ���� ���������
	*/
	void checkReturnStmt(struct ExprInfo * expr) throw(char*);

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