#ifndef POLIZ_H
#define POLIZ_H

#define MAXCMD 1024

#include "tables.hpp"
#include "exprtype.hpp"

typedef long long int op_t;

class POLIZ {
    op_t prog[MAXCMD];
    bool execBit[MAXCMD];
    int iter;

public:
    POLIZ(void);

    void pushVal(IdentTable * val);
    void pushOp(operation_t op);
    void repr(void);
    void interpretAsOp(op_t op);
    void interpretAsVal(op_t op);
};

#endif
