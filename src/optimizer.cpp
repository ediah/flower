#include "optimizer.hpp"

Optimizer::Optimizer(IdentTable * IT, POLIZ * p) {
    IdTable = IT;
    poliz = p;
}

void Optimizer::optimize(void) {
    IdTable->repr();
    poliz->repr();
}