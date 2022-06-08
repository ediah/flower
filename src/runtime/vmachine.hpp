#ifndef VMACHINE_H
#define VMACHINE_H

#include <fstream>
#include <vector>
#include "common/tables.hpp"
#include "common/poliz.hpp"
#include "common/stack.hpp"

class VirtualMachine {
protected:
    char * base;
    char * cmd;
    int cmdNum;

    bool inThread;

    Stack stackVM;
    Stack registerVM;
    Stack sharedVars;
    Stack offsets;
    Stack params;
    std::vector<pid_t> threads;
    std::vector<int *> pipefd;
    Gendarme dynamicStrings;
public:
    VirtualMachine(): base(nullptr), cmd(nullptr), cmdNum(0), inThread(false) {};

    void loadBIN(const char * filename);

    virtual void run(void);

    bool exec(op_t op, int * eip);

    void updateVars(void);

    static char * getString(void * x);

    void numberOp(type_t lval, type_t rval, void* f);

    template <typename lval_t, typename rval_t, typename res_t>
    void tempOp(res_t (*f) (lval_t, rval_t), type_t TYPE);

    template <class lval_t, class rval_t>
    void assign(void);

    virtual ~VirtualMachine(void);
};

#define ARITH_OPERATION(OP) { \
    if ((lval == _INT_) && (rval == _INT_)) \
        tempOp<int, int, int>( [] (int x, int y) { return x OP y; }, _INT_); \
    if ((lval == _INT_) && (rval == _REAL_)) \
        tempOp<int, float, float>( [] (int x, float y) { return x OP y; }, _REAL_); \
    if ((lval == _REAL_) && (rval == _INT_)) \
        tempOp<float, int, float>( [] (float x, int y) { return x OP y; }, _REAL_); \
    if ((lval == _REAL_) && (rval == _REAL_)) \
        tempOp<float, float, float>( [] (float x, float y) { return x OP y; }, _REAL_); \
}

#define LOGIC_OPERATION(OP) { \
    if ((lval == _INT_) && (rval == _INT_)) \
        tempOp<int, int, bool>( [] (int x, int y) { return x OP y; }, _BOOLEAN_); \
    if ((lval == _INT_) && (rval == _REAL_)) \
        tempOp<int, float, bool>( [] (int x, float y) { return x OP y; }, _BOOLEAN_); \
    if ((lval == _REAL_) && (rval == _INT_)) \
        tempOp<float, int, bool>( [] (float x, int y) { return x OP y; }, _BOOLEAN_); \
    if ((lval == _REAL_) && (rval == _REAL_)) \
        tempOp<float, float, bool>( [] (float x, float y) { return x OP y; }, _BOOLEAN_); \
}

#endif
