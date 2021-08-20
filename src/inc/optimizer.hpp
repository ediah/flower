#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "tables.hpp"
#include "poliz.hpp"
#include "controlflow.hpp"

class Optimizer {
    IdentTable * IdTable;
    POLIZ * poliz;
    ControlFlowGraph CFG;
public:
    Optimizer(IdentTable * IT, POLIZ * p);

    void optimize(bool verbose);
    void constProp(void);
};

#endif