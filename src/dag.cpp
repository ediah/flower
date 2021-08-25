#include "dag.hpp"
#include "util.hpp"
#include <iostream>

std::vector<DAGRow *> DAGRow::deleted;

bool operator==(DAGRow & a, DAGRow & b) {
    return (a.lvar == b.lvar) || (a.rvar == b.rvar) || (a.opcode == b.opcode);
}

void DirectedAcyclicGraph::stash(POLIZ & p) {
    int s = p.getSize();

    if (!p.getEB()[s - 1])
        return;
    if ((operation_t)(p.getProg()[s - 1] & 0xFF) != CALL)
        return;
    
    IdentTable * paramit = (IdentTable *) p.getProg()[s - 2];
    int paramNum = * (int *) paramit->getVal();
    stashed.clear();
    copyPOLIZ(p, stashed, s - paramNum - 2, s);
    for (int i = 0; i < paramNum + 2; i++)
        p.pop();
}

void DirectedAcyclicGraph::make(POLIZ p) {
    DAGRow * nr;
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
                opnum--;
            }
            if ((opnum != 0) && (queue.size() != 0)) {
                qrow->lvar = queue.back();
                queue.pop_back();
                //opnum--;
            }
            qrow->opcode = p.getProg()[i];
            queue.push_back(qrow);

            if ((op == ASSIGN) && (qrow->lvar != nullptr) && (qrow->rvar != nullptr)) {
                qrow->rvar->ident = qrow->lvar->ident;
                rep = std::make_pair(qrow->lvar->ident, qrow->rvar);
                changed.emplace(changed.begin(), rep); 
            }
        } else {
            int idx = find(changed, (IdentTable *) p.getProg()[i]);
            if (idx != -1) {
                qrow = changed[idx].second;
            } else {
                qrow = new DAGRow;
                qrow->ident = (IdentTable *) p.getProg()[i];
            }
            queue.push_back(qrow);
        }
    }

    for (auto it = queue.begin(); it != queue.end(); it++)
        rows.push_back(*it);
}

void DAGRow::decompose(POLIZ & p) {
    if ((ident == nullptr) || (!assigned)) {
        if (lvar != nullptr)
            lvar->decompose(p);
        if (rvar != nullptr)
            rvar->decompose(p);
        if (ident != nullptr)
            assigned = true;

        bool execBit = (opcode != (op_t) NONE);
        op_t op = (execBit)
                        ? opcode
                        : (op_t) ident;
        p.push(op, execBit);
    } else p.push((op_t)ident, false);
}

POLIZ DirectedAcyclicGraph::decompose(void) {
    POLIZ ret;

    for (auto head: rows) {
        head->decompose(ret);
        #ifdef DEBUG
        ret.repr();
        ret.clear();
        std::cout << "\n";
        #endif
    }

    copyPOLIZ(stashed, ret, 0, stashed.getSize());
       

    return ret;
}

void DirectedAcyclicGraph::commonSubExpr(void) {

}

void DirectedAcyclicGraph::replace(int from, int to) {

}

void DirectedAcyclicGraph::remove(int idx) {

}

DAGRow::~DAGRow() {
    if (find(deleted, lvar) == -1) {
        deleted.push_back(lvar);
        delete lvar;
    }
        
    if (find(deleted, rvar) == -1) {
        deleted.push_back(rvar);
        delete rvar;
    }
}

DirectedAcyclicGraph::~DirectedAcyclicGraph() {
    for (auto row: rows) delete row;
}