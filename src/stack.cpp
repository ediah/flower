#include <cassert>
#include <iostream>
#include "stack.hpp"
#include "exprtype.hpp"

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

bool Stack::isEmpty(void) {
    return pos == 0;
}

bool Stack::isDefined(void) {
    assert(pos >= 0);
    return defined[pos - 1];
}

void Stack::dump(void) const {
    for (int i = 0; i < pos; i++)
        std::cout << i << ") " << elem[i] << std::endl;
}

type_t Stack::topType(void) const {
    return memControl.topType();
}

type_t Gendarme::topType(void) const {
    return types[pos - 1];
}
