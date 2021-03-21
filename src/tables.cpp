#include "tables.hpp"
#include "obstacle.hpp"
#include "exprtype.hpp"

void IdentTable::pushId(char* ident) {
    IdentTable * p = this;
    while (p->next != nullptr) p = p->next;
    name = ident;
}

void IdentTable::pushType(type_t t) {
    IdentTable * p = this;
    while (p->next != nullptr) p = p->next;
    setType(t);
}

void IdentTable::confirm(void) {
    IdentTable * newIdent = new IdentTable;
    IdentTable *p = this;
    while (p->next != nullptr) p = p->next;
    p->next = newIdent;
}

/*
ExprTable * ExprTable::newExpr(ExprTable * e1, ExprTable * e2, operation_t o) {
    ExprTable * e = new ExprTable(e1, e2, o), *p = this;
    e->setType = expressionType(e1->getType, e2->getType, o);
    e->setDet = e1->getDet() && e2->getDet();
    push(e);
    return e;
}

ExprTable * ExprTable::newConst(void * value, type_t t) {
    ExprTable * e = new ExprTable(value, nullptr, NONE), *p = this;
    e->setType = t;
    e->setDet = true;
    push(e);
    return e;
}

void ExprTable::push(ExprTable * e) {
    ExprTable *p = this;
    while (p->next != nullptr) p = p->next;
    p->next = e;
}

ExprTable::~ExprTable(void) {
    if (next != nullptr) delete next;
}
*/
