/*
* \file tables_struct.h Содержит необходимые для генерации кода и таблиц структуры
*/

/*
* Хранит в себе возможный тип константы
*/
enum ConstType
{
    CONST_UTF8      = 1,    
    CONST_INT       = 3,
    CONST_FLOAT     = 4,
    CONST_CLASS     = 7,
    CONST_STRING    = 8,
    CONST_FIELDREF  = 9,
    CONST_METHODREF = 10,
    CONST_NAMETYPE  = 12
};

/*
* Хранит в себе информацию об элементе таблицы констант
*/
struct ConstTable_Elem
{
    enum ConstType type;	// Тип константы
	/*
	* Хранит в себе значение константы
	*/
    union 
	{
        char * utf8;	// Значение utf-8
		/*
		* Хранит в себе значение аргументов для ссылок на имя и тип
		*/
        struct 
		{
            int arg1;	// Первый аргумент ссылки (на имя)
            int arg2;	// Второй аргумент ссылки (на тип)
        } args;

        int val_int;	// Значение целого числа
    } value;
    
    struct ConstTable_Elem * next;	// Указатель на следующий элемент таблицы
};