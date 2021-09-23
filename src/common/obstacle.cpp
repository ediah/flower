#include <iostream>
#include "common/obstacle.hpp"

void Obstacle::describe(void) const {
    switch (r) {
        case PANIC:
            std::cout << "Компилятор в панике." << std::endl;
            break;
        case PROG_START: expected("program"); break;
        case PROG_OPENBR: expected("{"); break;
        case FUNC_OPENBR: expected("("); break;
        case FUNC_CLOSEBR: expected(")"); break;
        case OP_CLOSEBR:
        case PROG_CLOSEBR: expected("}"); break;
        case SEMICOLON: expected(";"); break;
        case BAD_PARAMS_OPBR: expected("("); break;
        case BAD_PARAMS_CLBR:
        case EXPR_CLOSEBR: expected(")"); break;
        case BAD_INT: expected("целое число"); break;
        case BAD_REAL: expected("вещественное число"); break;
        case BAD_BOOL: expected("true или false"); break;
        case WRONG_SCOPE: 
            std::cout << "Неизвестная область программы." << std::endl;
            break;
        case BAD_IDENT:
            std::cout << "Плохой идентификатор." << std::endl;
            break;
        case BAD_STRING:
            std::cout << "Плохая строка." << std::endl;
            break;
        case TYPE_UNKNOWN:
            std::cout << "Неизвестный тип." << std::endl;
            break;
        case EXPR_BAD_TYPE:
            std::cout << "Несовпадение типов выражения." << std::endl;
            break;
        case BAD_OPERATOR:
            std::cout << "Плохой оператор." << std::endl;
            break;
        case IDENT_NOT_DEF:
            std::cout << "Идентификатор не описан." << std::endl;
            break;
        case IDENT_DUP:
            std::cout << "Повторное описание идентификатора." << std::endl;
            break;
        case BAD_IF:
            std::cout << "Выражение должно быть условным." << std::endl;
            break;
        case BAD_LABEL:
            std::cout << "Перемещаться можно только по метке." << std::endl;
            break;
        case BAD_EXPR:
            std::cout << "Неверное выражение." << std::endl;
            break;
        case BREAK_OUTSIDE_CYCLE: 
            std::cout << "Break вне цикла." << std::endl;
            break;
        case CONTINUE_OUTSIDE_CYCLE:
            std::cout << "Continue вне цикла" << std::endl;
            break;
        case BAD_STRUCT:
            std::cout << "Плохое описание структуры." << std::endl;
            break;
        case STRUCT_UNDEF:
            std::cout << "Структура не описана." << std::endl;
            break;
        case LABEL_OR_IDENT: 
            std::cout << "Этот идентификатор описан не как метка." << std::endl;
            break;
        case TOO_MUCH_PARAMS:
            std::cout << "Слишком много формальных параметров." << std::endl;
            break;
        case NOT_CALLABLE:
            std::cout << "Этот объект нельзя вызвать как функцию." << std::endl;
            break;
        case CALLABLE:
            std::cout << "Нельзя проводить арифметические операции над функцией. Возможно, Вы забыли \"()\"." << std::endl;
            break;
        case LACK_PARAMS:
            std::cout << "Недостаточно параметров." << std::endl;
            break;
        case PROCEDURE:
            std::cout << "Процедуры не поддерживаются." << std::endl;
            break;
        case BAD_PARAMS_COUNT:
            std::cout << "Неверное количество фактических параметров." << std::endl;
            break;
        case NO_RETURN:
            std::cout << "Нет выхода из функции." << std::endl;
            break;
        case NO_TYPE: expected("тип функции"); break;
        case STRUCT_IN_EXPR:
            std::cout << "Структуры не могут участвовать в выражениях." << std::endl;
            break;
        case RETURN_TYPE_MISMATCH:
            std::cout << "Неверный тип возвращаемого параметра." << std::endl;
            break;
        case MODIF_WITHOUT_TYPE:
            std::cout << "Модификатор должен использоваться с типом переменной." << std::endl;
            break;
        case BAD_TYPE:
            std::cout << "Плохой тип." << std::endl;
            break;
        default:
            std::cout << "Неизвестная ошибка." << std::endl;
            break;
    }
}

void Obstacle::expected(const char * msg) {
    std::cout << "Ожидалось получить " << msg << '.' << std::endl;
}
