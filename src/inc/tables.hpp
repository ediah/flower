#ifndef TABLES_H
#define TABLES_H

#include <cstddef>
#include "exprtype.hpp"

class Table {
    type_t valType;
public:
    Table(type_t t = _NONE_): valType(t) {};
    void setType(type_t t) { valType = t; };
    type_t getType(void) { return valType; };
};

/*
class ConstTable: public Table {
    operation_t operation;
    ExprTable *expr1;
    ExprTable *expr2;

    ExprTable *next;

public:
    ExprTable(ExprTable * e1, ExprTable * e2, operation_t o):
        operation(o), expr1(e1), expr2(e2), next(nullptr) {};
    ExprTable * newExpr(ExprTable *e1, ExprTable *e2, operation_t o);
    ExprTable * newConst(void * value, type_t t);

    void push(ExprTable * e);

    ~ExprTable(void);
};
*/

class IdentTable: public Table {
    char * name;
    //ExprTable *value;

    IdentTable *next;
public:
    IdentTable(char * n = NULL): name(n), next(nullptr) {};
    void pushId(char * ident);
    void pushType(type_t t);
    void confirm(void);
};

#endif
