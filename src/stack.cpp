#include "stack.hpp"

void Stack::push(void * x) {
    elem[pos++] = x;
}

void * Stack::pop(void) {
    return elem[--pos];
}

void * Stack::top(void) {
    return elem[pos - 1];
}
