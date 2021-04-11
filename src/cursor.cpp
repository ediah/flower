#include <iostream>
#include <fstream>
#include "cursor.hpp"

std::ifstream & operator>>(std::ifstream & s, Cursor & x) {
    do {
        s.get(x.c);
        if (x.c == '\n') ++(x.line);
        #ifdef DEBUG
        std::cout << "прочитан \"" << x.c << "\"" << std::endl;
        #endif

    } while ((x.c == ' ') || (x.c == '\n'));

    return s;
}

std::ifstream & operator>>=(std::ifstream & s, Cursor & x) {
    s.get(x.c);
    #ifdef DEBUG
    std::cout << "прочитан \"" << x.c << "\"" << std::endl;
    #endif
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
    std::cout << "[строка " << line << "]: ";
}

char Cursor::symbol(void) {
    return c;
}
