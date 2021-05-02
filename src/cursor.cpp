#include <iostream>
#include <fstream>
#include <sstream>
#include "cursor.hpp"

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
    std::ostringstream os;
    while (*this != '\n') {
        s.seekg((int)s.tellg() - 2);
        s >>= *this;
    }
    do {
        s >>= *this;
        os << this->c;
    } while ((*this != '\n') && (!s.eof()));
    std::cout << os.str() << std::endl;
    line -= 2;
}
