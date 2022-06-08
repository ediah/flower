#ifndef STACK_H
#define STACK_H

#include <cstddef>

#include "config.hpp"
#include "common/exprtype.hpp"

/** Класс, предназначенный для освобождения неиспользуемой памяти.
 *  Во время своей работы виртуальная машина выделяет память под
 *  каждое новое значение, будь то строка или целое число. Поэтому
 *  как только это значение было присвоено некоторой переменной,
 *  жандарм освобождает всю выделенную до этого момента память.
 *  Так как машина стековая, каждый объект в её памяти живёт до
 *  тех пор, пока не покинет её стек (кроме статических переменных),
 *  поэтому вызов Gendarme::burn() произойдёт при опустошении стека.
 */
class Gendarme {
    void * pointers[MAXSTACK]; //!< Указатели на выделенную память.
    type_t types[MAXSTACK];    //!< Типы хранящихся по указателям значений.
    int pos;                   //!< Текущая позиция в массиве.

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
