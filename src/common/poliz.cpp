#include <iostream>
#include "common/exprtype.hpp"
#include "common/poliz.hpp"
#include "common/obstacle.hpp"

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

POLIZ::POLIZ(const POLIZ& p) {
    if (this == &p) return;
    for (int i = 0; i < p.iter; i++) {
        prog[i] = p.prog[i];
        execBit[i] = p.execBit[i];
    }
    iter = p.iter;
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
        IdentTable * mainIT = val->getMainTable();
        mainIT->pushType(_INT_);
        mainIT->pushVal(new int (val->getOrd()));
        pushVal(mainIT->confirm());
        pushOp(_NONE_, _INT_, LOAD);
        prog[iter - 1] = (char) val->getType() << 24 | (prog[iter - 1] & 0xFFF);
    } else {
        prog[iter] = (op_t) val;
        execBit[iter] = false;
        iter++;

        checkIter();
    }
}

void POLIZ::pushOp(type_t lval, type_t rval, operation_t op){
    type_t rest = expressionType(lval, rval, op);
    prog[iter] = (char) rest << 24 | (char) lval << 16 | (char) rval << 8 | (char) op;
    execBit[iter] = true;

    #ifdef DEBUG
    std::cout << "POLIZ pushOp ";
    interpretAsOp(prog[iter]);
    std::cout << std::endl;
    #endif

    iter++;
    checkIter();
}

void POLIZ::interpretAsOp(op_t op) {
    debugOp(static_cast<operation_t>(op & 0xFF));
    std::cout << "["  << typetostr((type_t)((op >>  8) & 0xFF));
    std::cout << ", " << typetostr((type_t)((op >> 16) & 0xFF));
    std::cout << "]";
}

void POLIZ::interpretAsVal(op_t val) {
    reinterpret_cast<IdentTable *>(val)->whoami();
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
    iter = (iter <= 0) ? 0 : iter - 1;
}

void POLIZ::clear(void) {
    iter = 0;
}

void POLIZ::incIter(void) {
    iter++;
    checkIter();
}

void POLIZ::push(op_t op, bool eb) {
    #ifdef DEBUG
    std::cout << "POLIZ push ";
    if (eb)
        interpretAsOp(op);
    else
        interpretAsVal(op);
    std::cout << std::endl;
    #endif

    prog[iter] = op;
    execBit[iter] = eb;
    iter++;
    checkIter();
}

void POLIZ::checkIter(void) const {
    if (iter >= MAXCMD) {
        std::cout << "Слот для байткода переполнен. Текущее значение: ";
        std::cout << MAXCMD << ". Скомпилируйте с большим значением.\n";
        exit(1);
    }
}

bool POLIZ::endsWithCall(void) const {
    bool call = (operation_t)(prog[iter - 1] & 0xFF) == CALL;
    bool exec = execBit[iter - 1];
    return call && exec;
}

bool POLIZ::endsWithRet(void) const {
    bool call = (operation_t)(prog[iter - 1] & 0xFF) == RET;
    bool exec = execBit[iter - 1];
    return call && exec;
}