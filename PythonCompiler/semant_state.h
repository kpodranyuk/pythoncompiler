/*
* \file semant_state.h ������ � ���� ��������� �������������� �����������
* @author Kate
*/

/*
* ������ � ���� ���������� ��������� �����������
*/
enum GlobalState{
	_MAIN_STATE,	// ���������� ��� (������� main)
	_FUNC_STATE		// ��������� ��� (������ �������, ������������ �������������)
};

/*
* ������ � ���� ��������� ��������� �����������
*/
enum LocalState{
	_CYCLE_STATE,	// ���������� ������ �����
	_REGULAR_STATE	// ���������� �� ������ �����
};
