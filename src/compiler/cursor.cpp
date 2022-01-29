#include <iostream>
#include <fstream>
#include <sstream>
#include "compiler/cursor.hpp"

std::ifstream & operator>>(std::ifstream & s, Cursor & x) {
    do { 
        s >>= x; 
        // открывающие скобки комментариев
        if (x == '/') {
            s >>= x;
            if (x == '*') {
                // комментарий
                while (true) {
                    s >>= x;
                    // закрывающие скобки комментариев
                    if (x == '*') {
                        s >>= x;
                        if (x == '/') {
                            s >> x;
                            break;
                        }
                    }
                }
            } else {
                s.seekg((int)s.tellg() - 2);
                s >>= x;
            }
        }

    } while (((x.c == ' ') || (x.c == '\n')) && (!s.eof()));

    return s;
}

std::ifstream & operator>>=(std::ifstream & s, Cursor & x) {
    s.read( &(x.c), sizeof(char));
    #ifdef DEBUG
    std::cout << "cursor " << x.c << std::endl;
    #endif
    if (x.c == '\n') ++(x.line);
    ++(x.pos);
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

bool Cursor::cite(std::ifstream & s) {
    std::ostringstream os;
    while ((*this != '\n') && ((int)s.tellg() > 1)) {
        s.seekg((int)s.tellg() - 2);
        s >>= *this;
    }
    if ((int)s.tellg() <= 1) os << this->c;
    int l, r = 0;
    do {
        s >>= *this;
        os << this->c;
        if (*this == '{') l++;
        if (*this == '}') r++;
    } while ((*this != '\n') && (!s.eof()));
    std::cout << os.str() << std::endl;
    line -= 2;
    return l > r;
}
