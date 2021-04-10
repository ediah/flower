#include <iostream>
#include <cstring>
#include "vmachine.hpp"
#include "exprtype.hpp"
#include "obstacle.hpp"

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
    for (int i = 0; i < cmdNum; i++) {
        if (*(bool*)(cmd + 4 + i * 5)) { // ExecBit = 1
            op = (op_t) *(int*)(cmd + i * 5);
            if (exec(op)) break;
        } else { // ExecBit = 0
            //std::cout << "Значение " << * (int *) (base + *(int*)(cmd + i * 5)) << std::endl;
            stackVM.push((void *) (base + *(int*)(cmd + i * 5)));
        }
    }
    //* (int *) (base + cmd[i * 5])
    //std::cout << * (int *) (base + cmd[10]) << std::endl;
}

bool VirtualMachine::exec(op_t op) {
    bool exitStatus = false;
    type_t lval = (type_t) ((op >> 16) & 0xFF);
    type_t rval = (type_t) ((op >>  8) & 0xFF);
    type_t rest = expressionType(lval, rval, (operation_t) (op & 0xFF));

    switch(op & 0xFF) {
        case PLUS:
            if (rest == _INT_) {
                int a = * (int *) stackVM.pop();
                int b = * (int *) stackVM.pop();
                stackVM.push(new int (a + b));
            } else if (rest == _REAL_) {
                float * r = new float (0);

                if (rval == _INT_) *r += * (int *) stackVM.pop();
                else *r += * (float *) stackVM.pop();

                if (lval == _INT_) *r += * (int *) stackVM.pop();
                else *r += * (float *) stackVM.pop();

                stackVM.push(r);
            } else {

            }
            break;
        case ASSIGN:
            if (lval == _INT_) {
                int x = * (int *) stackVM.pop();
                * (int *) stackVM.pop() = x;
            } else if (lval == _REAL_) {
                float x = * (float *) stackVM.pop();
                * (float *) stackVM.pop() = x;
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
        case MUL:
            if (rest == _INT_) {
                int a = * (int *) stackVM.pop();
                int b = * (int *) stackVM.pop();
                stackVM.push(new int (a * b));
            } else if (rest == _REAL_) {
                float * r = new float (1);

                if (rval == _INT_) *r *= * (int *) stackVM.pop();
                else *r *= * (float *) stackVM.pop();

                if (lval == _INT_) *r *= * (int *) stackVM.pop();
                else *r *= * (float *) stackVM.pop();

                stackVM.push(r);
            } else {

            }
            break;
        default:
            std::cout << "Неизвестная команда." << std::endl;
    }

    return exitStatus;
}
