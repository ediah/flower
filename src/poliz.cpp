#include <iostream>
#include "poliz.hpp"
#include "obstacle.hpp"

POLIZ::POLIZ(int s) {
    iter = 0;

    for (int i = 0; i < MAXCMD; i++) {
        prog[i] = (op_t) NONE;
        execBit[i] = false;
    }

}

POLIZ& POLIZ::operator=(const POLIZ& p) {
    if (this == &p) return *this;
    for (int i = 0; i < p.iter; i++) {
        prog[i] = p.prog[i];
        execBit[i] = p.execBit[i];
    }
    iter = p.iter;
    return *this;
}

void POLIZ::repr(bool dontBreak) {
    for (int i = 0; i < iter; i++) {
        std::cout << i << ") ";
        if (execBit[i]) interpretAsOp(prog[i]);
        else interpretAsVal(prog[i]);
        if (!dontBreak)
            std::cout << "\n";
        else
            std::cout << "\\n";
    }
}

void POLIZ::pushVal(IdentTable * val) {
    #ifdef DEBUG
    std::cout << "POLIZ pushVal";
    val->whoami();
    if (val->isReg())
        std::cout << " REGISTER";
    std::cout << std::endl;
    #endif
    if (val->isReg()) {
        val->pushType(_INT_);
        val->pushVal(new int (val->getOrd()));
        pushVal(val->confirm());
        pushOp(_NONE_, _NONE_, LOAD);
    } else {
        prog[iter] = (op_t) val;
        execBit[iter] = false;
        iter++;
    }
}

void POLIZ::pushOp(type_t lval, type_t rval, operation_t op){
    #ifdef DEBUG
    std::cout << "POLIZ pushOp ";
    interpretAsOp(op);
    std::cout << std::endl;
    #endif
    type_t rest = expressionType(lval, rval, op);
    prog[iter] = (char) rest << 24 | (char) lval << 16 | (char) rval << 8 | (char) op;
    execBit[iter] = true;
    iter++;
}

void POLIZ::interpretAsOp(op_t op) {
    switch (op & 0xFF) {
        case INV: std::cout << "INV "; break;
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
        case STOP: std::cout << "STOP "; break;
        case WRITE: std::cout << "WRITE "; break;
        case ENDL: std::cout << "ENDL "; break;
        case READ: std::cout << "READ "; break;
        case JIT: std::cout << "JIT "; break;
        case JMP: std::cout << "JMP "; break;
        case RET: std::cout << "RET "; break;
        case CALL: std::cout << "CALL "; break;
        case REGR: std::cout << "REGR "; break;
        case LOAD: std::cout << "LOAD "; break;
        case NONE: std::cout << "NONE "; break;
        default: throw Obstacle(PANIC);
    }
}

void POLIZ::interpretAsVal(op_t val) {
    ((IdentTable *) val)->whoami();
}

op_t * POLIZ::getProg(void) {
    return prog;
}

bool * POLIZ::getEB(void) {
    return execBit;
}

int POLIZ::getSize(void) const {
    return iter;
}

void POLIZ::pop(void) {
    iter = (iter == 0) ? 0 : iter - 1;
}

void POLIZ::clear(void) {
    iter = 0;
}

void POLIZ::incIter(void) {
    iter++;
}

void POLIZ::push(op_t op, bool eb) {
    prog[iter] = op;
    execBit[iter] = eb;
    iter++;
}
