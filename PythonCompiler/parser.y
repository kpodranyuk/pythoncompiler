%{
#include "struct_functions.h"
#include <string.h>
#include <math.h>
#include <process.h>
#include <stdio.h>	
extern int yylex();
extern void yyerror(const char* s);
extern FILE* logFileB;
struct StmtListInfo* root;
%}
%union
{
	int Int;
	char* StringVal;
	struct ValInfo* var_valinfo;

	struct FuncDefInfo* funcdef;
	struct DefFuncParamListInfo* defparamslist;
	struct ExprInfo * exprinfo;
	struct ExprListInfo * exprlist;
	struct StmtListInfo * stmtlist;
	struct StmtInfo * stmtinfo;
	struct IfStmtInfo * ifstmt;
	struct ForStmtInfo * forstmt;
	struct WhileStmtInfo * whilestmt;
	struct ElifListInfo * eliflist;
};
%type <stmtlist> fullroot
%type <ifstmt> if_stmt
%type <eliflist> elif_list
%type <forstmt> for_stmt
%type <whilestmt> while_stmt
%type <var_valinfo> var_val
%type <stmtinfo> stmt
%type <stmtlist> stmt_list
%type <exprinfo> expr
%type <exprlist> param_list
%type <exprlist> param_listE
%type <funcdef> func_def
%type <exprinfo> func_call
%type <Int> line_sep
%type <defparamslist> def_param_list
%type <defparamslist> def_param_listE

%nonassoc INDENT NEWLINE// Начало и конец блока кода
%left DEDENT// Начало и конец блока кода
%token ':'		// Двоеточие
%token EOF_TOKEN		// Конец файла
%token TRUE	FALSE	// Оператор логической истины
%token WHILE	// Оператор цикла пока
%token FOR		// Оператор цикла для
%token CONTINUE	// Оператор продолжения цикла
%token BREAK	// Оператор прерывания
%token IF		// Оператор условия if
%token ELIF		// Оператор условия elif
%token ELSE		// Оператор условия else 
%token RETURN	// Оператор возврата значения
%token DEF		// Оператор объявления функции
%token DEL		// Оператор удаления переменной
%token ','		// Запятая
%nonassoc '.'		// Точка
%token IN 										// Оператор in
%right '='										// Оператор присвоения
%left AND OR NOT								// Оператор и или не
%left EQUAL NOT_EQUAL							// Оператор равенства и неравенства
%left LESS_OR_EQUAL '<' GREATER_OR_EQUAL '>'	// Оператор меньше либо равно, больше либо равно, меньше, больше
%left '+'	'-'									// Оператор сложения и вычитания
%left MUL DIV MOD INT							// Оператор умножения, деления, остатка от деления, целой части от деления (возможна замена одним символом)
%left POW										// Оператор возведения в степень
%left UMINUS									// Унарный минус
%nonassoc '['		// Открывающая квадратная скобка
%nonassoc ']'		// Закрывающая квадратная скобка
%nonassoc '('		// Открывающая круглая скобка
%nonassoc ')'		// Закрывающая круглая скобка
%token <StringVal>OPERAND	// Операнд
%left <StringVal>STRING	// Строка
%left <Int>NUMBER			// Число
%start fullroot		// Стартовый символ		//fullroot NEWLINE INDENT {printf("BISON:\tfound NEWLINE INDENT fullroot\n"); fprintf(logFileB,"BISON:\tfound NEWLINE INDENT fullroot\n");}//{$$=$2;}
%%
fullroot:line_sep stmt_list {root=$2;	printf("BISON:\tconcatenated 2 strings\n"); fprintf(logFileB,"BISON:\tconcatenated 2 strings\n");}//| fullroot NEWLINE {printf("BISON:\tconcatenated NEWLINE\n"); fprintf(logFileB,"BISON:\tconcatenated NEWLINE\n");}
| stmt_list {root=$1;	printf("BISON:\tconcatenated 2 strings\n"); fprintf(logFileB,"BISON:\tconcatenated 2 strings\n");}//| fullroot NEWLINE {printf("BISON:\tconcatenated NEWLINE\n"); fprintf(logFileB,"BISON:\tconcatenated NEWLINE\n");}
;

if_stmt: IF expr ':' NEWLINE INDENT stmt_list DEDENT {$$=createIfStatement($2,$6,NULL,NULL);  printf("BISON:\tfound IF_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound IF_STMT:\t\n");}
| IF expr ':' NEWLINE INDENT stmt_list DEDENT ELSE ':' NEWLINE INDENT stmt_list DEDENT {$$=createIfStatement($2,$6,NULL,$12);  printf("BISON:\tfound IF_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound IF_STMT:\t\n");}
| IF expr ':' NEWLINE INDENT stmt_list DEDENT elif_list {$$=createIfStatement($2,$6,$8,NULL);  printf("BISON:\tfound IF_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound IF_STMT:\t\n");}
| IF expr ':' NEWLINE INDENT stmt_list DEDENT elif_list ELSE ':' NEWLINE INDENT stmt_list DEDENT {$$=createIfStatement($2,$6,$8,$13);  printf("BISON:\tfound IF_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound IF_STMT:\t\n");}
;

elif_list: ELIF expr ':' NEWLINE INDENT stmt_list DEDENT {$$=createElifList($2,$6,NULL);  printf("BISON:\tfound ELIF_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound ELIF_STMT:\t\n");}
| elif_list ELIF expr ':' NEWLINE INDENT stmt_list DEDENT {$$=createElifList($3,$7,$1);  printf("BISON:\tfound ELIF_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound ELIF_STMT:\t\n");}
;

for_stmt: FOR OPERAND IN expr ':' NEWLINE INDENT stmt_list DEDENT {$$=createForStatement(createSimpleExpr(_OPERAND,$2,NULL,createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)),$4,$8,NULL); printf("BISON:\tfound FOR_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound FOR_STMT:\t\n");}
| FOR OPERAND IN expr ':' NEWLINE INDENT stmt_list DEDENT ELSE ':' NEWLINE INDENT stmt_list DEDENT {$$=createForStatement(createSimpleExpr(_OPERAND,$2,NULL,createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)),$4,$8,$14); printf("BISON:\tfound FOR_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound FOR_STMT:\t\n");}
;

while_stmt: WHILE expr ':' NEWLINE INDENT stmt_list DEDENT {$$=createWhileStatement($2,$6,NULL); printf("BISON:\tfound WHILE_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound WHILE_STMT:\t\n");}
|  WHILE expr ':' NEWLINE INDENT stmt_list DEDENT ELSE ':' NEWLINE INDENT stmt_list DEDENT {$$=createWhileStatement($2,$6,$12); printf("BISON:\tfound WHILE_STMT:\t\n"); fprintf(logFileB,"BISON:\tfound WHILE_STMT:\t\n");}
;

stmt_list: stmt_list line_sep {$$ = createStatementList(NULL, $1); printf("BISON:\tconcatenated 2 strings\n"); fprintf(logFileB,"BISON:\tconcatenated 2 strings\n");}//| fullroot NEWLINE {printf("BISON:\tconcatenated NEWLINE\n"); fprintf(logFileB,"BISON:\tconcatenated NEWLINE\n");}
| stmt {$$ = createStatementList($1, NULL); printf("BISON:\tfound stmt_list:\t\n"); fprintf(logFileB,"BISON:\tfound stmt_list:\t\n");}
| stmt_list stmt {$$ = createStatementList($2, $1); printf("BISON:\tfound stmt_list:\t\n"); fprintf(logFileB,"BISON:\tfound stmt_list:\t\n");}
;

stmt: expr line_sep {$$ = createFromExprStatement(_EXPR,$1); printf("BISON:\tfound stmt stmt:\t\n"); fprintf(logFileB,"BISON:\tfound expr_stmt:\t\n");}
| CONTINUE line_sep {$$ = createFromContinueBreakStatement(_CONTINUE,createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column)); printf("BISON:\tfound stmt continue:\t\n"); fprintf(logFileB,"BISON:\tfound stmt_continue:\t\n");}
| BREAK line_sep {$$ = createFromContinueBreakStatement(_BREAK,createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column)); printf("BISON:\tfound stmt break:\t\n"); fprintf(logFileB,"BISON:\tfound stmt_break:\t\n");}
| RETURN expr line_sep {$$=createFromReturnStatement(_RETURN, $2,createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column)); printf("BISON:\tfound stmt return:\t\n"); fprintf(logFileB,"BISON:\tfound stmt_return:\t\n");}
| DEL OPERAND line_sep {$$=createFromDelStatement(_DEL, $2, createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column), createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column));  printf("BISON:\tfound expr: DEL_stmt\n"); fprintf(logFileB,"BISON:\tfound expr: DEL_stmt\n");}
| if_stmt {$$ = createFromIfStatement(_IF, $1); printf("BISON:\tfound if_stmt:\t\n"); fprintf(logFileB,"BISON:\tfound if_stmt:\t\n");}
| func_def {$$ = createFromFuncDefStatement(_FUNC_DEF,$1); printf("BISON:\tfound func_def:\t\n"); fprintf(logFileB,"BISON:\tfound func_def:\t\n");}
| for_stmt {$$ = createFromForStatement(_FOR, $1) ;printf("BISON:\tfound for_stmt:\t\n"); fprintf(logFileB,"BISON:\tfound for_stmt:\t\n");}
| while_stmt {$$ = createFromWhileStatement(_WHILE, $1) ; printf("BISON:\tfound while_stmt:\t\n"); fprintf(logFileB,"BISON:\tfound while_stmt:\t\n");}
;

var_val: TRUE	{$$=createValNode(_TRUE,true,NULL,NULL,createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column));				printf("BISON:\tfound var_val: TRUE\n"); fprintf(logFileB,"BISON:\tfound var_val: TRUE\n");}
| FALSE		{$$=createValNode(_FALSE,false,NULL,NULL,createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column));					printf("BISON:\tfound var_val: FALSE\n"); fprintf(logFileB,"BISON:\tfound var_val: FALSE\n");}
| STRING	{$$=createValNode(_STRING,false,$1,NULL,createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column));					printf("BISON:\tfound var_val: STRING \t %s\n", $1); fprintf(logFileB,"BISON:\tfound var_val: STRING \t %s\n", $1);}
| NUMBER		{$$=createValNode(_NUMBER,false,NULL,$1,createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column));			printf("BISON:\tfound var_val: NUMBER\n"); fprintf(logFileB,"BISON:\tfound var_val: NUMBER\n");}
;
expr: expr OR expr				{$$=createExprInfo(_OR,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: OR\n"); fprintf(logFileB,"BISON:\tfound expr: OR\n");}
| expr AND expr					{$$=createExprInfo(_AND,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: AND\n"); fprintf(logFileB,"BISON:\tfound expr: AND\n");}
| NOT expr						{$$=createExprInfo(_NOT,$2,NULL, createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column)); printf("BISON:\tfound expr: NOT\n"); fprintf(logFileB,"BISON:\tfound expr: NOT\n");}
| expr NOT_EQUAL expr			{$$=createExprInfo(_NOT_EQUAL,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: NOT_EQUAL\n"); fprintf(logFileB,"BISON:\tfound expr: NOT_EQUAL\n");}
| expr EQUAL expr				{$$=createExprInfo(_EQUAL,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: EQUAL\n"); fprintf(logFileB,"BISON:\tfound expr: EQUAL\n");}
| expr '>' expr					{$$=createExprInfo(_GREATER,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: >\n"); fprintf(logFileB,"BISON:\tfound expr: >\n");}
| expr GREATER_OR_EQUAL expr	{$$=createExprInfo(_GREATER_OR_EQUAL,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: GREATER_OR_EQUAL\n"); fprintf(logFileB,"BISON:\tfound expr: GREATER_OR_EQUAL\n");}
| expr '<' expr					{$$=createExprInfo(_LESS,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: <\n"); fprintf(logFileB,"BISON:\tfound expr: <\n");}
| expr LESS_OR_EQUAL expr		{$$=createExprInfo(_LESS_OR_EQUAL,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: LESS_OR_EQUAL\n"); fprintf(logFileB,"BISON:\tfound expr: LESS_OR_EQUAL\n");}
| expr '-' expr					{$$=createExprInfo(_SUB,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: -\n"); fprintf(logFileB,"BISON:\tfound expr: -\n");}
| expr '+' expr					{$$=createExprInfo(_ADD,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: +\n"); fprintf(logFileB,"BISON:\tfound expr: +\n");}
| expr INT expr					{$$=createExprInfo(_INT,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: INT\n"); fprintf(logFileB,"BISON:\tfound expr: INT\n");}
| expr MOD expr					{$$=createExprInfo(_MOD,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: MOD\n"); fprintf(logFileB,"BISON:\tfound expr: MOD\n");}
| expr DIV expr					{$$=createExprInfo(_DIV,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: DIV\n"); fprintf(logFileB,"BISON:\tfound expr: DIV\n");}
| expr MUL expr					{$$=createExprInfo(_MUL,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: MUL\n"); fprintf(logFileB,"BISON:\tfound expr: MUL\n");}
| expr POW expr					{$$=createExprInfo(_POW,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: POW\n"); fprintf(logFileB,"BISON:\tfound expr: POW\n");}
| '-' expr %prec UMINUS			{$$=createExprInfo(_UMINUS,$2,NULL, createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column)); printf("BISON:\tfound expr: UMINUS\n"); fprintf(logFileB,"BISON:\tfound expr: UMINUS\n");}
| '(' expr ')'					{$$=$2; printf("BISON:\tfound expr: BRACKETS\n"); fprintf(logFileB,"BISON:\tfound expr: BRACKETS\n");}
| expr '=' expr					{$$=createExprInfo(_ASSIGN,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: =\n"); fprintf(logFileB,"BISON:\tfound expr: =\n");}
| expr '[' expr ']'				{$$=createExprInfo(_ARRID,$1,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: index\n"); fprintf(logFileB,"BISON:\tfound expr: index\n");}
| '[' param_list ']'			{$$=createInitListInfo(_ARRINIT,$2, createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column)); printf("BISON:\tfound expr: initialize\n"); fprintf(logFileB,"BISON:\tfound expr: initialize\n");}
| expr '.' OPERAND '(' expr ')'  {$$=createActListInfo(_ARRACT,$1,$5,$3, createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); printf("BISON:\tfound expr: actMas\n"); fprintf(logFileB,"BISON:\tfound expr: actMas\n");}
| var_val					{$$=createSimpleExpr(_VARVAL,NULL,$1,$1->loc); printf("BISON:\tfound expr: var_val\n"); fprintf(logFileB,"BISON:\tfound expr: var_val\n");}
| OPERAND 					{$$=createSimpleExpr(_OPERAND,$1,NULL,createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column)); printf("BISON:\tfound expr: OPERAND\t%s\n",$1); fprintf(logFileB,"BISON:\tfound expr: OPERAND\t%s\n",$1);}
| func_call {$$=$1; printf("BISON:\tfound expr: FUNC_CALL\t%s\n",$1); fprintf(logFileB,"BISON:\tfound expr: FUNC_CALL\t%s\n",$1);}
;
param_list:	/*empty*/	{$$ = createExprList(NULL,NULL); printf("BISON:\tfound param_list: EMPTY\n"); fprintf(logFileB,"BISON:\tfound param_list: EMPTY\n");}
| param_listE	{$$ = createExprList(NULL,$1); printf("BISON:\tfound param_list: param_listE\n"); fprintf(logFileB,"BISON:\tfound param_list: param_listE\n");}
;
param_listE: expr	{$$ = createExprList($1,NULL); printf("BISON:\tfound param_listE: expr\n"); fprintf(logFileB,"BISON:\tfound param_listE: expr\n");}
| param_listE ',' expr	{$$ = createExprList($3,$1); printf("BISON:\tfound param_listE: ,\n"); fprintf(logFileB,"BISON:\tfound param_listE: ,\n");}
;

def_param_list:	/*empty*/	{$$ = createDefFuncParamListInfo(NULL,NULL,NULL,NULL); printf("BISON:\tfound def_param_list: EMPTY\n"); fprintf(logFileB,"BISON:\tfound def_param_list: EMPTY\n");}
| def_param_listE	{$$ = createDefFuncParamListInfo(NULL,NULL,$1,NULL);  printf("BISON:\tfound def_param_list: def_param_listE\n"); fprintf(logFileB,"BISON:\tfound def_param_list: def_param_listE\n");}
;
def_param_listE: OPERAND	{$$ = createDefFuncParamListInfo($1,NULL,NULL,createCodeLocation(@1.first_line,@1.first_column,@1.last_line,@1.last_column)); printf("BISON:\tfound def_param_listE: OPERAND\n"); fprintf(logFileB,"BISON:\tfound def_param_listE: expr\n");}
| OPERAND '=' var_val {$$ = createDefFuncParamListInfo($1,$3,NULL,createCodeLocation(@1.first_line,@1.first_column,$3->loc->lastLine,$3->loc->lastColumn)); printf("BISON:\tfound def_param_listE: OPERAND\n"); fprintf(logFileB,"BISON:\tfound def_param_listE: expr\n");}
| def_param_listE ',' OPERAND '=' var_val {$$ = createDefFuncParamListInfo($3,$5,$1,createCodeLocation(@3.first_line,@3.first_column,$5->loc->lastLine,$5->loc->lastColumn)); printf("BISON:\tfound def_param_listE: OPERAND\n"); fprintf(logFileB,"BISON:\tfound def_param_listE: expr\n");}
| def_param_listE ',' OPERAND	{$$ = createDefFuncParamListInfo($3,NULL,$1,createCodeLocation(@3.first_line,@3.first_column,@3.last_line,@3.last_column)); printf("BISON:\tfound def_param_listE: ,\n"); fprintf(logFileB,"BISON:\tfound def_param_listE: ,\n");}
;

func_def: DEF OPERAND '(' def_param_list ')' ':' NEWLINE INDENT stmt_list DEDENT	{$$=createFuncDef($2,$4,$9,createCodeLocation(@2.first_line,@2.first_column,@2.last_line,@2.last_column)); 
printf("BISON:\tfound func_def: %s\nLocation: (%d.%d-%d.%d)\n defLoc: (%d.%d-%d.%d)\n",$2,@2.first_line,@2.first_column,@2.last_line,@2.last_column,@1.first_line,@1.first_column,@1.last_line,@1.last_column); 
fprintf(logFileB,"BISON:\tfound func_def: %s\n",$2);}
;
func_call: OPERAND '(' param_list ')' {$$=createExprInfoFromFuncCall(_FUNCCALL,$1,$3, createCodeLocation(@1.first_line,@1.first_column,@4.last_line,@4.last_column)); printf("BISON:\tfound func_call: FUNC_CALL\t%s\n",$1); fprintf(logFileB,"BISON:\tfound func_call: FUNC_CALL\t%s\n",$1);}
;
line_sep:NEWLINE	{printf("BISON:\tfound line_sep\n"); fprintf(logFileB,"BISON:\tfound line_sep\n");}
| EOF_TOKEN	{printf("BISON:\tfound line_sep\n"); fprintf(logFileB,"BISON:\tfound line_sep\n");}
// NEWLINE EOF_TOKEN	%prec POW{printf("BISON:\tfound line_sep\n"); fprintf(logFileB,"BISON:\tfound line_sep\n");}
;
%%