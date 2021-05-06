#include <cassert>
#include "stack.hpp"

void Stack::push(void * x, bool del) {
    assert(pos < MAXSTACK);
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