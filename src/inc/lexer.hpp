#ifndef LEXER_H
#define LEXER_H

#define MAXIDENT 50

#include <iostream>
#include <fstream>

#include "cursor.hpp"
#include "tables.hpp"
#include "poliz.hpp"

class Lexer {
    std::ifstream code;
    std::ofstream bin;
    Cursor c;
    IdentTable IdTable;
    POLIZ poliz;

    // Вспомогательные функции
    int fastPow(int x, int n);
    bool readWord(char * word);
    void revert(int x);
public:
    Lexer() {
        c.line = 1;
    };

    // Загрузка исходного кода
    void load(const char * name);
    // Основная функция разбора, "program"
    void parse(void);
    void defs(void);
    void type(void);
    void variable(void);
    char * identificator(void);
    void constVal(void);
    int constInt(void);
    char * constString(void);
    float constReal(void);

    void operations(void);
    void operation(void);
    void saveLabel(char * label);
    type_t expr(void);
    type_t add(void);
    type_t mul(void);
    type_t constExpr(void);

    void condOp(void);
    void forOp(void);
    void whileOp(void);
    void breakOp(void);
    void writeOp(void);
    void gotoOp(void);
    void readOp(void);

    void finalize(void);
    void giveBIN(char * filename);

    ~Lexer() {};
};

#endif
