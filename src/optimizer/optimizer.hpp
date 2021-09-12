#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include "tables.hpp"
#include "poliz.hpp"
#include "controlflow.hpp"
#include "dag.hpp"

class Optimizer {
    IdentTable * IdTable;
    POLIZ * poliz;
    ControlFlowGraph CFG;
public:
    Optimizer(IdentTable * IT, POLIZ * p);

    void optimize(bool verbose);
    void reduceConstants(void);
    void commonSubExpr(void);
};

#endif