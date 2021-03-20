#ifndef TABLES_H
#define TABLES_H

enum type_t {
    _NONE_,
    _INT_,
    _REAL_,
    _STRING_,
    _BOOLEAN_
};

enum operation_t {
    NONE,
    PLUS,
    MINUS,
    MUL,
    DIV,
    MOD
};

class Table {
    bool determined;
    type_t valType;
public:
    Table(bool d = true, type_t t = NONE): determined(d), valType(t) {};
    void setDet(bool d): determined(d) {};
    void setType(type_t t): valType(t) {};
    bool getDet(void) { return determined; };
    type_t getType(void) { return valType; };
};

class ExprTable: public Table {
    operation_t operation;
    ExprTable *expr1;
    ExprTable *expr2;

    ExprTable *next;

public:
    ExprTable(ExprTable * e1, ExprTable * e2, operation_t o):
        operation(o), expr1(e1), expr2(e2), next(nullptr) {};
    ExprTable * newExpr(ExprTable *e1, ExprTable *e2, operation_t o);
    ExprTable * newConst(void * value, type_t t);

    ~ExprTable(void);
};

class IdentTable: public Table {
    char * name;
    ExprTable *value;

    IdentTable *next;

};

#endif
