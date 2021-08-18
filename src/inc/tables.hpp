#ifndef TABLES_H
#define TABLES_H

#include <cstddef>
#include <ostream>
#include "exprtype.hpp"

class IdentTable {
    type_t valType;    // Тип идентификатора
    char * structName; // Имя структуры (если valType == _STRUCT_)
    char * name;       // Имя идентификатора
    bool def;          // Определён ли
    bool func;         // Является ли функцией
    bool reg;          // Лежит ли на регистрах
    void * val;        // Данные
    int ord;           // Номер элемента
    int params;        // Количество переменных (если функция)
    int offset;        // Позиция в байткоде

public:
    IdentTable *next;

    IdentTable(void): valType(_NONE_), structName(nullptr), name(nullptr), params(0),
        next(nullptr), def(false), func(false), reg(false), val(nullptr), ord(0), offset(0) {};
    IdentTable(const IdentTable & templateIT);
    IdentTable & operator=(const IdentTable & templateIT);
    void pushId(char * ident);
    void pushType(type_t t);
    void pushStruct(char * name);
    void pushVal(void* v);
    IdentTable * confirm(void);
    void dupType(void);
    char * getStruct(void) const;
    type_t getType(void) const;
    void setType(type_t newType);
    void * getVal(void);
    void setFunc(void);
    bool isFunc(void) const;
    void setVal(void * val);
    void setId(char * name);
    char * getId(void) const;
    int getOrd(void) const;
    void setOrd(int x);
    void onReg(void);
    bool isReg(void) const;
    void setParams(int x);
    int getParams(void) const;
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
