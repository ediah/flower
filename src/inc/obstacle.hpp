#ifndef OBSTACLE_H
#define OBSTACLE_H

enum reason {
    PANIC,
    PROG_START,
    PROG_OPENBR,
    PROG_CLOSEBR,
    TYPE_UNKNOWN,
    EXPR_BAD_TYPE
};

class Obstacle {
    reason r;
public:
    Obstacle(reason x): r(x) {};

    void describe(void);
    void expected(const char * msg);
};

#endif
