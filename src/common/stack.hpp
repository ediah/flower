#ifndef STACK_H
#define STACK_H

#include <cstddef>
#include "common/exprtype.hpp"

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
    const type_t * getTypes(void) const;
    void updateType(type_t type);

    ~Gendarme();
};

class Stack {
    void * elem[MAXSTACK];
    bool defined[MAXSTACK];
    int pos;

    Gendarme memControl;
public:
    Stack(bool gendarme = true);

    void push(void * x, type_t type = _NONE_);
    void * pop(void); // удаляет
    void * top(void) const; // НЕ удаляет
    void * get(int x) const; // НЕ удаляет
    void set(int i, void * x);
    int size(void) const;
    void * const * data(void) const;
    type_t topType(void) const;
    void updateType(type_t type);
    const type_t * getTypes(void) const;
    bool isEmpty(void) const;
    bool isDefined(void);
};

#endif
