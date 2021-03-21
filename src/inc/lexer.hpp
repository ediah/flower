#ifndef LEXER_H
#define LEXER_H

#define MAXIDENT 50

#include <iostream>
#include <fstream>

#include "cursor.hpp"
#include "tables.hpp"

class Lexer {
    std::ifstream code;
    Cursor c;
    IdentTable IdTable;
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
    void variable(void);
    void identificator(void);
    void constVal(void);
    void constInt(void);
    void constString(void);

    void operations(void);

    // Вспомогательные функции
    bool readWord(char * word);

    ~Lexer() {};
};

#endif
