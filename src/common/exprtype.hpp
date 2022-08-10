#ifndef EXPRTYPE_H
#define EXPRTYPE_H

#include <cstdint>

/** Перечисление всех типов.
 * Тип uint8_t выбран в целях оптимизации памяти.
 */
enum type_t: uint8_t {
    _NONE_,
    _INT_,
    _REAL_,
    _STRING_,
    _BOOLEAN_,
    _LABEL_,
    _STRUCT_
};

/** Перечисление всех операций.
 * Типа uint8_t должно хватить на 255 операций. Такой тип
 * выбран в целях оптимизации памяти.
 */
enum operation_t: uint8_t {
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
    UNPACK, //! Распаковать данные и выполнить над ними инструкции
    ALLOC,
    DEREF,  //! Вычисляет адрес элемента массива
    COPY    //! Копирует значение на стеке
};

type_t expressionType(type_t t1, type_t t2, operation_t o);
char * typetostr(type_t t);
int typeSize(type_t t);
bool isExpr(operation_t o);
bool isUnary(operation_t o);

int operands(operation_t o);

void debugOp(operation_t op);

#endif
