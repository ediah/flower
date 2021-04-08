#include <iostream>
#include <cstring>
#include "tables.hpp"
#include "obstacle.hpp"
#include "exprtype.hpp"

// Получаем последний объект списка
IdentTable * IdentTable::last(void) {
    IdentTable * p = this;
    while (p->next != nullptr) p = p->next;
    return p;
}

// Сохраняем идентификатор переменной
void IdentTable::pushId(char* ident) {
    last()->name = ident;
}

// Сохраняем тип переменной
void IdentTable::pushType(type_t t) {
    last()->valType = t;
}

// Сохраняем значение переменной
void IdentTable::pushVal(void* v) {
    IdentTable * l = last();
    l->val = v;
    l->def = true;
}

// Всё, что могло быть известно, мы узнали. Переключаемся на новый объект.
IdentTable * IdentTable::confirm(void) {
    IdentTable * l = last();

    #ifdef DEBUG
    std::cout << "Создан новый объект: ";
    l->whoami();
    std::cout << std::endl;
    #endif

    IdentTable * newIdent = new IdentTable;
    l->next = newIdent;

    return l;
}

// Дублирование типа для выражений вида:
// int a, b, c;
void IdentTable::dupType(void) {
    IdentTable * p = this;
    if (last()->valType == _NONE_) {
        while (p->next->next != nullptr) p = p->next;
        p->next->valType = p->valType;
    }
}

type_t IdentTable::getType(void) {
    return valType;
}

IdentTable * IdentTable::getIT(char * name) {
    IdentTable * p = this;

    while (!strcmp(p->name, name)){
        if (p->next != nullptr) p = p->next;
        else throw Obstacle(IDENT_NOT_DEF);
    }

    return p;
}

void IdentTable::whoami(void) {

    std::cout << '{' << typetostr(valType) << ' ';
    if (name != nullptr)
        std::cout << name << " = ";
    if (def) {
        switch (valType) {
            case _INT_:
                std::cout << * (int*) val; break;
            case _REAL_:
                std::cout << * (float*) val; break;
            case _STRING_:
                std::cout << (char*) val; break;
            case _BOOLEAN_:
                std::cout << * (bool*) val; break;
        }
    } else std::cout << "(не определён)";
    std::cout << '}';

}

void IdentTable::repr(void) {
    IdentTable * p = this;
    std::cout << "IdentTable:" << std::endl;
    while (p->next != nullptr) {
        std::cout << p << " ";
        p->whoami();
        std::cout << std::endl;
        p = p->next;
    }
}

IdentTable::~IdentTable() {
    if (name != nullptr) delete name;

    if (def) {
        switch (valType) {
            case _INT_:
                delete (int*) val; break;
            case _REAL_:
                delete (float*) val; break;
            case _STRING_:
                delete [] (char*) val; break;
            case _BOOLEAN_:
                delete (bool*) val; break;
        }
    }

    if (next != nullptr) delete next;
}
