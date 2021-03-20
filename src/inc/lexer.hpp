#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <fstream>

#include "cursor.hpp"
#include "tables.hpp"

class Lexer {
    std::ifstream code;
    Cursor c;
    IdentTable idTable;
public:
    Lexer() {
        c.line = 1;
        c.pos = 1;
    };

    // Загрузка исходного кода
    void load(const char * name);
    // Основная функция разбора, "program"
    void parse(void);
    void defs(void);
    void type(void);
    void operations(void);
    void identificator(void);
    void constVal(void);

    // Вспомогательные функции


    ~Lexer() {};
};

#endif
