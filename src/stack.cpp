#include <cassert>
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
    assert(pos < MAXSTACK);
    if (isEmpty()) memControl.burn();
    memControl.push(x, type);
    elem[pos++] = x;
}

void * Stack::pop(void) {
    assert(pos > 0);
    return elem[--pos];
}

void * Stack::top(void) {
    assert(pos > 0);
    return elem[pos - 1];
}

bool Stack::isEmpty(void) {
    return pos == 0;
}