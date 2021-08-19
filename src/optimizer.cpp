#include "optimizer.hpp"
#include "exprtype.hpp"
#include <iostream>

Optimizer::Optimizer(IdentTable * IT, POLIZ * p) {
    IdTable = IT;
    poliz = p;
}

void Optimizer::optimize(void) {
    #ifdef DEBUG
    IdTable->repr();
    poliz->repr();
    #endif

    CFG.make(poliz);
    CFG.draw();

}
