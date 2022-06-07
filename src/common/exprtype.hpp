#ifndef EXPRTYPE_H
#define EXPRTYPE_H

enum type_t {
    _NONE_,
    _INT_,
    _REAL_,
    _STRING_,
    _BOOLEAN_,
    _LABEL_,
    _STRUCT_
};

enum operation_t {
    NONE,
    INV,
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
    LOAD,
    READ,
    WRITE,
    ENDL,
    JIT,
    JMP,
    RET,
    CALL,
    STOP,
    SHARE,
    FORK,
    LOCK,
    UNPACK
};

type_t expressionType(type_t t1, type_t t2, operation_t o);
char * typetostr(type_t t);
bool isExpr(operation_t o);
bool isUnary(operation_t o);

int operands(operation_t o);

void debugOp(operation_t op);

#endif
