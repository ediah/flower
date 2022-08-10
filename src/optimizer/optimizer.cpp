#include "optimizer/optimizer.hpp"
#include "common/exprtype.hpp"
#include "optimizer/util.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

Optimizer::Optimizer(IdentTable * IT, POLIZ * p, bool verb) {
    IdTable = IT;
    poliz = p;
    verbose = verb;
}

void Optimizer::reduceConstants(void) {
    IdentTable * ait = IdTable, *bit = IdTable, *temp;

    while (ait->next != nullptr) {
        bit = ait;
        while (bit->next != nullptr) {
            if ((*ait == *bit->next) && (bit->next->isDef())) {
                for (int i = 0; i < poliz->getSize(); i++) {
                    if ((!poliz->getEB()[i]) && (poliz->getVal(i) == bit->next))
                        poliz->setVal(i, ait);
                }
                if (verbose) {
                    std::cout << "УДАЛЁН ";
                    bit->next->whoami();
                    std::cout << "\n";
                }
                
                temp = bit->next;
                bit->next = bit->next->next;
                temp->next = nullptr;
                delete temp;
            } else bit = bit->next;
        }
        ait = ait->next;
    }
}

IdentTable * Optimizer::optimize() {
    reduceConstants();
    
    CFG.make(poliz);
    #ifdef DRAW_GRAPH
    CFG.draw("compiled");
    #endif
    if (verbose) CFG.info();

    std::vector<flowTree *> optimized = {CFG.head()};
    std::vector<flowTree *> queue = {CFG.head()};
    std::vector<bool> afterCall = {false}; // Нужно собирать возврат

    std::map<int, std::vector<type_t>> funcRets;
    std::vector<type_t> typeOnStack;
    std::vector<std::vector<type_t>> oldStack = {{}};
    std::vector<int> conn; // ID вызванной функции
    std::vector<int> funcHead = {};

    while (queue.size() != 0) {
        DirectedAcyclicGraph DAG(verbose);
        DAG.make(queue.back()->block);

        DAG.commonSubExpr(IdTable);

        typeOnStack = oldStack.front();
        if (afterCall.front()) {
            std::vector<type_t> tail = funcRets[conn.front()];
            typeOnStack.insert(typeOnStack.end(), tail.begin(), tail.begin());
            conn.erase(conn.begin());
        }
        POLIZ src = DAG.decompose(&typeOnStack);
        src.repr();
        queue.back()->block.clear();
        copyPOLIZ(src, queue.back()->block, 0, src.getSize());

        if (queue.back()->block.endsWithRet()) {
            funcRets[funcHead.back()] = typeOnStack;
            funcHead.pop_back();
        }

        if (queue.back()->block.endsWithCall()) {
            auto nextB = queue.back()->next;
            auto isTrueB = [](std::pair<flowTree *, char> pair){return pair.second == 1;};
            auto nodeTrueB = std::find_if(nextB.begin(), nextB.end(), isTrueB);
            int connID = nodeTrueB->first->ID;

            for (auto node: queue.back()->next) {
                if ((node.second == 2) && (find(optimized, node.first) == -1)) {
                    conn.push_back(connID);
                    queue.push_back(node.first);
                    optimized.push_back(node.first);
                    afterCall.push_back(true);
                    oldStack.push_back(typeOnStack);
                }
            }
            for (auto node: queue.back()->next) {
                if ((node.second == 1) && (find(optimized, node.first) == -1)) {
                        queue.push_back(node.first);
                        optimized.push_back(node.first);
                        afterCall.push_back(false);
                        oldStack.push_back({});
                        funcHead.push_back(connID);
                }
            }
        } else {
            for (auto node: queue.back()->next) {
                if (find(optimized, node.first) == -1) {
                    queue.push_back(node.first);
                    optimized.push_back(node.first);
                    afterCall.push_back(false);
                    oldStack.push_back(typeOnStack);
                }
            }
        }
        
        queue.pop_back();
        afterCall.erase(afterCall.begin());
        oldStack.erase(oldStack.begin());
    }

    IdTable = CFG.decompose(IdTable, poliz);

    #ifdef DRAW_GRAPH
    CFG.clear();
    CFG.make(poliz);
    CFG.draw("optimized");
    #endif   

    return IdTable; 
}