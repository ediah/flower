#ifndef CURSOR_H
#define CURSOR_H

#include <iostream>

class Cursor {
    char c;
public:
    int line;
    Cursor(int l = 1): c('\0'), line(l) { std::cout << std::boolalpha; };
    // Чтение с пропуском пробелов и переносов строки
    friend std::ifstream & operator>>(std::ifstream & s, Cursor & x);
    // Чтение один в один
    friend std::ifstream & operator>>=(std::ifstream & s, Cursor & x);
    bool operator==(char x) const;
    bool operator!=(char x) const;
    bool operator<=(char x) const;
    bool operator>=(char x) const;

    void where(void) const;
    void cite(std::ifstream & s);
    char symbol(void) const;
};

#endif
