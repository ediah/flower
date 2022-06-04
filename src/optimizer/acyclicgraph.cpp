#include "optimizer/acyclicgraph.hpp"
#include "common/exprtype.hpp"
#include "common/util.hpp"
#include <iostream>

std::vector<DAGRow *> DAGRow::created;

bool DAGRow::isLast(void) const {
    return (lvar == nullptr) && (rvar == nullptr);
}

/*
DAGRow & DAGRow::operator=(const DAGRow & dr) {
    if (this == &dr) return *this;

    ident = dr.ident;
    opcode = dr.opcode;
    prev = dr.prev;
    
    if (dr.lvar != nullptr) {
        lvar = new DAGRow;
        *lvar = *dr.lvar;
    } else lvar = nullptr;

    if (dr.rvar != nullptr) {
        rvar = new DAGRow;
        *rvar = *dr.rvar;
    } else rvar = nullptr;

    assigned = dr.assigned;

    return *this;
}

DAGRow::DAGRow(const DAGRow & dr) {
    ident = dr.ident;
    opcode = dr.opcode;
    prev = dr.prev;
    
    if (dr.lvar != nullptr) {
        lvar = new DAGRow;
        *lvar = *dr.lvar;
    } else lvar = nullptr;

    if (dr.rvar != nullptr) {
        rvar = new DAGRow;
        *rvar = *dr.rvar;
    } else rvar = nullptr;

    assigned = dr.assigned;
}
*/

bool operator==(DAGRow & a, DAGRow & b) {
    if (&a == &b) return false;

    bool ret = (a.opcode == b.opcode);

    if (a.lvar != b.lvar) {
        if ((a.lvar == nullptr) || (b.lvar == nullptr))
            return false;
        ret = ret && (*a.lvar == *b.lvar);
    }

    if (a.rvar != b.rvar) {
        if ((a.rvar == nullptr) || (b.rvar == nullptr))
            return false;
        ret = ret && (*a.rvar == *b.rvar);
    }

    if (a.isLast() && b.isLast())
        return ret && (*a.ident == *b.ident);

    if ((!a.isLast()) && (!b.isLast()))
        return ret;

    return false;
}

void DirectedAcyclicGraph::stash(POLIZ & p) {
    int s = p.getSize();

    if (s == 0) return;
    if (!p.getEB()[s - 1])
        return;
    if ((operation_t)(p.getProg()[s - 1] & 0xFF) != CALL)
        return;
    
    IdentTable * paramit = IT_FROM_POLIZ(p, s - 2);
    int paramNum = * (int *) paramit->getVal();
    stashed.clear();
    copyPOLIZ(p, stashed, s - paramNum - 2, s);
    for (int i = 0; i < paramNum + 2; i++)
        p.pop();
}

void DirectedAcyclicGraph::make(POLIZ p) {
    std::vector<std::pair<IdentTable *, DAGRow *>> changed;
    std::pair<IdentTable *, DAGRow *> rep;
    std::vector<DAGRow *> queue;
    stash(p);
    for (int i = 0; i < p.getSize(); i++) {
        DAGRow * qrow;
        if (p.getEB()[i]) {
            qrow = new DAGRow;
            operation_t op = (operation_t)(p.getProg()[i] & 0xFF);
            type_t rettype = (type_t) ((p.getProg()[i] >> 24) & 0xFF);
            qrow->type = rettype;
            int opnum = operands(op);
            if ((opnum != 0) && (queue.size() != 0)) {
                qrow->rvar = queue.back();
                queue.pop_back();
                if (qrow->rvar->prev == nullptr)
                    qrow->rvar->prev = qrow;
                opnum--;
            }
            if ((opnum != 0) && (queue.size() != 0)) {
                qrow->lvar = queue.back();
                queue.pop_back();
                if (qrow->lvar->prev == nullptr)
                    qrow->lvar->prev = qrow;
                //opnum--;
            }
            qrow->opcode = p.getProg()[i];
            queue.push_back(qrow);

            if ((op == ASSIGN) && (qrow->lvar != nullptr) && (qrow->rvar != nullptr)) {
                if (qrow->rvar->ident == nullptr)
                    qrow->rvar->ident = qrow->lvar->ident;
                rep = std::make_pair(qrow->lvar->ident, qrow->rvar);
                changed.emplace(changed.begin(), rep); 
            }
            
        } else {
            int idx = find(changed, IT_FROM_POLIZ(p, i));
            if (idx != -1) {
                qrow = changed[idx].second;
            } else {
                qrow = new DAGRow;
                qrow->ident = IT_FROM_POLIZ(p, i);
            }
            queue.push_back(qrow);
        }
    }

    for (auto it = queue.begin(); it != queue.end(); it++)
        rows.push_back(*it);
}

void DAGRow::decompose(POLIZ & p, std::vector<DAGRow *> * asd) {
    if ((ident == nullptr) || (!assigned)) {
        if (lvar != nullptr)
            lvar->decompose(p, asd);
        if (rvar != nullptr)
            rvar->decompose(p, asd);
        if (ident != nullptr) {
            asd->push_back(this);
            assigned = true;
        }

        bool execBit = (opcode != (op_t) NONE);
        type_t ltype = (lvar == NULL)
                                    ? _NONE_
                                    : lvar->type;
        type_t rtype = (rvar == NULL)
                                    ? _NONE_
                                    : rvar->type;

        op_t newOp = (char) type << 24 | (char) ltype << 16;
        newOp |= (char) rtype << 8 | (char) (opcode & 0xFF);
        op_t op = (execBit)
                        ? newOp
                        : (op_t) ident;
        p.push(op, execBit);
    } else p.push((op_t)ident, false);
}

type_t DAGRow::updateType(std::vector<type_t> * typeOnStack) {
    type_t ltype, rtype;
    
    int x, y;
    switch ((operation_t) (opcode & 0xFF)) {
        case LOAD:
            if ((!typeOnStack->empty()))
                typeOnStack->pop_back();
            std::cout << "<= " << this << " (" << typetostr(type) << ")" << std::endl;
            return type;
        /*
        case CALL:
            x = * (int *) lvar->ident->getVal();
            //y = * (int *) rvar->ident->getVal();
            for (int i = 0; i < x; i++)
                typeOnStack->pop_back();
            break;
        */
        default:
            break;
    }

    if ((lvar != nullptr)){ //&& isExpr((operation_t) (lvar->opcode & 0xFF))) {
        std::cout << "=> " << lvar << "\n";
        ltype = lvar->updateType(typeOnStack);
    //} else if (opcode != (op_t) NONE) {
    //    ltype = (type_t) ((opcode >> 16) & 0xFF);
    } else ltype = _NONE_;
    
    if ((rvar != nullptr)){ //&& isExpr((operation_t) (rvar->opcode & 0xFF))) {
        std::cout << "=> " << rvar << "\n";
        rtype = rvar->updateType(typeOnStack);
    //} else if (opcode != (op_t) NONE) {
    //    rtype = (type_t) ((opcode >> 8) & 0xFF);
    } else rtype = _NONE_;

    if (opcode != (op_t) NONE) {
        // FIXME: Убрать это!!!
        operation_t oper = (operation_t) (opcode & 0xFF);
        debugOp(oper);
        std::cout << "[" << typetostr(ltype) << ", " << typetostr(rtype) << "]\n";
        try {
            type = expressionType(ltype, rtype, oper);
        } catch (...) {
            if ((ltype == _NONE_) && (!typeOnStack->empty())) {
                ltype = typeOnStack->back();
                typeOnStack->pop_back();
            }
            if ((rtype == _NONE_) && (!typeOnStack->empty())) {
                rtype = typeOnStack->back();
                typeOnStack->pop_back();
            }
            type = expressionType(ltype, rtype, oper);
        }
        
    } else {
        if (ident != nullptr){
            type = ident->getType();
        } else
            type = _NONE_;
    }
    std::cout << "<= " << this << " (" << typetostr(type) << ")" << std::endl;
    return type;
}

POLIZ DirectedAcyclicGraph::decompose(std::vector<type_t> * typeOnStack) {
    POLIZ ret;
    std::vector<DAGRow *> asd;
    type_t t;

    for (auto head: rows) {
        t = head->updateType(typeOnStack);
        if (t != _NONE_) typeOnStack->push_back(t);
        head->decompose(ret, &asd);
        #ifdef DEBUG
        //ret.repr();
        //ret.clear();
        //std::cout << "\n";
        #endif
    }

    for (auto x: asd) {
        x->assigned = false;
    }

    copyPOLIZ(stashed, ret, 0, stashed.getSize());
    
    return ret;
}

/* FIXME: Ошибка в небезопасном преобразовании типов
 * 
 * def fun(): int {
 *   int x;
 *   real a = 0.1;
 *   x = a;
 *   return x; // <= становится "return a;"
 * }
 * 
 * Не совпадает тип возвращаемого значения с заявленным типом,
 * машина пытается прочесть float как int.
 * То есть, смотреть надо на это, как на несовпадение правого 
 * типа в ASSIGN и правой части выражения
 * (real)x =real=real= (int)a;
 * И надо добираться до места, где встречается тот ASSIGN, менять
 * его на другой тип.
 *
 * Надо понять, в какой момент происходит замена такого выражения.
 * Я искал, искал, но код не проходит по ядру этой функции, хотя
 * почему-то идёт замена x на r.
 */

void DirectedAcyclicGraph::commonSubExpr(IdentTable * IT) {
    std::pair<std::pair<DAGRow *, DAGRow *>, int> fcret;
    std::pair<DAGRow *, DAGRow *> rowc;

    for (int i = 0; i < (signed int)rows.size(); i++) {
        for (int j = i; j < (signed int)rows.size(); j++) {
            fcret = findCopies(rows[i], rows[j], i, j);
            rowc = fcret.first;
            int left = fcret.second;
            
            if (rowc.first == nullptr)
                continue;

            if ((!isExpr((operation_t)(rowc.first->opcode & 0xFF))) ||
                (!isExpr((operation_t)(rowc.first->prev->opcode & 0xFF))))
                continue;
            
            rowc.second->assigned = true;
                
            if ((operation_t)(rowc.first->prev->opcode & 0xFF) == ASSIGN) {
                if (rowc.second->prev->lvar == rowc.second)
                    rowc.second->prev->lvar = rowc.first;
                else
                    rowc.second->prev->rvar = rowc.first;
                
                if (verbose) {
                    std::cout << "Исключено подвыражение, значение было вычислено в ";
                    rowc.first->prev->lvar->ident->whoami();
                    std::cout << std::endl;
                }
                continue;
            }
            
            type_t tt = (type_t) ((rowc.first->opcode >> 24) & 0xFF);
            DAGRow * temprow = new DAGRow;
            temprow->opcode = (char) tt << 16 | (char) tt << 8 | (char) tt;
            temprow->opcode = temprow->opcode << 8  | (char) ASSIGN;
            temprow->rvar = rowc.first;
            temprow->lvar = new DAGRow;

            temprow->lvar->ident = IT->last();
            temprow->rvar->ident = IT->last();
            temprow->lvar->ident->setType(tt);
            temprow->lvar->ident->setOrd(IT->last()->getOrd() + 1);
            IT->last()->next = new IdentTable;

            rows.insert(rows.begin() + left, temprow);
            if (left == i) i++;
            j++;

            if (rowc.first->prev->lvar == rowc.first)
                rowc.first->prev->lvar = temprow->rvar;
            else
                rowc.first->prev->rvar = temprow->rvar;

            if (rowc.second->prev->lvar == rowc.second)
                rowc.second->prev->lvar = temprow->rvar;
            else
                rowc.second->prev->rvar = temprow->rvar;

            if (verbose) {
                std::cout << "Исключено подвыражение для двух выражений.\n";
            }

            #ifdef DEBUG
            std::cout << "Итерация i = " << i << " j = " << j << ":\n";
            std::vector<type_t> typeOnStack;
            decompose(&typeOnStack).repr(false);
            #endif
        }
    }
}

// Это очень нагруженная функция! Как сделать лучше?
std::pair<std::pair<DAGRow *, DAGRow *>, int> 
DirectedAcyclicGraph::findCopies(DAGRow * left, DAGRow * right, int a, int b) {

    if ((left == nullptr) && (right != nullptr))
        return findCopies(right->lvar, right->rvar, b, b);
    
    if ((left != nullptr) && (right == nullptr))
        return findCopies(left->lvar, left->rvar, a, a);
    
    if ((left == nullptr) || (right == nullptr))
        return std::make_pair(std::make_pair(nullptr, nullptr), 0);

    if ((*left == *right) && (left->prev != nullptr) &&
        (!left->isLast()) && (!right->isLast())) {
        #ifdef DEBUG
        POLIZ temp;
        std::vector<DAGRow *> asd;
        left->decompose(temp, &asd);
        for (auto x: asd) x->assigned = false;
        std::cout << "Новая копия:\n";
        temp.repr(false);
        std::cout << "\n";
        #endif
        return std::make_pair(std::make_pair(left, right), a);
    }
        
    std::pair<std::pair<DAGRow *, DAGRow *>, int> ret;
    ret = findCopies(left->lvar, right, a, b);
    if (ret.first.first != nullptr) return ret;
    ret = findCopies(left->rvar, right, a, b);
    if (ret.first.first != nullptr) return ret;
    ret = findCopies(left, right->lvar, a, b);
    if (ret.first.first != nullptr) return ret;
    ret = findCopies(left, right->rvar, a, b);
    if (ret.first.first != nullptr) return ret;

    return std::make_pair(std::make_pair(nullptr, nullptr), 0);
}

DirectedAcyclicGraph::~DirectedAcyclicGraph() {
    for (DAGRow * node: DAGRow::created)
        delete node;
    DAGRow::created.clear();
}