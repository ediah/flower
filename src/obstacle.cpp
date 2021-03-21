#include <iostream>
#include "obstacle.hpp"

void Obstacle::describe(void) {
    switch (r) {
        case PANIC:
            std::cout << "Компилятор в панике." << std::endl;
            break;
        case PROG_START: expected("program"); break;
        case PROG_OPENBR: expected("{"); break;
        case PROG_CLOSEBR: expected("}"); break;
        case DEF_END: expected(";"); break;
        case BAD_NUM:
            std::cout << "Плохое число." << std::endl;
            break;
        case BAD_STRING:
            std::cout << "Плохая строка." << std::endl;
            break;
        case TYPE_UNKNOWN:
        case EXPR_BAD_TYPE:
            std::cout << "Несовпадение типов выражения." << std::endl;
            break;
        default:
            std::cout << "Неизвестная ошибка." << std::endl;
            break;
    }
}

void Obstacle::expected(const char * msg) {
    std::cout << "Ожидалось получить " << msg << '.' << std::endl;
}
