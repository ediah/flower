#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "tables.hpp"
#include "poliz.hpp"

class Optimizer {
    IdentTable * IdTable;
    POLIZ * poliz;
public:
    Optimizer(IdentTable * IT, POLIZ * p);

    void optimize(void);
};

#endif