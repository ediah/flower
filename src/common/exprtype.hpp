#ifndef EXPRTYPE_H
#define EXPRTYPE_H

enum type_t {
    _NONE_,
    _INT_,
    _REAL_,
    _STRING_,
    _BOOLEAN_,
    _LABEL_,
    _STRUCT_
};

enum operation_t {
    NONE,   //! Операции нет
    INV,    //! Сменить знак операнда
    PLUS,   //! Сложить
    MINUS,  //! Вычесть
    LOR,    //! Логическое "ИЛИ"
    MUL,    //! Умножить
    DIV,    //! Поделить
    LAND,   //! Логическое "И"
    LNOT,   //! Логическое "НЕ"
    MOD,    //! Деление по модулю
    LESS,   //! Меньше
    GRTR,   //! Больше
    LESSEQ, //! Меньше либо равно
    GRTREQ, //! Больше либо равно
    EQ,     //! Равно
    NEQ,    //! Не равно
    ASSIGN, //! Присвоить
    LOAD,   //! Загрузить значение из регистров
    READ,   //! Прочитать из стандартного ввода
    WRITE,  //! Записать в стандартный вывод
    ENDL,   //! Перенос строки в стандартном выводе
    JIT,    //! Условный переход (Jump If True)
    JMP,    //! Безусловный переход
    RET,    //! Возврат управления из функции
    CALL,   //! Вызов функции
    STOP,   //! Останов
    SHARE,  //! Сделать переменную общей
    FORK,   //! Запустить поток
    LOCK,
    UNPACK,
    ALLOC,
    DEREF   //! Вычисляет адрес элемента массива
};

type_t expressionType(type_t t1, type_t t2, operation_t o);
char * typetostr(type_t t);
int typeSize(type_t t);
bool isExpr(operation_t o);
bool isUnary(operation_t o);

int operands(operation_t o);

void debugOp(operation_t op);

#endif
