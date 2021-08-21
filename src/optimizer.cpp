#include "optimizer.hpp"
#include "exprtype.hpp"
#include <iostream>

Optimizer::Optimizer(IdentTable * IT, POLIZ * p) {
    IdTable = IT;
    poliz = p;
}

void Optimizer::optimize(bool verbose) {
    CFG.make(poliz);
    CFG.draw("compiled");

    if (verbose) CFG.info();

    /* Тут должны быть оптимизационные процедуры */

    CFG.decompose(IdTable, poliz);

    #ifdef DEBUG
    CFG.clear();
    CFG.make(poliz);
    CFG.draw("optimized");

    if (verbose) CFG.info();

    IdTable->repr();
    poliz->repr();
    #endif    
}
