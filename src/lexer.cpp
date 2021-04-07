#include <iostream>
#include "lexer.hpp"
#include "obstacle.hpp"
#define C_IS_ALPHA ((c >= 'a') && (c <= 'z') || (c >= 'A') && (c <= 'Z'))
#define C_IS_NUM ((c >= '0') && (c <= '9'))

void Lexer::load(const char * name) {
    code.open(name);
}

void Lexer::parse(void) {
    char word[8] = "program";
    code >> c;

    try {
        for (int i = 0; i < 6; i++) {
            if (c != word[i]) throw Obstacle(PROG_START);
            code >>= c;
        }
        if (c != word[6]) throw Obstacle(PROG_START);

        code >> c;
        if (c != '{') throw Obstacle(PROG_OPENBR);

        defs();
        operations();

        if (c != '}') throw Obstacle(PROG_CLOSEBR);
    }
    catch (Obstacle & o) {
        c.where();
        o.describe();
        exit(-1);
    }

}

void Lexer::defs(void) {
    while (true) {
        try { type(); }
        catch(Obstacle o) { break; }

        do {
            variable();
        } while (c == ',');

        if (c != ';') throw Obstacle(DEF_END);
    }
}

void Lexer::type(void) {
    code >> c;

    switch(c.symbol()) {
        case 'i':
            if (readWord("nt"))
                IdTable.pushType(_INT_);
            else throw Obstacle(TYPE_UNKNOWN);
            break;
        case 's':
            if (readWord("tring"))
                IdTable.pushType(_STRING_);
            else throw Obstacle(TYPE_UNKNOWN);
            break;
        default:
            throw Obstacle(TYPE_UNKNOWN);
    }
}

bool Lexer::readWord(char * word) {
    bool r = true;
    for (int i = 0; word[i] != '\0'; i++) {
        code >>= c;
        if (c != word[i]) r = false;
    }
    return r;
}

void Lexer::variable(void) {
    IdTable.dupType();
    identificator();
    if (c == '=') constVal();
    IdTable.confirm();
}

void Lexer::identificator(void) {
    char * ident = new char[MAXIDENT];
    int i = 0;
    code >> c;
    while (C_IS_ALPHA) {
        ident[i++] = c.symbol();
        code >>= c;
    }
    ident[i] = '\0';
    std::cout << ident << std::endl;
    if (c.symbol() == ' ') code >> c;
    IdTable.pushId(ident);
}

void Lexer::constVal(void) {
    try { constInt(); }
    catch(...) {
        try { constString(); }
        catch (Obstacle & o) {
            c.where();
            o.describe();
            exit(-1);
        }
    }
}

void Lexer::constInt(void) {
    int x = 0, sign = 1;
    code >> c;
    if (c == '-') {
        sign = -1;
        code >> c;
    } else if (c == '+') code >> c;

    if (!C_IS_NUM) throw Obstacle(BAD_NUM);

    do {
        x = 10 * x + ( c.symbol() - '0');
        code >>= c;
    } while (C_IS_NUM);
    x = x * sign;

    int * v = new int; *v = x;
    IdTable.pushVal(v);

    std::cout << "Прочитано число " << x << std::endl;
}

void Lexer::constString(void) {
    if (c != '\"') throw Obstacle(BAD_STRING);

    int len = 0, start = code.tellg();
    while (code.peek() != '\"') {
        code.seekg(++len + start);
        if (code.eof()) throw Obstacle(BAD_STRING);
    }
    code.seekg(start);

    char *x = new char[len + 1];

    for (int i = 0; i < len; i++) {
        code >>= c;
        x[i] = c.symbol();
    }

    x[len] = '\0';

    code >>= c;
    code >> c;

    IdTable.pushVal(x);
    std::cout << "Прочитана строка \"" << x << '\"'<< std::endl;
}

void Lexer::operations(void) {

}
