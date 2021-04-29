#ifndef OBSTACLE_H
#define OBSTACLE_H

enum reason {
    PANIC,
    PROG_START,
    PROG_OPENBR,
    PROG_CLOSEBR,
    TYPE_UNKNOWN,
    EXPR_BAD_TYPE,
    EXPR_CLOSEBR,
    READ_BOOL,
    DEF_END,
    BAD_EXPR,
    BAD_INT,
    BAD_REAL,
    BAD_BOOL,
    BAD_IF,
    BAD_LABEL,
    BAD_IDENT,
    BAD_STRING,
    BAD_OPERATOR,
    OP_CLOSEBR,
    BAD_PARAMS_OPBR,
    BAD_PARAMS_CLBR,
    SEMICOLON,
    IDENT_NOT_DEF,
    IDENT_DUP,
    BREAK_OUTSIDE_CYCLE
};

class Obstacle {
public:
    reason r;
    explicit Obstacle(reason x): r(x) {};

    void describe(void);
    void expected(const char * msg);
};

#endif
