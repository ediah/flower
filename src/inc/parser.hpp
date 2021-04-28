#ifndef PARSER_H
#define PARSER_H

#define MAXIDENT 50

#include <iostream>
#include <fstream>

#include "cursor.hpp"
#include "tables.hpp"
#include "poliz.hpp"
#include "stack.hpp"

class Parser {
    std::ifstream code;
    std::ofstream bin;
    Cursor c;
    IdentTable IdTable;
    POLIZ poliz;

    Stack exits;
    bool ok;

    // Вспомогательные функции
    int fastPow(int x, int n);
    bool readWord(char * word);
    void revert(int x);
public:
    Parser(): ok(true) {
        c.line = 1;
    };

    // Загрузка исходного кода
    void load(std::string name);
    // Основная функция разбора, "program"
    void parse(void);
    void defs(void);
    bool type(void);
    void variable(void);
    char * identificator(void);
    void constVal(void);
    int constInt(void);
    char * constString(void);
    float constReal(void);
    bool constBool(void);

    void operations(void);
    void operation(void);
    IdentTable * saveLabel(char * label, int addr);
    type_t expr(void);
    type_t andExpr(void);
    type_t boolExpr(void);
    type_t add(void);
    type_t mul(void);
    type_t constExpr(void);
    IdentTable * cycleparam(void);

    void condOp(void);
    void forOp(void);
    void whileOp(void);
    void breakOp(void);
    void writeOp(void);
    void gotoOp(void);
    void readOp(void);

    void finalize(void);
    void giveBIN(char * filename);

    ~Parser(void);
};

#endif
