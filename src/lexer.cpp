#include <iostream>
#include "lexer.hpp"
#include "obstacle.hpp"

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

        identificator();

        if (c == '=') constVal();
    }
}

void Lexer::type(void) {
    char wordInt[3] = "nt";
    char wordString[6] = "tring";

    code >> c;

    switch(c.symbol()) {
        case 'i':
            for (int i = 0; i < 2; i++) {
                code >>= c;
                if (c != wordInt[i]) throw Obstacle(TYPE_UNKNOWN);
            }
            break;
        case 's':
            for (int i = 0; i < 5; i++) {
                code >>= c;
                if (c != wordString[i]) throw Obstacle(TYPE_UNKNOWN);
            }
            break;
        default:
            throw Obstacle(TYPE_UNKNOWN);
    }
}

void Lexer::identificator(void) {

}

void Lexer::constVal(void) {

}

void Lexer::operations(void) {

}
