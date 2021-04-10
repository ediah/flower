#include "obstacle.hpp"
#include "exprtype.hpp"

type_t expressionType(type_t t1, type_t t2, operation_t o) {
    type_t r = _NONE_;
    if (o == PLUS) {
        if (t1 == _INT_) {
            if ((t2 == _INT_) || (t2 == _REAL_)) r = t2;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else if (t1 == _REAL_) {
            if ((t2 == _INT_) || (t2 == _REAL_)) r = _REAL_;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else if (t1 == _STRING_) {
            if (t2 == _STRING_) r = _STRING_;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else if (t1 == _NONE_) {
            if ((t2 == _INT_) || (t2 == _REAL_)) r = t2;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else throw Obstacle(EXPR_BAD_TYPE);
    } else if (o == MINUS) {
        if (t1 == _INT_) {
            if ((t2 == _INT_) || (t2 == _REAL_)) r = t2;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else if (t1 == _REAL_) {
            if ((t2 == _INT_) || (t2 == _REAL_)) r = _REAL_;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else if (t1 == _NONE_) {
            if ((t2 == _INT_) || (t2 == _REAL_)) r = t2;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else throw Obstacle(EXPR_BAD_TYPE);
    } else if ((o == MUL) || (o == DIV)) {
        if (t1 == _INT_) {
            if ((t2 == _INT_) || (t2 == _REAL_)) r = t2;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else if (t1 == _REAL_) {
            if ((t2 == _INT_) || (t2 == _REAL_)) r = _REAL_;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else throw Obstacle(EXPR_BAD_TYPE);
    } else if (o == MOD) {
        if (t1 == _INT_) {
            if (t2 == _INT_) r = _INT_;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else throw Obstacle(EXPR_BAD_TYPE);
    } else if ((o == LESS) || (o == GRTR) || (o == EQ) || (o == NEQ)) {
        if ((t1 == _INT_) || (t1 == _REAL_)) {
            if ((t2 == _INT_) || (t2 == _REAL_)) r = _BOOLEAN_;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else if (t1 == _STRING_) {
            if (t2 == _STRING_) r = _BOOLEAN_;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else throw Obstacle(EXPR_BAD_TYPE);
    } else if ((o == LESSEQ) || (o == GRTREQ)) {
        if (((t1 == _INT_) || (t1 == _REAL_)) &&
            ((t2 == _INT_) || (t2 == _REAL_))) r = _BOOLEAN_;
        else throw Obstacle(EXPR_BAD_TYPE);
    } else if ((o == LOR) || (o == LAND)) {
        if ((t1 == _BOOLEAN_) && (t2 == _BOOLEAN_)) r = _BOOLEAN_;
        else throw Obstacle(EXPR_BAD_TYPE);
    } else if (o == LNOT) {
        if ((t1 == _NONE_) && (t2 == _BOOLEAN_)) r = _BOOLEAN_;
        else throw Obstacle(EXPR_BAD_TYPE);
    } else if (o == ASSIGN) {
        if ((t1 == _INT_) || (t1 == _REAL_)) {
            if ((t2 == _INT_) || (t2 == _REAL_)) r = t1;
            else throw Obstacle(EXPR_BAD_TYPE);
        } else if ((t1 == _BOOLEAN_) && (t2 == _BOOLEAN_))
            r = _BOOLEAN_;
        else if ((t1 == _STRING_) && (t2 == _STRING_))
            r = _STRING_;
        else throw Obstacle(EXPR_BAD_TYPE);
    } else if ((o != STOP) && (o != WRITE))
        throw Obstacle(PANIC);
    return r;
}

char * typetostr(type_t t) {
    switch(t) {
        case _NONE_: return "NONE"; break;
        case _INT_:  return "INT";  break;
        case _REAL_: return "REAL"; break;
        case _STRING_: return "STRING"; break;
        case _BOOLEAN_: return "BOOLEAN"; break;
    }
    throw Obstacle(PANIC);
}
