#ifndef POLIZ_H
#define POLIZ_H

#include "config.hpp"
#include "common/tables.hpp"
#include "common/exprtype.hpp"

#define IT_FROM_POLIZ(p, i) reinterpret_cast<IdentTable *>(p.getProg()[i])

// Размер анонимной структуры
#define PSLOT_PROG 4

union pslot {
    //TODO: Остаётся ещё 3 свободных байта.
    struct {
        type_t ltype, rtype, restype;
        operation_t opcode;
        bool execBit; // Дополнительный байт при чтении из машины
    };

    uint32_t offset; // Макс. 4 ГБ статической памяти программы

    IdentTable * val; // Для 32-х/64-х битной ОС 4/8 бит
};

class POLIZ {
    pslot prog[MAXCMD];
    bool execBit[MAXCMD];
    int iter;

public:
    POLIZ(int s = 0);

    POLIZ& operator=(const POLIZ& p);

    POLIZ(const POLIZ& p);

    void pushVal(IdentTable * val);
    void pushOp(type_t lval, type_t rval, operation_t op);
    void pop(void);
    void remove(int pos, int n);
    /** Перемешать элементы ПОЛИЗа
     * @param[in] start   Начало блока
     * @param[in] end     Конец блока
     * @param[in] n       Размер блока после фильтрации
     * @param[in] pi      Перестановка на множестве [0;n-1]
     * @param[in] reverse Читать pi с конца
     */
    void permutate(int start, int end, int n, int* pi, bool reverse = true);
    void combine(std::vector<int> pi);
    void push(pslot op, bool eb);
    void clear(void);
    void repr(bool dontBreak = false);
    int getSize(void) const;
    pslot * getProg(void);
    operation_t getOpcode(int index);  // С начала
    operation_t rgetOpcode(int index); // С конца
    IdentTable * getVal(int index);    // С начала
    IdentTable * rgetVal(int index);   // С конца
    bool * getEB(void);
    void incIter(void);
    void checkIter(void) const;
    bool endsWithCall(void) const;
    bool endsWithRet(void) const;

    void setVal(int index, IdentTable * val);

    static void interpretAsOp(pslot op);
    static void interpretAsVal(pslot val);

};

#endif
