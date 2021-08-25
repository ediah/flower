#include "optimizer.hpp"
#include "exprtype.hpp"
#include "util.hpp"
#include <iostream>
#include <vector>

Optimizer::Optimizer(IdentTable * IT, POLIZ * p) {
    IdTable = IT;
    poliz = p;
}

void Optimizer::optimize(bool verbose) {
    CFG.make(poliz);
    #ifdef DEBUG
    CFG.draw("compiled");
    #endif
    if (verbose) CFG.info();

    std::vector<flowTree *> optimized;
    std::vector<flowTree *> queue;
    queue.push_back(CFG.head());
    optimized.push_back(CFG.head());

    while (queue.size() != 0) {
        DirectedAcyclicGraph DAG;
        DAG.make(queue.front()->block);

        DAG.commonSubExpr();

        POLIZ dst = queue.front()->block;
        POLIZ src = DAG.decompose();
        dst.clear();
        copyPOLIZ(src, dst, 0, src.getSize());

        for (auto node: queue.front()->next) {
            if (find(optimized, node.first) == -1) {
                queue.push_back(node.first);
                optimized.push_back(node.first);
            }
        }
        queue.erase(queue.begin());
    }

    CFG.decompose(IdTable, poliz);

    #ifdef DEBUG
    CFG.clear();
    CFG.make(poliz);
    CFG.draw("optimized");

    if (verbose) CFG.info();

    DAG.make(CFG.head()->block);
    POLIZ dcp = DAG.decompose();
    CFG.head()->block.repr();
    std::cout << "\n";
    dcp.repr();

    //IdTable->repr();
    //poliz->repr();
    #endif    
}