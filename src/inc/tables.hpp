#ifndef TABLES_H
#define TABLES_H

#define DEBUG

#include <cstddef>
#include "exprtype.hpp"

class IdentTable {
    type_t valType;
    char * name;
    bool def;
    void * val;

    IdentTable *next;
public:
    IdentTable(type_t t = _NONE_, char * n = NULL): valType(t),
        name(n), next(nullptr), def(false), val(nullptr) {};
    void pushId(char * ident);
    void pushType(type_t t);
    void pushVal(void* v);
    void confirm(void);
    void dupType(void);
    IdentTable * last(void);

    ~IdentTable();
};

#endif
