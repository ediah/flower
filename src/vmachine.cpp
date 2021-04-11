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
            //std::cout << "Значение " << * (int *) (base + *(int*)(cmd + i * 5)) << std::endl;
            stackVM.push((void *) (base + *(int*)(cmd + *eip * 5)));
        }
        *eip += 1;
    }
    //* (int *) (base + cmd[i * 5])
    //std::cout << * (int *) (base + cmd[10]) << std::endl;
}

bool VirtualMachine::exec(op_t op, int * eip) {
    bool exitStatus = false;
    type_t lval = (type_t) ((op >> 16) & 0xFF);
    type_t rval = (type_t) ((op >>  8) & 0xFF);
    type_t rest = expressionType(lval, rval, (operation_t) (op & 0xFF));

    switch(op & 0xFF) {
        case PLUS:
            if (rest == _INT_) {
                int b = * (int *) stackVM.pop();
                int a = * (int *) stackVM.pop();
                stackVM.push(new int (a + b));
            } else if (rest == _REAL_) {
                float * r = new float (0);

                if (rval == _INT_) *r += * (int *) stackVM.pop();
                else *r += * (float *) stackVM.pop();

                if (lval == _INT_) *r += * (int *) stackVM.pop();
                else *r += * (float *) stackVM.pop();

                stackVM.push(r);
            } else if (rest == _STRING_){
                char * b = (char *) stackVM.pop();
                char * a = (char *) stackVM.pop();

                int alen = 0;
                int blen = 0;
                for (; a[alen] != '\0'; ++alen);
                for (; b[alen] != '\0'; ++blen);

                char * c = new char [alen + blen + 1];
                for (int i = 0; i < alen; ++i)
                    c[i] = a[i];
                for (int i = 0; i < blen; ++i)
                    c[alen + i] = b[i];

                c[alen + blen] = '\0';

                stackVM.push(c);
            } else throw Obstacle(PANIC);
            break;
        case MINUS:
            if (rest == _INT_) {
                int b = * (int *) stackVM.pop();
                int a = * (int *) stackVM.pop();
                stackVM.push(new int (a - b));
            } else if (rest == _REAL_) {
                float * r = new float (0);

                if (rval == _INT_) *r -= * (int *) stackVM.pop();
                else *r -= * (float *) stackVM.pop();

                if (lval == _INT_) *r -= * (int *) stackVM.pop();
                else *r -= * (float *) stackVM.pop();

                stackVM.push(r);
            } else throw Obstacle(PANIC);
            break;
        case MUL:
            if (rest == _INT_) {
                int b = * (int *) stackVM.pop();
                int a = * (int *) stackVM.pop();
                stackVM.push(new int (a * b));
            } else if (rest == _REAL_) {
                float * r = new float (1);

                if (rval == _INT_) *r *= * (int *) stackVM.pop();
                else *r *= * (float *) stackVM.pop();

                if (lval == _INT_) *r *= * (int *) stackVM.pop();
                else *r *= * (float *) stackVM.pop();

                stackVM.push(r);
            } else throw Obstacle(PANIC);
            break;
        case DIV:
            if (rest == _INT_) {
                int b = * (int *) stackVM.pop();
                int a = * (int *) stackVM.pop();
                stackVM.push(new int (a / b));
            } else if (rest == _REAL_) {
                float * r = new float (1);

                if (rval == _INT_) *r /= * (int *) stackVM.pop();
                else *r /= * (float *) stackVM.pop();

                if (lval == _INT_) *r /= * (int *) stackVM.pop();
                else *r /= * (float *) stackVM.pop();

                stackVM.push(r);
            } else throw Obstacle(PANIC);
            break;
        case MOD:
            if (rest == _INT_) {
                int b = * (int *) stackVM.pop();
                int a = * (int *) stackVM.pop();
                stackVM.push(new int (a % b));
            } else throw Obstacle(PANIC);
            break;
        case INV:
            if (rest == _INT_) {
                int a = * (int *) stackVM.pop();
                stackVM.push(new int (-a));
            } else if (rest == _REAL_) {
                float a = * (float *) stackVM.pop();
                stackVM.push(new float (-a));
            } else throw Obstacle(PANIC);
        case ASSIGN:
            if (lval == _INT_) {
                if (rval == _INT_) {
                    int x = * (int *) stackVM.pop();
                    * (int *) stackVM.pop() = x;
                } else if (rval == _REAL_) {
                    float x = * (float *) stackVM.pop();
                    * (int *) stackVM.pop() = x;
                }
            } else if (lval == _REAL_) {
                if (rval == _INT_) {
                    int x = * (int *) stackVM.pop();
                    * (float *) stackVM.pop() = x;
                } else if (rval == _REAL_) {
                    float x = * (float *) stackVM.pop();
                    * (float *) stackVM.pop() = x;
                }
            } else {

            }
            break;
        case STOP:
            exitStatus = true;
            break;
        case WRITE:
            if (rval == _INT_) {
                int x = * (int *) stackVM.pop();
                std::cout << x << std::endl;
            } else if (rval == _REAL_) {
                float x = * (float *) stackVM.pop();
                std::cout << x << std::endl;
            } else {

            }
            break;
        case JIT:
            if ((lval == _BOOLEAN_) && (rval == _INT_)) {
                int offset = * (int *) stackVM.pop();
                bool condition = * (bool *) stackVM.pop();

                if (condition) *eip = offset - 1;
            } else throw Obstacle(PANIC);
            break;
        default:
            std::cout << "Неизвестная команда." << std::endl;
            exit(-1);
    }

    return exitStatus;
}
