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

    void push(void * x) {
        Stack * p = this;
        while (p->next != nullptr) p = p->next;
        p->next = new Stack;
        p->next->elem = x;
    }

    void * pop(void) {
        void * r;
        Stack * p = this;
        if (p->next != nullptr) {
            while (p->next->next != nullptr) p = p->next;
            r = p->next->elem;
            p->next->~Stack();
            p->next = nullptr;
        } else {
            r = p->elem;
            elem = nullptr;
        }
        return r;
    }
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

    bool exec(op_t op);

};

#endif
