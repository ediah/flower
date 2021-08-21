#ifndef CONTROLFLOW_HPP
#define CONTROLFLOW_HPP

#include <vector>
#include <fstream>
#include "tables.hpp"
#include "poliz.hpp"

/* Эта опция нужна для прорисовки ГПУ по шагам */
//#define CFG_STEPBYSTEP

struct flowTree {
    int ID;                           // Для визуализации
    POLIZ block;                      // Блок кода
    bool splitted;
    std::vector<std::pair<flowTree*, char>> prev;      // Множество предыдущих блоков
    /* 0 -- безусловный переход, 1 -- True, 2 -- False */
    //std::vector<char> cond;           // Для идентификации при условных переходах
    /* В связи с выбранной архитектурой, размер next не должен превышать 2 */
    std::vector<std::pair<flowTree*, char>> next;      // Множество следующих блоков
    static std::vector<int> checked;  // Для поиска
    flowTree(int id): ID(id), splitted(false) {};
    flowTree* getFT(int id, bool head = true);   // Поиск
    flowTree* split(int id);
};

class ControlFlowGraph {
    flowTree ft;
    std::ofstream graph;
    std::vector<int> drawed;
    int blocksNum;
    int jumpsNum;
public:
    ControlFlowGraph(): ft(0), blocksNum(0), jumpsNum(0) {};

    void make(POLIZ * p);
    void makeBranch(POLIZ * p, flowTree * curBlock, flowTree * fb, bool exists);
    void draw(std::string filename);
    void drawNode(flowTree p);
    void drawEdge(flowTree & p);
    void newBlock(int blockId, POLIZ * p, flowTree * curBlock, char cond = 0);
    void decompose(IdentTable* IT, POLIZ* poliz);
    void insertBlock(POLIZ* poliz, flowTree * curBlock, std::vector<int> * ls, std::vector<flowTree *> * eb);
    void newConn(POLIZ* poliz, flowTree * curBlock, std::vector<int> * ls, std::vector<flowTree *> * eb);
    void info(void);
    void clear(void);
    void deleteBranch(std::vector<std::pair<flowTree *, char>> vec, std::vector<flowTree*> * del);
    void fixStop(flowTree * ft);
    ~ControlFlowGraph();
};

#endif