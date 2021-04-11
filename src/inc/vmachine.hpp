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

};

#endif
