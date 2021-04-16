#include <iostream>
#include <cstring>
#include "vmachine.hpp"
#include "exprtype.hpp"
#include "obstacle.hpp"

void Stack::push(void * x) {
    Stack * p = this;
    while (p->next != nullptr) p = p->next;
    p->next = new Stack;
    p->next->elem = x;
}

void * Stack::pop(void) {
    void * r;
    Stack * p = this;
    if (p->next != nullptr) {
        while (p->next->next != nullptr) p = p->next;
        r = p->next->elem;
        p->next->~Stack();
        p->next = nullptr;
    } else {
        r = p->elem;
        elem = nullptr;
    }
    return r;
}

void VirtualMachine::loadBIN(char * filename) {
    std::ifstream bin;
    bin.open(filename, std::ios_base::binary | std::ios_base::in);

    bin.seekg(0, std::ios::end);
    int size = bin.tellg();
    base = new char [size];
    bin.seekg(0, std::ios::beg);
    bin.read(base, size * sizeof(char));

    cmd = base + base[0];
    cmdNum = (size - base[0]) / (sizeof(int) + sizeof(bool));
}

void VirtualMachine::run(void) {
    op_t op;
    int * eip = new int (0);
    while (*eip < cmdNum) {
        if (*(bool*)(cmd + 4 + *eip * 5)) { // ExecBit = 1
            op = (op_t) *(int*)(cmd + *eip * 5);
            if (exec(op, eip)) break;
        } else { // ExecBit = 0
            //std::cout << "Значение " << * (int *) (base + *(int*)(cmd + *eip * 5)) << std::endl;
            stackVM.push((void *) (base + *(int*)(cmd + *eip * 5)));
        }
        *eip += 1;
    }
    delete eip;
}

template <class lval_t, class rval_t, class res_t>
void VirtualMachine::tempOp(res_t (*f) (lval_t, rval_t)) {
    rval_t b = * (rval_t *) stackVM.pop();
    lval_t a = * (lval_t *) stackVM.pop();
    stackVM.push(new res_t ( f(a, b) ));
}

template <class lval_t, class rval_t>
void VirtualMachine::assign(void) {
    rval_t x = * (rval_t *) stackVM.pop();
    * (lval_t *) stackVM.pop() = x;
}

inline char * VirtualMachine::getString(void * x) {
    return ( ((char **) x)[0] == nullptr) ? (char *) x + sizeof(char*) : * (char **) x;
}

#define ARITH_OPERATION(OP) { \
    if ((lval == _INT_) && (rval == _INT_)) \
        tempOp<int, int, int>( [] (int x, int y) { return x OP y; } ); \
    if ((lval == _INT_) && (rval == _REAL_)) \
        tempOp<int, float, float>( [] (int x, float y) { return x OP y; } ); \
    if ((lval == _REAL_) && (rval == _INT_)) \
        tempOp<float, int, float>( [] (float x, int y) { return x OP y; } ); \
    if ((lval == _REAL_) && (rval == _REAL_)) \
        tempOp<float, float, float>( [] (float x, float y) { return x OP y; } ); \
}

#define LOGIC_OPERATION(OP) { \
    if ((lval == _INT_) && (rval == _INT_)) \
        tempOp<int, int, bool>( [] (int x, int y) { return x OP y; } ); \
    if ((lval == _INT_) && (rval == _REAL_)) \
        tempOp<int, float, bool>( [] (int x, float y) { return x OP y; } ); \
    if ((lval == _REAL_) && (rval == _INT_)) \
        tempOp<float, int, bool>( [] (float x, int y) { return x OP y; } ); \
    if ((lval == _REAL_) && (rval == _REAL_)) \
        tempOp<float, float, bool>( [] (float x, float y) { return x OP y; } ); \
}

bool VirtualMachine::exec(op_t op, int * eip) {
    bool exitStatus = false;
    type_t lval = (type_t) ((op >> 16) & 0xFF);
    type_t rval = (type_t) ((op >>  8) & 0xFF);
    type_t rest = expressionType(lval, rval, (operation_t) (op & 0xFF));

    switch(op & 0xFF) {
        case PLUS:
            if ((rest == _INT_) || (rest == _REAL_)) {
                ARITH_OPERATION(+)
            }

            if (rest == _STRING_){
                char * b = getString(stackVM.pop());
                char * a = getString(stackVM.pop());

                int alen = 0;
                int blen = 0;
                for (; a[alen] != '\0'; ++alen);
                for (; b[blen] != '\0'; ++blen);

                char * c = new char [alen + blen + 1];
                for (int i = 0; i < alen; ++i)
                    c[i] = a[i];
                for (int i = 0; i < blen; ++i)
                    c[alen + i] = b[i];

                c[alen + blen] = '\0';
                stackVM.push(c);
            }
            break;
        case MINUS: ARITH_OPERATION(-) break;
        case MUL:   ARITH_OPERATION(*) break;
        case DIV:   ARITH_OPERATION(/) break;
        case MOD:
            if (rest == _INT_) {
                int b = * (int *) stackVM.pop();
                int a = * (int *) stackVM.pop();
                stackVM.push(new int (a % b));
            }
            break;
        case INV:
            if (rest == _INT_) {
                int a = * (int *) stackVM.pop();
                stackVM.push(new int (-a));
            } else {
                float a = * (float *) stackVM.pop();
                stackVM.push(new float (-a));
            }
        case ASSIGN:
            if (lval == _INT_) {
                if (rval == _INT_) assign<int, int>();
                else assign<int, float>();
            } else if (lval == _REAL_) {
                if (rval == _INT_) assign<float, int>();
                else assign<float, float>();
            } else if (lval == _STRING_){
                char * b = (char *) stackVM.pop();
                char * a = (char *) stackVM.pop();
                memcpy(a, &b, sizeof(void*));
            }
            break;
        case STOP:
            exitStatus = true;
            break;
        case WRITE:
            if (rval == _INT_) {
                int x = * (int *) stackVM.pop();
                std::cout << x;
            } else if (rval == _REAL_) {
                float x = * (float *) stackVM.pop();
                std::cout << x;
            } else if (rval == _STRING_) {
                char * x = getString(stackVM.pop());
                std::cout << x;
            }
            break;
        case ENDL:
            std::cout << std::endl;
            break;
        case JIT:
            if ((lval == _BOOLEAN_) && (rval == _INT_)) {
                int offset = * (int *) stackVM.pop();
                bool condition = * (bool *) stackVM.pop();

                if (condition) *eip = offset - 1;
            } else throw Obstacle(PANIC);
            break;
        case JMP:
            if ((lval == _NONE_) && (rval == _NONE_)) {
                int offset = * (int *) stackVM.pop();
                *eip = offset - 1;
            } else throw Obstacle(PANIC);
            break;
        case LESS:
            if ((lval == _STRING_) && (rval == _STRING_)) {

            } else {
                LOGIC_OPERATION(<)
            }
            break;
        case GRTR:
            if ((lval == _STRING_) && (rval == _STRING_)) {

            } else {
                LOGIC_OPERATION(>)
            }
            break;
        case LESSEQ: LOGIC_OPERATION(<=) break;
        case GRTREQ: LOGIC_OPERATION(>=) break;
        case EQ: LOGIC_OPERATION(==) break;
        case NEQ: LOGIC_OPERATION(!=) break;

        case READ:
            if (rval == _INT_) {
                std::cin >> * (int *) stackVM.pop();
            } else if (rval == _REAL_) {
                float x = * (float *) stackVM.pop();
                std::cout << x << std::endl;
            } else if (rval == _STRING_) {
                char * x = getString(stackVM.pop());
                std::cout << x << std::endl;
            }
            break;

        case LAND:
        case LNOT:
            if (rest == _BOOLEAN_) {
                bool a = * (bool *) stackVM.pop();
                stackVM.push(new bool (!a));
            }
            break;
        case LOR:

        default:
            std::cout << "Неизвестная команда." << std::endl;
            exit(-1);
    }

    return exitStatus;
}
