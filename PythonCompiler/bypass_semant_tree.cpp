#include "bypass_semant_tree.h"

#pragma warning(disable : 4996)

/** ������ ����� ������� ��������
* �������: � ���������; � ������ (?); ��������
* ������ � ���� ������� � ������� csv, ����������� - ;
* ������ ���������: "Constant number";"String number";"Type";"Static";"Constant value";
* ������ ������ ���������: 1;;"utf-8";"Code"
* ������� �������� ��� ����������� ���� ������ ������ � ���� global.csv
* ������� �������� ��� ������� ������ � ���� <func_name>.csv
**/

/* ����������� �� ������:
* !!! ������� ����� ������ ���������� ����������, �� ���������� ��� ���������� ������� - ��� !!!
* !!! ������� ���������� ��������� ����������, �� �� ������ �� �������� (�� ��������, �� �� ������) !!!
*/

// ����������� �� ���������
TreeTraversal::TreeTraversal()
{
}

// ������ ������������������ ������ (������ ����� - ?)
void TreeTraversal::printTree(const struct StmtListInfo* treeRoot)
{
}

/*!
*	---------------- ���� ��������������� ������� ��� ������ �� �������� � ������������ ----------------
*/

/*std::string TreeTraversal::convertTypeToString(enum TableElemType type)
{
	std::string str="";
	switch (type)
	{
	case(_UTF8):
		str="UTF-8";
		break;
	case(_INT):
		str="INT";
		break;
	case(_STRING):
		str="String";
		break;
	case(_NAMEnTYPE):
		str="Name&Type";
		break;
	case(_FIELDREF):
		str="FieldRef";
		break;
	case(_METHODREF):
		str="MethodRef";
		break;
	case(_CLASS):
		str="Class";
		break;		
	}
	return str;
}*/

void TreeTraversal::appendToConstTable(struct ConstTable_Elem* ce)
{
	struct ConstTable_Elem* last_el=this->ct->last;
	this->ct->last=ce;
	this->ct->last->next=NULL;
	last_el->next=this->ct->last;
}

bool TreeTraversal::isEqualFuncHeaders(struct FunctionHeader* first, struct FunctionHeader* second) const
{
	return strcmp(first->functionName,second->functionName)==0&&isEqualDefFuncParams(first->params,second->params);
}

bool TreeTraversal::isEqualDefFuncParams(struct DefFuncParamListInfo* first, struct DefFuncParamListInfo* second) const
{
	struct DefFuncParamInfo* beginFirst = first->first;
	struct DefFuncParamInfo* beginSecond = second->first;
	while(beginFirst!=NULL&&beginSecond!=NULL)
	{
		// �����, ����� ����� �������� � �������� (���� ����)
		// ���� ������� ����� ���������� - ������ �� �����
		if(strcmp(beginFirst->paramName,beginSecond->paramName)!=0)
			return false;
		// ���� �� ����� �������� ���������� - ������ �� �����
		if(!isEqualVarVals(beginFirst->paramVal,beginSecond->paramVal))
			return false;
		// ��� ��� �������� �����, ���������� � ��������� ��������� ���������
		beginFirst = beginFirst->next;
		beginSecond = beginSecond->next;
	}
	// ���� ������ �����, �� ��� ��������� - ����, ����� - �� �����
	return beginFirst==NULL&&beginSecond==NULL;
}

bool TreeTraversal::isEqualVarVals(struct ValInfo* first, struct ValInfo* second) const
{
	// ���� ��� ����, �� �����
	if(first==NULL&&second==NULL)
		return true;
	// ���� ��� �� ����..
	else if(first!=NULL&&second!=NULL)
	{
		// ���� ����� ����, �������� ������ �����, ���������� �������� � ��������� ��������, �� �����
		return (first->type==second->type&&
			first->intVal==second->intVal&&
			first->logVal==second->logVal&&
			(first->stringVal==NULL&&second->stringVal==NULL||
			first->stringVal!=NULL&&second->stringVal!=NULL&&
			strcmp(first->stringVal,second->stringVal)==0));
	}
	return false;
}

bool TreeTraversal::containsFuncHeader(std::vector<struct FunctionHeader*>& vec, struct FunctionHeader* func) const
{
	std::vector<struct FunctionHeader*>::const_iterator iter;  // ��������� �������� ��� ������ �������
	bool contains = false;							// ��������� ���� ����, ��� ������� �� ������� � ������
	// ��� ������� �������� ������ � ���� �� ������� ��������..
	for(iter=vec.begin(); iter<vec.end()&&!contains; iter++) 
	{
		// ���������, ����� �� ������� ������� ������
		contains=isEqualFuncHeaders((*iter),func);
	}
	return contains;
}

void TreeTraversal::deleteFuncHeader(std::vector<struct FunctionHeader*>& vec, struct FunctionHeader* func)
{
	std::vector<struct FunctionHeader*>::iterator iter;  // ��������� �������� ��� ������ �������
	// ��� ������� �������� ������ � ���� �� ������� ��������..
	for(iter=vec.begin(); iter<vec.end(); iter++) 
	{
		// ���������, ����� �� ������� ������� ������
		if(isEqualFuncHeaders((*iter),func))
		{
			// ������� ������ ��������
			vec.erase(iter);
			// ������� �� �����
			break;
		}
	}
}

bool TreeTraversal::containsString(std::vector<std::string>& vec, std::string str) const
{
	std::vector<std::string>::const_iterator iter;  // ��������� �������� ��� ������ �����
	bool contains = false;							// ��������� ���� ����, ��� ������ �� ������� � ������
	// ��� ������� �������� ������ � ���� �� ������� ��������..
	for(iter=vec.begin(); iter<vec.end()&&!contains; iter++) 
	{
		// ���������, ����� �� ������� ������ ������
		contains=(*iter).compare(str)==0;
	}
	return contains;
}

void TreeTraversal::deleteString(std::vector<std::string>& vec, std::string str)
{
	std::vector<std::string>::iterator iter;  // ��������� �������� ��� ������ �����
	// ��� ������� �������� ������ � ���� �� ������� ��������..
	for(iter=vec.begin(); iter<vec.end(); iter++) 
	{
		// ���������, ����� �� ������� ������ ������
		if((*iter).compare(str)==0)
		{
			// ������� ������ ��������
			vec.erase(iter);
			// ������� �� �����
			break;
		}
	}
}

char* TreeTraversal::makeStringForException(char* message, struct CodeLocation* location)
{
	char* bufstr;
	if(location!=NULL)
	{
		bufstr = new char [62];
		sprintf(bufstr,"Location:(%d.%d-%d.%d)",location->firstLine,location->firstColumn,location->lastLine,location->lastColumn);
	}
	else
	{
		bufstr = new char [2];
		bufstr[0] = '\0';
	}
	char* finalString = new char [strlen(bufstr)+strlen(message)+2];
	strcpy(finalString,message);
	strcat(finalString,"\n");
	strcat(finalString,bufstr);
	return finalString;
}

void TreeTraversal::initializeConstTable(struct ConstTable_List* ct)
{
	// ����� ���������
	*(ct_consts->constnumber)=1;
	// �������� ������
	ct=new struct ConstTable_List;
	ct->first=new struct ConstTable_Elem;
	// �������������� ������ �������
	ct->first->type=CONST_UTF8;
	ct->first->value.utf8="Code";
	ct->first->next=NULL;
	// ��������� ������ � ��������� ��������
	ct->last=ct->first;
	// CT_AddConst(CONST_UTF8, NULL); - ��������� ������� � ����������� �� ����
	// ������ �������� �������� � ��������� ��� �� ����� ���������� � ������
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"java/lang/Object",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "java/lang/Object";

	//CT_AddConst(CONST_CLASS, (void*)&constnumber);
	//objectClass = constnumber;

	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));
	ct_consts->objectClass=*(ct_consts->constnumber);

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "<init>";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"<init>",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "()V";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()V",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));

	//CONSTANT_INIT = constnumber;
	ct_consts->init=*(ct_consts->constnumber);

	//CT_AddConst2(CONST_METHODREF, objectClass, constnumber);
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));


	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "Program";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"Program",NULL,NULL,NULL));
	//CT_AddConst(CONST_CLASS, (void*)&constnumber);
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));

	//programClass.classConst = constnumber;
	prog->classConst=*(ct_consts->constnumber);

	//programClass.firstField = NULL;
	prog->firstField=NULL;

	// ������ ��� �������
	// ������� �������
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "main";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"main",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "([Ljava/lang/String;)V";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"([Ljava/lang/String;)V",NULL,NULL,NULL));
	ct_consts->nameconstid = *(ct_consts->constnumber) - 1;
	// ����� �� ������ �������� ��� �����?

	
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "rtl/Value";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"rtl/Value",NULL,NULL,NULL));
	//CT_AddConst(CONST_CLASS, (void*)&constnumber);
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));
	ct_consts->valueType = *(ct_consts->constnumber);
	//CT_AddConst2(CONST_METHODREF, mixedtype, CONSTANT_INIT);
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->valueType,ct_consts->init));

	/*CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "rtl/Undefined";
	CT_AddConst(CONST_CLASS, (void*)&constnumber);
	mixedtype = constnumber;
	CT_AddConst2(CONST_METHODREF, mixedtype, CONSTANT_INIT);*/


	// rtl
	//----------------------------------
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "rtl/Lib";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"rtl/Lib",NULL,NULL,NULL));
	//CT_AddConst(CONST_CLASS, (void*)&constnumber);
	appendToConstTable(makeTableEl(CONST_CLASS,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber),NULL));
	ct_consts->rtlClass = *(ct_consts->constnumber);
	
	// print
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "print";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"print",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;)V";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)V",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, ct_consts->rtlClass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//----------------------------------


	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "sub";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"sub",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//arifmentic = constnumber;
	ct_consts->arifmetic=*(ct_consts->constnumber);

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "add";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"add",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "mul";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mul",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "div";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"div",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "mod";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mod",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "toIntBool";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"toIntBool",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;)I";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)I",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	/*CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "ListAdd";
	CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	list = constnumber;

	CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "ListSet";
	CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "(Lrtl/Mixed;Lrtl/Mixed;Lrtl/Mixed;)V";
	CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);

	CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "ListGet";
	CT_AddConst(CONST_UTF8, NULL);
	lastConst->value.utf8 = "(Lrtl/Mixed;Lrtl/Mixed;)Lrtl/Mixed;";
	CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);*/

	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "eq";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"eq",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//logic = constnumber;
	ct_consts->logic=*(ct_consts->constnumber);

	//CT_AddConst(CONST_UTF8, NULL); // + 4
	//lastConst->value.utf8 = "less";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"less",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL); // + 8
	//lastConst->value.utf8 = "more";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"more",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);// + 12
	//lastConst->value.utf8 = "lessOrEq";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"lessOrEq",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);// + 16
	//lastConst->value.utf8 = "moreOrEq";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"moreOrEq",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);// + 20
	//lastConst->value.utf8 = "not";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"not",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);// + 24
	//lastConst->value.utf8 = "or";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"or",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);// + 28
	//lastConst->value.utf8 = "and";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"and",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//CT_AddConst(CONST_UTF8, NULL);// + 32
	//lastConst->value.utf8 = "notEq";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"notEq",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(Lrtl/Value;Lrtl/Value;)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(Lrtl/Value;Lrtl/Value;)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	//mixedclass = constnumber;//??
	ct_consts->valueClass=*(ct_consts->constnumber);

	// mixedFromInt 4
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "mixedFromInt";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mixedFromInt",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(I)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(I)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// mixedFromUndefined 8
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "mixedFromUndefined";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mixedFromUndefined",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "()Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// mixedFromFloat 12
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "mixedFromFloat";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mixedFromFloat",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "(F)Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"(F)Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));

	// mixedFromList 16
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "mixedFromList";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"mixedFromList",NULL,NULL,NULL));
	//CT_AddConst(CONST_UTF8, NULL);
	//lastConst->value.utf8 = "()Lrtl/Value;";
	appendToConstTable(makeTableEl(CONST_UTF8,ct_consts->constnumber,"()Lrtl/Value;",NULL,NULL,NULL));
	//CT_AddConst2(CONST_NAMETYPE, constnumber - 1, constnumber);
	//CT_AddConst2(CONST_METHODREF, rtlclass, constnumber);
	appendToConstTable(makeTableEl(CONST_NAMETYPE,ct_consts->constnumber,NULL,NULL,*(ct_consts->constnumber)-1,*(ct_consts->constnumber)));
	appendToConstTable(makeTableEl(CONST_METHODREF,ct_consts->constnumber,NULL,NULL,ct_consts->rtlClass,*(ct_consts->constnumber)));
}