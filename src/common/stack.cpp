#include <cassert>
#include <iostream>
#include "common/obstacle.hpp"
#include "common/stack.hpp"
#include "common/exprtype.hpp"

void Gendarme::push(void * p, type_t type) {
    pointers[pos] = p;
    types[pos] = type;
    pos++;
}

void Gendarme::burn(void) {
    pos--;
    while (pos >= 0) {
        switch (types[pos]) {
            case _INT_: 
                delete (int*) pointers[pos]; break;
            case _REAL_: 
                delete (float*) pointers[pos]; break;
            case _STRING_:
                delete [] (char *) pointers[pos]; break;
            case _BOOLEAN_:
                delete (bool*) pointers[pos]; break;
        }
        pos--;
    }
    pos = 0;
}

Gendarme::~Gendarme(void) {
    burn();
}

Stack::Stack(bool gendarme) {
    pos = 0;
    for (int i = 0; i < MAXSTACK; i++) {
        elem[i] = nullptr;
        defined[i] = false;
    }
}

void Stack::push(void * x, type_t type) {
    #ifdef DEBUG
    std::cout << "PUSH " << x << '(';
    switch (type) {
        case _INT_:
            std::cout << * (int *) x;
            break;
        case _REAL_:
            std::cout << * (float *) x;
            break;
        case _BOOLEAN_:
            std::cout << * (bool *) x;
            break;
        case _STRING_:
            std::cout << (char *) x;
            break;
        default:
            std::cout << "???";
            break;
    }
    std::cout << ')' << std::endl;
    #endif

    assert(pos < MAXSTACK);
    if (isEmpty()) memControl.burn();
    memControl.push(x, type);
    elem[pos++] = x;
}

void * Stack::pop(void) {
    assert(pos > 0);
    return elem[--pos];
}

void * Stack::top(void) const {
    assert(pos >= 0);
    return elem[pos - 1];
}

void * Stack::get(int x) const {
    assert(pos - x >= 0);
    return elem[pos - x - 1];
}

void Stack::set(int i, void * x) {
    assert(pos - i > 0);
    elem[pos - i - 1] = x;
}

int Stack::size() const {
    return pos;
}

void * const * Stack::data(void) const {
    return elem;
}

const type_t * Stack::getTypes(void) const {
    return memControl.getTypes();
}

bool Stack::isEmpty(void) const {
    return pos == 0;
}

type_t Stack::topType(void) const {
    return memControl.topType();
}

type_t Gendarme::topType(void) const {
    assert(pos > 0);
    return types[pos - 1];
}

const type_t * Gendarme::getTypes(void) const {
    return types;
}

void Stack::updateType(type_t type) {
    memControl.updateType(type);
}

void Gendarme::updateType(type_t type) {
    types[pos] = type;
}