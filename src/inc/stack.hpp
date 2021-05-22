#ifndef STACK_H
#define STACK_H

#include <cstddef>
#include "exprtype.hpp"

#define MAXSTACK 1024

class Gendarme {
    void * pointers[MAXSTACK];
    type_t types[MAXSTACK];
    int pos;

public:
    Gendarme(void): pos(0) {
        for (int i = 0; i < MAXSTACK; i++) {
            pointers[i] = nullptr;
            types[i] = _NONE_;
        }
    };

    void push(void * p, type_t type);
    void burn(void);
    type_t topType(void) const;

    ~Gendarme();
};

class Stack {
    void * elem[MAXSTACK];
    int pos;

    Gendarme memControl;
public:
    Stack(bool gendarme = true): pos(0){
        for (int i = 0; i < MAXSTACK; i++) elem[i] = nullptr;
    };

    void push(void * x, type_t type = _NONE_);
    void * pop(void); // удаляет
    void * top(void) const; // НЕ удаляет
    void * get(int x) const; // НЕ удаляет
    void dump(void) const;
    type_t topType(void) const;
    bool isEmpty(void);
};

#endif
