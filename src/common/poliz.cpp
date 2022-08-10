#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include "common/exprtype.hpp"
#include "common/poliz.hpp"
#include "common/obstacle.hpp"
#include "common/tables.hpp"

POLIZ::POLIZ(int s) {
    iter = 0;

    for (int i = 0; i < MAXCMD; i++) {
        prog[i].val = nullptr;
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

IdentTable * POLIZ::getVal(int index) {
    return prog[index].val;
}

IdentTable * POLIZ::rgetVal(int index) {
    return prog[iter - index - 1].val;
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
        prog[iter - 1].restype = val->getType();
    } else {
        prog[iter].val = val;
        execBit[iter] = false;
        iter++;

        checkIter();
    }
}

void POLIZ::pushOp(type_t lval, type_t rval, operation_t op){
    type_t rest = expressionType(lval, rval, op);
    prog[iter].restype = rest;
    prog[iter].ltype = lval;
    prog[iter].rtype = rval;
    prog[iter].opcode = op;
    execBit[iter] = true;

    #ifdef DEBUG
    std::cout << "POLIZ pushOp ";
    interpretAsOp(prog[iter]);
    std::cout << std::endl;
    #endif

    iter++;
    checkIter();
}

void POLIZ::setVal(int index, IdentTable *val) {
    prog[index].val = val;
}

void POLIZ::interpretAsOp(pslot op) {
    debugOp(op.opcode);
    std::cout << "["  << typetostr(op.ltype);
    std::cout << ", " << typetostr(op.rtype);
    std::cout << "]";
}

void POLIZ::interpretAsVal(pslot val) {
    val.val->whoami();
}

pslot * POLIZ::getProg(void) {
    return prog;
}

operation_t POLIZ::getOpcode(int index) {
    return prog[index].opcode;
}

operation_t POLIZ::rgetOpcode(int index) {
    return prog[iter - index - 1].opcode;
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

void POLIZ::remove(int pos, int n) {
    assert((pos >= 0) && (pos < iter));
    assert(pos - n >= 0);
    if (n == 0) return;
    while (prog[pos].val != nullptr) {
        prog[pos] = prog[pos + n];
        execBit[pos] = execBit[pos + n];
        pos++;
    }
    iter = iter - n;
}

void POLIZ::permutate(int start, int end, int n, int* pi, bool reverse) {
    assert((start >= 0) && (start < iter));
    assert((n > 0) && (n < iter));
    // Блок перестановки должен поместиться
    assert(iter - start >= n);

    int index;
    POLIZ buff;
    for (int i = 0; i < n; i++) {
        if (reverse)
            index = start + pi[n - i - 1];
        else
            index = start + pi[i];
        
        buff.push(prog[index], execBit[index]);
    }

    for (int i = 0; i < n; i++) {
        prog[start + i] = buff.prog[i];
        execBit[start + i] = buff.execBit[i];
    }

    remove(start + n, end - start - n);
}

void POLIZ::combine(std::vector<int> pi) {
    std::vector<std::pair<int, int>> map;
    int pos = iter - 1, lastPos = pos;
    int fields = pi.size();
    int ops = 0;
    while (fields) {
        if (execBit[pos]) {
            ops += operands(prog[pos].opcode);
        } else ops--;
        if (ops == 1) {
            ops = 0;
            map.insert(map.begin(), std::pair<int, int>(pos, lastPos));
            lastPos = pos - 1;
            fields--;
        }
        pos--;
    }

    POLIZ buff;
    for (auto i: pi) {
        for (int j = map[i].first; j <= map[i].second; j++) {
            buff.push(prog[j], execBit[j]);
        }
    }

    for (int i = 0; i < buff.getSize(); i++) {
        prog[map[0].first + i] = buff.prog[i];
        execBit[map[0].first + i] = buff.execBit[i];
    }
}

void POLIZ::clear(void) {
    iter = 0;
}

void POLIZ::incIter(void) {
    iter++;
    checkIter();
}

void POLIZ::push(pslot op, bool eb) {
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
    bool call = prog[iter - 1].opcode == CALL;
    bool exec = execBit[iter - 1];
    return call && exec;
}

bool POLIZ::endsWithRet(void) const {
    bool call = prog[iter - 1].opcode == RET;
    bool exec = execBit[iter - 1];
    return call && exec;
}