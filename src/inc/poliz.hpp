#ifndef POLIZ_H
#define POLIZ_H

#define MAXCMD 1024

#include "tables.hpp"
#include "exprtype.hpp"

typedef long long op_t;

class POLIZ {
    op_t prog[MAXCMD];
    bool execBit[MAXCMD];
    int iter;

public:
    POLIZ(void);

    void pushVal(IdentTable * val);
    void pushOp(type_t lval, type_t rval, operation_t op);
    void pop(void);
    void repr(bool dontBreak = false);
    void interpretAsOp(op_t op);
    void interpretAsVal(op_t val);
    int getSize(void) const;
    op_t * getProg(void);
    bool * getEB(void);

};

#endif
