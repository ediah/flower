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
    MUL,
    DIV,
    MOD,
    LESS,
    GRTR,
    LESSEQ,
    GRTREQ,
    EQ,
    NEQ
};

#endif
