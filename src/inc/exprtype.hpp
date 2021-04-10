#ifndef EXPRTYPE_H
#define EXPRTYPE_H

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
    LOR,
    MUL,
    DIV,
    LAND,
    LNOT,
    MOD,
    LESS,
    GRTR,
    LESSEQ,
    GRTREQ,
    EQ,
    NEQ,
    ASSIGN,
    WRITE,
    STOP,
};

type_t expressionType(type_t t1, type_t t2, operation_t o);
char * typetostr(type_t t);

#endif
