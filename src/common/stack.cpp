#include <cassert>
#include <iostream>
#include "common/stack.hpp"
#include "common/obstacle.hpp"
#include "common/exprtype.hpp"

void Gendarme::push(void * p, type_t type) {
    assert(pos < MAXSTACK);
    pointers[pos] = p;
    types[pos] = type;
    pos++;
}

void Gendarme::lock(void) {
    minPos = pos;
}

void Gendarme::burn(void) {
    pos--;
    while (pos >= minPos) {
        #ifdef DEBUG
        std::cout << "Burn " << pointers[pos] << std::endl;
        #endif
        switch (types[pos]) {
            case _INT_: 
                delete    static_cast<int *> ( pointers[pos] ); break;
            case _REAL_: 
                delete    static_cast<float*>( pointers[pos] ); break;
            case _STRING_:
                delete [] static_cast<char *>( pointers[pos] ); break;
            case _BOOLEAN_:
                delete    static_cast<bool *>( pointers[pos] ); break;
        }
        pos--;
    }
    pos = minPos;
}

Gendarme::~Gendarme(void) {
    minPos = 0;
    burn();
}

Stack::Stack(bool gendarme) {
    pos = 0;
    minPos = 0;
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
            std::cout << * static_cast<int *>( x );
            break;
        case _REAL_:
            std::cout << * static_cast<float *>( x );
            break;
        case _BOOLEAN_:
            std::cout << * static_cast<bool *>( x );
            break;
        case _STRING_:
            std::cout << static_cast<char *>( x );
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

void Stack::lock(void) {
    minPos = pos;
    memControl.lock();
}

void * Stack::pop(void) {
    assert(pos > 0);
    pos--;
    if (pos < minPos) lock();
    return elem[pos];
}

void * Stack::top(void) const {
    assert(pos > 0);
    return elem[pos - 1];
}

void * Stack::get(int x) const {
    assert(pos - x > 0);
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
    #ifdef DEBUG
    std::cout << "isEmpty: " << pos << ", " << minPos << std::endl;
    #endif
    return pos == minPos;
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