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
    bool verbose;
public:
    Optimizer(IdentTable * IT, POLIZ * p, bool verb);

    IdentTable * optimize();
    void reduceConstants(void);
    void commonSubExpr(void);
};

#endif