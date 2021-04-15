#ifndef VMACHINE_H
#define VMACHINE_H

#include <fstream>
#include "tables.hpp"
#include "poliz.hpp"
#include "lexer.hpp"

class Stack {
    void * elem;
    Stack * next;

public:
    Stack(): elem(nullptr), next(nullptr) {};

    void push(void * x);
    void * pop(void);
};

class VirtualMachine {
    char * base;
    char * cmd;
    int cmdNum;

    Stack stackVM;
public:
    //VirtualMachine();

    void loadBIN(char * filename);

    void run(void);

    bool exec(op_t op, int * eip);
    /*
    template <typename lval_t, typename rval_t, typename res_t>
    void tempOp(res_t (*f) (lval_t, rval_t));

    template <typename lval_t, typename rval_t, typename res_t>
    res_t plusOp(lval_t a, rval_t b);
    */
};

#endif
