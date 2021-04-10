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
    int ord;
    int offset;

public:
    IdentTable *next;

    IdentTable(type_t t = _NONE_, char * n = NULL): valType(t),
        name(n), next(nullptr), def(false), val(nullptr), ord(0), offset(0) {};
    void pushId(char * ident);
    void pushType(type_t t);
    void pushVal(void* v);
    IdentTable * confirm(void);
    void dupType(void);
    type_t getType(void);
    void * getVal(void);
    IdentTable * getIT(char * name);
    IdentTable * last(void);
    void whoami(void);
    void repr(void);
    int ordNum(void);
    void setOffset(int x);
    int getOffset(void);
    void writeValToStream(std::ostream & s);

    ~IdentTable();
};

#endif
