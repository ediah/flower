#include <iostream>
#include <fstream>
#include "cursor.hpp"

std::ifstream & operator>>(std::ifstream & s, Cursor & x) {
    char nextChar;
    while (true) {
        s >> nextChar;
        x.pos++;

        if (nextChar == '\n') {
            x.line++;
            x.pos = 1;
        }
        if ((nextChar != ' ') && (nextChar != '\n')) break;
    }
    x.c = nextChar;
    return s;
}

std::ifstream & operator>>=(std::ifstream & s, Cursor & x) {
    s >> x.c;
    return s;
}

bool Cursor::operator==(char x) {
    return c == x;
}

bool Cursor::operator!=(char x) {
    return c != x;
}

void Cursor::where(void) {
    std::cout << '[' << line << ", " << pos << "]: ";
}

char Cursor::symbol(void) {
    return c;
}
