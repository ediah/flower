#include <iostream>
#include <fstream>
#include "cursor.hpp"

std::ifstream & operator>>(std::ifstream & s, Cursor & x) {
    do {
        s >>= x;

        if (x.c == '\n') {
            x.line++;
            x.pos = 1;
        }
    } while ((x.c == ' ') || (x.c == '\n'));

    return s;
}

std::ifstream & operator>>=(std::ifstream & s, Cursor & x) {
    s.get(x.c);
    std::cout << "прочитан \"" << x.c << "\"" << std::endl;
    x.pos++;
    return s;
}

bool Cursor::operator==(char x) {
    return c == x;
}

bool Cursor::operator!=(char x) {
    return c != x;
}

bool Cursor::operator<=(char x) {
    return c <= x;
}

bool Cursor::operator>=(char x) {
    return c >= x;
}

void Cursor::where(void) {
    std::cout << '[' << line << ", " << pos << "]: ";
}

char Cursor::symbol(void) {
    return c;
}
