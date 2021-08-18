#include "optimizer.hpp"
#include "exprtype.hpp"
#include <iostream>

Optimizer::Optimizer(IdentTable * IT, POLIZ * p) {
    IdTable = IT;
    poliz = p;
}

void Optimizer::optimize(void) {
    IdTable->repr();
    poliz->repr();

    CFG.make(poliz);
    CFG.draw();

}
