#include "optimizer.hpp"
#include "exprtype.hpp"
#include "util.hpp"
#include <iostream>
#include <vector>

Optimizer::Optimizer(IdentTable * IT, POLIZ * p) {
    IdTable = IT;
    poliz = p;
}

void Optimizer::reduceConstants(void) {
    IdentTable * ait = IdTable, *bit = IdTable, *temp;

    while (ait->next != nullptr) {
        bit = ait;
        while (bit->next != nullptr) {
            if ((*ait == *bit->next) && (bit->next->isDef())) {
                for (int i = 0; i < poliz->getSize(); i++) {
                    if ((!poliz->getEB()[i]) && (poliz->getProg()[i] == (op_t)bit->next))
                        poliz->getProg()[i] = (op_t) ait;
                }
                #ifdef DEBUG
                std::cout << "УДАЛЁН ";
                bit->next->whoami();
                std::cout << "\n";
                #endif
                temp = bit->next;
                bit->next = bit->next->next;
                temp->next = nullptr;
                delete temp;
            } else bit = bit->next;
        }
        ait = ait->next;
    }
}

void Optimizer::optimize(bool verbose) {
    reduceConstants();
    
    CFG.make(poliz);
    #ifdef DRAW_GRAPH
    CFG.draw("compiled");
    #endif
    if (verbose) CFG.info();

    std::vector<flowTree *> optimized;
    std::vector<flowTree *> queue;
    queue.push_back(CFG.head());
    optimized.push_back(CFG.head());

    while (queue.size() != 0) {
        DirectedAcyclicGraph DAG(verbose);
        DAG.make(queue.front()->block);

        DAG.commonSubExpr(IdTable);

        POLIZ src = DAG.decompose();
        queue.front()->block.clear();
        copyPOLIZ(src, queue.front()->block, 0, src.getSize());

        for (auto node: queue.front()->next) {
            if (find(optimized, node.first) == -1) {
                queue.push_back(node.first);
                optimized.push_back(node.first);
            }
        }
        queue.erase(queue.begin());
    }

    CFG.decompose(IdTable, poliz);

    #ifdef DRAW_GRAPH
    CFG.clear();
    CFG.make(poliz);
    CFG.draw("optimized");
    #endif    
}