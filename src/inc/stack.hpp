#ifndef STACK_H
#define STACK_H

#define MAXSTACK 1024

class Stack {
    void * elem[MAXSTACK];
    int pos;

public:
    Stack(void): pos(0) {};

    void push(void * x);
    void * pop(void); // удаляет
    void * top(void); // НЕ удаляет
};

#endif
