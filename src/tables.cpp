#include "tables.hpp"
#include "obstacle.hpp"
type_t expressionType(type_t t1, type_t t2, operation_t o) {
    type_t r = _NONE_;
    if (o == PLUS) {
            if (t1 == _INT_) {
                if (t2 == _INT_) r = _INT_;
                else if (t2 == _REAL_) r = _REAL_;
                else throw Obstacle(EXPR_BAD_TYPE);
            } else if (t1 == _REAL_) {
                if ((t2 == _INT_) || (t2 == _REAL_)) r = _REAL_;
                else throw Obstacle(EXPR_BAD_TYPE);
            } else if (t1 == _STRING_) {
                if (t2 == _STRING_) r = _STRING_;
                else throw Obstacle(EXPR_BAD_TYPE);
            } else throw Obstacle(EXPR_BAD_TYPE);
    }
    return r;
}

ExprTable * ExprTable::newExpr(ExprTable * e1, ExprTable * e2, operation_t o) {
    ExprTable * e = new ExprTable(e1, e2, o), *p = this;
    e.setType = expressionType(e1->getType, e2->getType, o);
    e.setDet = e1->getDet() && e2->getDet();
    while (p->next != nullptr) p = p->next;
    p->next = e;
    return e;
}

ExprTable * ExprTable::newConst(void * value, type_t t) {
    return newExpr(value, nullptr, NONE, t);
}

ExprTable::~ExprTable(void) {
    if (next != nullptr) delete next;
}
