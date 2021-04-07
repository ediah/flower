#include <iostream>
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
void IdentTable::confirm(void) {
    IdentTable * l = last();

    #ifdef DEBUG
    std::cout << "Создан новый объект: " << typetostr(l->valType);
    std::cout << ' ' << l->name << " = ";
    if (l->def) {
        switch (l->valType) {
            case _INT_:
                std::cout << * (int*) l->val; break;
            case _REAL_:
                std::cout << * (float*) l->val; break;
            case _STRING_:
                std::cout << (char*) l->val; break;
            case _BOOLEAN_:
                std::cout << * (bool*) l->val; break;
        }
    }
    std::cout << std::endl;
    #endif

    IdentTable * newIdent = new IdentTable;
    l->next = newIdent;
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

IdentTable::~IdentTable() {
    delete name;

    if (def) {
        switch (valType) {
            case _INT_:
                delete (int*) val;
            case _REAL_:
                delete (float*) val;
            case _STRING_:
                delete (char*) val;
            case _BOOLEAN_:
                delete (bool*) val;
        }
    }

    if (next != nullptr) delete next;
}
