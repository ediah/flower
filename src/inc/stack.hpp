#ifndef STACK_H
#define STACK_H

#include <cstddef>

#define MAXSTACK 1024

class Stack {
    void * elem[MAXSTACK];
    int pos;

public:
    Stack(void): pos(0) {
        for (int i = 0; i < MAXSTACK; i++) elem[i] = NULL;
    };

    void push(void * x);
    void * pop(void); // удаляет
    void * top(void); // НЕ удаляет
    bool isEmpty(void);
};

#endif
