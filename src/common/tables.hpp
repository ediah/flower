#ifndef TABLES_H
#define TABLES_H

#include <cstddef>
#include <ostream>
#include <vector>
#include "common/exprtype.hpp"

class IdentTable {
    type_t valType;    // Тип идентификатора
    char * structName; // Имя структуры (если valType == _STRUCT_)
    char * name;       // Имя идентификатора
    char * fadedName;  // Для отладки
    bool def;          // Определён ли
    bool func;         // Является ли функцией
    bool reg;          // Лежит ли на регистрах
    void * val;        // Данные
    int ord;           // Номер элемента
    int params;        // Количество переменных (если функция)
    int offset;        // Позиция в байткоде
    bool shared;
    bool array;
    int arraySize;
    IdentTable * mainTable;

public:
    IdentTable *next;

    IdentTable(void);
    IdentTable(const IdentTable & templateIT);
    IdentTable & operator=(const IdentTable & templateIT);
    friend bool operator==(IdentTable & a, IdentTable & b);

    void pushId(char * ident);
    void pushType(type_t t);
    void pushStruct(char * name);
    void pushVal(void* v);

    void setType(type_t newType);
    void setFunc(void);
    void setVal(void * val);
    void setId(char * name);
    void setOrd(int x);
    void setReg(bool x);
    void setParams(int x);
    void setOffset(int x);
    void setShared(void);
    void setMainTable(IdentTable* table);
    void setStruct(char * name);
    void setArray(int size = 0);

    char * getStruct(void) const;
    type_t getType(void) const;
    void * getVal(void) const;
    char * getId(void) const;
    int getOrd(void) const;
    int getParams(void) const;
    int getOffset(void) const;
    IdentTable* getMainTable(void);
    int getFieldShift(void) const;
    int getArray(void) const;

    bool isFunc(void) const;
    bool isDef(void) const;
    bool isReg(void) const;
    bool isShared(void) const;
    bool isStatic(void) const; // Статический массив
    bool isArray(void) const;

    void whoami();
    void repr(void);
    void fade(void);
    void dupType(void);
    IdentTable * last(void);
    IdentTable * confirm(void);
    IdentTable * deleteLabels(void);
    void writeValToStream(std::ostream & s);
    IdentTable * getIT(char * name, bool autodel = true);
    int typeSize(void) const;

    bool checkBounds(int index);

    ~IdentTable();
};

class StructTable {
    char * name;
    IdentTable fields;
    StructTable * next;
public:
    StructTable(void): name(nullptr), next(nullptr) {};

    void pushName(char * name);
    void pushField(type_t type, char * name, char * structName, bool shared = false);
    StructTable * confirm(void);
    StructTable * last(void);

    StructTable * getStruct(char * name);
    IdentTable & getFields(void);
    std::vector<type_t> getTypes(char * name);

    ~StructTable();
};

#endif
