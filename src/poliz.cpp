#include <iostream>
#include "poliz.hpp"
#include "obstacle.hpp"

POLIZ::POLIZ(void) {
    iter = 0;

    for (int i = 0; i < MAXCMD; i++) {
        prog[i] = (op_t) NONE;
        execBit[i] = false;
    }

}

void POLIZ::repr(void) {
    for (int i = 0; i < iter; i++) {
        if (execBit[i]) interpretAsOp(prog[i]);
        else interpretAsVal(prog[i]);
    }
}

void POLIZ::pushVal(IdentTable * val) {
    #ifdef DEBUG
    std::cout << "POLIZ pushVal";
    val->whoami();
    std::cout << std::endl;
    #endif
    prog[iter] = (op_t) val;
    execBit[iter] = false;
    iter++;
}

void POLIZ::pushOp(operation_t op){
    #ifdef DEBUG
    std::cout << "POLIZ pushOp ";
    interpretAsOp(op);
    std::cout << std::endl;
    #endif
    prog[iter] = (op_t) op;
    execBit[iter] = true;
    iter++;
}

void POLIZ::interpretAsOp(op_t op) {
    switch (op) {
        case PLUS: std::cout << "PLUS "; break;
        case MINUS: std::cout << "MINUS "; break;
        case MUL: std::cout << "MUL "; break;
        case DIV: std::cout << "DIV "; break;
        case LOR: std::cout << "LOR "; break;
        case LAND: std::cout << "LAND "; break;
        case LNOT: std::cout << "LNOT "; break;
        case MOD: std::cout << "MOD "; break;
        case LESS: std::cout << "LESS "; break;
        case GRTR: std::cout << "GRTR "; break;
        case LESSEQ: std::cout << "LESSEQ "; break;
        case GRTREQ: std::cout << "GRTREQ "; break;
        case EQ: std::cout << "EQ "; break;
        case NEQ: std::cout << "NEQ "; break;
        case ASSIGN: std::cout << "ASSIGN "; break;
        case GET: std::cout << "GET "; break;
        case STOP: std::cout << "STOP "; break;
        default: throw Obstacle(PANIC);
    }
}

void POLIZ::interpretAsVal(op_t val) {
    ((IdentTable *) val)->whoami();
}
