#ifndef VMACHINE_H
#define VMACHINE_H

#include <fstream>
#include <vector>
#include <unordered_map>
#include "runtime/program.hpp"
#include "common/tables.hpp"
#include "common/poliz.hpp"
#include "common/stack.hpp"
#include "config.hpp"

class VirtualMachine {
protected:
    Program program;
    int progSize = 0;

    bool inThread = false;

    Stack stackVM;
    Stack registerVM;
    Stack offsets;
    Stack params;
    std::unordered_map<void*, int> sharedVars;
    std::vector<pid_t> threads;
    std::vector<int *> pipefd;
    std::vector<void *> allocated;
    Gendarme dynamicStrings;
public:
    void loadBIN(const char * filename);

    virtual void run(void);
    bool execNext(Program::cell * lastOp = nullptr);

    bool exec(Program::cell op, int * eip);

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
    if ((op.ltype == _INT_) && (op.rtype == _INT_)) \
        tempOp<int, int, int>( [] (int x, int y) { return x OP y; }, _INT_); \
    if ((op.ltype == _INT_) && (op.rtype == _REAL_)) \
        tempOp<int, float, float>( [] (int x, float y) { return x OP y; }, _REAL_); \
    if ((op.ltype == _REAL_) && (op.rtype == _INT_)) \
        tempOp<float, int, float>( [] (float x, int y) { return x OP y; }, _REAL_); \
    if ((op.ltype == _REAL_) && (op.rtype == _REAL_)) \
        tempOp<float, float, float>( [] (float x, float y) { return x OP y; }, _REAL_); \
}

#define LOGIC_OPERATION(OP) { \
    if ((op.ltype == _INT_) && (op.rtype == _INT_)) \
        tempOp<int, int, bool>( [] (int x, int y) { return x OP y; }, _BOOLEAN_); \
    if ((op.ltype == _INT_) && (op.rtype == _REAL_)) \
        tempOp<int, float, bool>( [] (int x, float y) { return x OP y; }, _BOOLEAN_); \
    if ((op.ltype == _REAL_) && (op.rtype == _INT_)) \
        tempOp<float, int, bool>( [] (float x, int y) { return x OP y; }, _BOOLEAN_); \
    if ((op.ltype == _REAL_) && (op.rtype == _REAL_)) \
        tempOp<float, float, bool>( [] (float x, float y) { return x OP y; }, _BOOLEAN_); \
}

#endif
