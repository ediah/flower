#include "dag.hpp"
#include "util.hpp"
#include <iostream>

std::vector<DAGRow *> DAGRow::created;

bool DAGRow::isLast(void) const {
    return (lvar == nullptr) && (rvar == nullptr);
}

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

bool operator==(DAGRow & a, DAGRow & b) {
    if (&a == &b) return false;

    bool ret = (a.opcode == b.opcode);
    //ret = ret && (a.lvar == b.lvar) && (a.rvar == b.rvar);


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

    if (!p.getEB()[s - 1])
        return;
    if ((operation_t)(p.getProg()[s - 1] & 0xFF) != CALL)
        return;
    
    IdentTable * paramit = reinterpret_cast<IdentTable *>(p.getProg()[s - 2]);
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
            /*
            if ((op == LOAD) && (qrow->rvar != nullptr)) {
                qrow->ident = qrow->rvar->ident;
            }
            */
        } else {
            int idx = find(changed, reinterpret_cast<IdentTable *>(p.getProg()[i]));
            if (idx != -1) {
                qrow = changed[idx].second;
            } else {
                qrow = new DAGRow;
                qrow->ident = reinterpret_cast<IdentTable *>(p.getProg()[i]);
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
        op_t op = (execBit)
                        ? opcode
                        : (op_t) ident;
        p.push(op, execBit);
    } else p.push((op_t)ident, false);
}

POLIZ DirectedAcyclicGraph::decompose(void) {
    POLIZ ret;
    std::vector<DAGRow *> asd;

    for (auto head: rows) {
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
            decompose().repr(false);
            #endif
        }
    }
}

// Это очень нагруженная функция! Как сделать лучше?
std::pair<std::pair<DAGRow *, DAGRow *>, int> DirectedAcyclicGraph::findCopies(
                                DAGRow * left, DAGRow * right, int a, int b) {

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