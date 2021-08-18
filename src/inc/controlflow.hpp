#ifndef CONTROLFLOW_HPP
#define CONTROLFLOW_HPP

#include <set>
#include <fstream>
#include "poliz.hpp"

struct flowTree {
    int ID;           // Для визуализации
    bool cond;        // Для идентификации при условных переходах
    POLIZ block;      // Блок кода
    std::set<flowTree*> prev; // Множество предыдущих блоков
    std::set<flowTree*> next; // Множество следующих блоков
    static std::set<int> checked;
    flowTree(int id, bool c = true): ID(id), cond(c) {};
    flowTree* getFT(int id, bool head = true);
};

class ControlFlowGraph {
    flowTree ft;
    std::ofstream graph;
    std::set<int> drawed;
public:
    ControlFlowGraph();

    void make(POLIZ * p);
    void makeBranch(POLIZ * p, flowTree * curBlock, flowTree * fb);
    void draw(void);
    void drawNode(flowTree p);
    void drawEdge(flowTree p);
    void newBlock(int blockId, POLIZ * p, flowTree * curBlock, bool cond = true);
    void merge();

    ~ControlFlowGraph();
};

#endif