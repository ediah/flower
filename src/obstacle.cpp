#include <iostream>
#include "obstacle.hpp"

void Obstacle::describe(void) {
    switch (r) {
        case PANIC:
            std::cout << "Компилятор в панике." << std::endl;
            break;
        case PROG_START: expected("program"); break;
        case PROG_OPENBR: expected("{"); break;
        case OP_CLOSEBR:
        case PROG_CLOSEBR: expected("}"); break;
        case DEF_END: expected(";"); break;
        case BAD_PARAMS_OPBR: expected("("); break;
        case BAD_PARAMS_CLBR:
        case EXPR_CLOSEBR: expected(")"); break;
        case BAD_INT:
            std::cout << "Плохое целое число." << std::endl;
            break;
        case BAD_REAL:
            std::cout << "Плохое вещественное число." << std::endl;
            break;
        case BAD_BOOL: expected("true или false"); break;
        case BAD_IDENT:
            std::cout << "Плохой идентификатор." << std::endl;
            break;
        case BAD_STRING:
            std::cout << "Плохая строка." << std::endl;
            break;
        case TYPE_UNKNOWN:
        case EXPR_BAD_TYPE:
            std::cout << "Несовпадение типов выражения." << std::endl;
            break;
        case BAD_OPERATOR:
            std::cout << "Плохой оператор." << std::endl;
            break;
        case CLOSED_BOOK:
            std::cout << "Невозможно открыть книгу: на 100-й странице пропущена точка с запятой!" << std::endl;
            break;
        case IDENT_NOT_DEF:
            std::cout << "Идентификатор не описан." << std::endl;
            break;
        case BAD_IF:
            std::cout << "Выражение должно быть условным." << std::endl;
            break;
        case BAD_LABEL:
            std::cout << "Перемещаться можно только по метке." << std::endl;
            break;
        case READ_BOOL:
            std::cout << "Ввод булевского типа запрещён." << std::endl;
            break;
        case BAD_EXPR:
            std::cout << "Неверное выражение." << std::endl;
            break;
        default:
            std::cout << "Неизвестная ошибка." << std::endl;
            break;
    }
}

void Obstacle::expected(const char * msg) {
    std::cout << "Ожидалось получить " << msg << '.' << std::endl;
}
