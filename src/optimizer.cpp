#include "optimizer.hpp"

Optimizer::Optimizer(IdentTable * IT, POLIZ * p) {
    IdTable = IT;
    poliz = p;
}

void Optimizer::optimize(void) {
    IdTable->repr();
    poliz->repr();

    constProp();

}

void Optimizer::constProp(void) {
    op_t * prog = poliz->getProg();
    bool * eb = poliz->getEB();
    int n = poliz->getSize();

    for (int i = 0; i < n; i++) {
        
    }
}