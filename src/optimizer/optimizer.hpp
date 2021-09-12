#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include "common/tables.hpp"
#include "common/poliz.hpp"
#include "optimizer/controlflow.hpp"
#include "optimizer/acyclicgraph.hpp"

class Optimizer {
    IdentTable * IdTable;
    POLIZ * poliz;
    ControlFlowGraph CFG;
public:
    Optimizer(IdentTable * IT, POLIZ * p);

    IdentTable * optimize(bool verbose);
    void reduceConstants(void);
    void commonSubExpr(void);
};

#endif