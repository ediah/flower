#ifndef CURSOR_H
#define CURSOR_H

#include <iostream>

class Cursor {
    char c;
public:
    int line;
    Cursor(int l = 1): line(l) {};
    // Чтение с пропуском пробелов и переносов строки
    friend std::ifstream & operator>>(std::ifstream & s, Cursor & x);
    // Чтение один в один
    friend std::ifstream & operator>>=(std::ifstream & s, Cursor & x);
    bool operator==(char x);
    bool operator!=(char x);
    bool operator<=(char x);
    bool operator>=(char x);

    void where(void);
    char symbol(void);
};

#endif
