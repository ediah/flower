#include <iostream>
#include <fstream>
#include "cursor.hpp"

std::ifstream & operator>>(std::ifstream & s, Cursor & x) {
    do { s >>= x; } 
    while ((x.c == ' ') || (x.c == '\n'));

    return s;
}

std::ifstream & operator>>=(std::ifstream & s, Cursor & x) {
    s.read( &(x.c), sizeof(char));
    if (x.c == '\n') ++(x.line);
    #ifdef DEBUG
    std::cout << "прочитан \"" << x.c << "\"" << std::endl;
    #endif
    return s;
}

bool Cursor::operator==(char x) const {
    return c == x;
}

bool Cursor::operator!=(char x) const {
    return c != x;
}

bool Cursor::operator<=(char x) const {
    return c <= x;
}

bool Cursor::operator>=(char x) const {
    return c >= x;
}

void Cursor::where(void) const {
    std::cout << "[строка " << line << "]: ";
}

char Cursor::symbol(void) const {
    return c;
}

void Cursor::cite(std::ifstream & s) {
    while (*this != '\n') {
        s.seekg((int)s.tellg() - 2);
        s >>= *this;
    }
    do {
        s >>= *this;
        std::cout << this->c;
    } while ((*this != '\n') && (!s.eof()));
    std::cout << std::endl;
    line -= 2;
}
