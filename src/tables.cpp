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
    l->next->ord = l->ord + 1;

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

    while ((p->name == nullptr) || (strcmp(p->name, name) != 0)) {
        if (p->next->valType != _NONE_) p = p->next;
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
            case _INT_: case _LABEL_:
                std::cout << * (int*) val; break;
            case _REAL_:
                std::cout << * (float*) val; break;
            case _STRING_:
                std::cout << (char*) val; break;
            case _BOOLEAN_:
                std::cout << * (bool*) val; break;
            default:
                std::cout << "[неизвестен]"; break;
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

void IdentTable::setId(char * name) {
    this->name = name;
}

void * IdentTable::getVal(void) {
    return val;
}

void IdentTable::setVal(void * val) {
    this->val = val;
    def = true;
}

int IdentTable::ordNum(void) {
    return ord;
}

void IdentTable::setOffset(int x) {
    offset = x;
}

int IdentTable::getOffset(void) {
    return offset;
}

void IdentTable::writeValToStream(std::ostream & s) {
    if (!def) {
        switch (valType) {
            case _INT_: case _LABEL_:
                val = new int (0); break;
            case _REAL_:
                val = new float (0); break;
            case _STRING_:
                val = new char ('\0'); break;
            case _BOOLEAN_:
                val = new bool (false); break;
            default:
                throw Obstacle(PANIC);
        }
    }
    switch (valType) {
        case _INT_: case _LABEL_:
            s.write((char*)val, sizeof(int)); break;
        case _REAL_:
            s.write((char*)val, sizeof(float)); break;
        case _STRING_:
            s.write((char*)val, sizeof(char) * (strlen((char*)val) + 1)); break;
        case _BOOLEAN_:
            s.write((char*)val, sizeof(bool)); break;
        default:
            throw Obstacle(PANIC);
    }
}

IdentTable::~IdentTable() {
    if (name != nullptr) delete [] name;

    /*
    if (def) {
        writeToStream(std::cout);
    }
    */
    if (next != nullptr) delete next;
}
