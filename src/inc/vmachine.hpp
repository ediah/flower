#ifndef VMACHINE_H
#define VMACHINE_H

#include <fstream>
#include "tables.hpp"
#include "poliz.hpp"
#include "parser.hpp"
#include "stack.hpp"

#define MAXREGS 10

class VirtualMachine {
    char * base;
    char * cmd;
    int cmdNum;

    Stack stackVM;
    Stack registerVM;
    Gendarme dynamicStrings;
public:
    VirtualMachine(): base(nullptr), cmd(nullptr), cmdNum(0) {};

    void loadBIN(const char * filename);

    virtual void run(void);

    bool exec(op_t op, int * eip);

    void copy(void * x, type_t type);

    char * getString(void * x);

    void numberOp(type_t lval, type_t rval, void* f);

    template <typename lval_t, typename rval_t, typename res_t>
    void tempOp(res_t (*f) (lval_t, rval_t), type_t TYPE);

    template <class lval_t, class rval_t>
    void assign(void);

    virtual ~VirtualMachine(void);
};

#endif