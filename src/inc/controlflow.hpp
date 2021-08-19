#ifndef CONTROLFLOW_HPP
#define CONTROLFLOW_HPP

#include <vector>
#include <fstream>
#include "poliz.hpp"

struct flowTree {
    int ID;                           // Для визуализации
    POLIZ block;                      // Блок кода
    std::vector<std::pair<flowTree*, char>> prev;      // Множество предыдущих блоков
    /* 0 -- безусловный переход, 1 -- True, 2 -- False */
    //std::vector<char> cond;           // Для идентификации при условных переходах
    /* В связи с выбранной архитектурой, размер next не должен превышать 2 */
    std::vector<std::pair<flowTree*, char>> next;      // Множество следующих блоков
    static std::vector<int> checked;  // Для поиска
    flowTree(int id): ID(id) {};
    flowTree* getFT(int id, bool head = true);   // Поиск
    flowTree* split(int id);
};

class ControlFlowGraph {
    flowTree ft;
    std::ofstream graph;
    std::vector<int> drawed;
public:
    ControlFlowGraph(): ft(0) {};

    void make(POLIZ * p);
    void makeBranch(POLIZ * p, flowTree * curBlock, flowTree * fb, bool exists);
    void draw(void);
    void drawNode(flowTree p);
    void drawEdge(flowTree & p);
    flowTree* newBlock(int blockId, POLIZ * p, flowTree * curBlock, char cond = 0);
};

#endif