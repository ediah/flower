#ifndef TABLES_H
#define TABLES_H

#include <cstddef>
#include "exprtype.hpp"

class IdentTable {
    type_t valType;
    char * structName;
    char * name;
    bool def;
    void * val;
    int ord;
    int offset; // для бинарки

public:
    IdentTable *next;

    IdentTable(void): valType(_NONE_), structName(nullptr), name(nullptr), 
        next(nullptr), def(false), val(nullptr), ord(0), offset(0) {};
    IdentTable(IdentTable & templateIT);
    void pushId(char * ident);
    void pushType(type_t t);
    void pushStruct(char * name);
    void pushVal(void* v);
    IdentTable * confirm(void);
    void dupType(void);
    char * getStruct(void) const;
    type_t getType(void) const;
    void * getVal(void);
    void setVal(void * val);
    void setId(char * name);
    char * getId(void) const;
    IdentTable * getIT(char * name, bool autodel = true);
    IdentTable * last(void);
    void whoami(void);
    void repr(void);
    void setOffset(int x);
    int getOffset(void) const;
    void writeValToStream(std::ostream & s);

    ~IdentTable();
};

class StructTable {
    char * name;
    IdentTable fields;
    StructTable * next;
public:
    StructTable(void): name(nullptr), next(nullptr) {};

    void pushName(char * name);
    void pushField(type_t type, char * name, char * structName);
    StructTable * confirm(void);
    StructTable * last(void);

    StructTable * getStruct(char * name);
    IdentTable & getFields(void);

    ~StructTable();
};

#endif
