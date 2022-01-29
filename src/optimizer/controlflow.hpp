#ifndef CONTROLFLOW_HPP
#define CONTROLFLOW_HPP

#include <vector>
#include <fstream>
#include "common/tables.hpp"
#include "common/poliz.hpp"

/* Эта опция нужна для прорисовки ГПУ по шагам */
//#define CFG_STEPBYSTEP

struct flowTree {
    int ID;                           // Для визуализации
    POLIZ block;                      // Блок кода
    bool splitted;

    explicit flowTree(int id): ID(id), splitted(false) {};
    /* 0 -- безусловный переход, 1 -- True, 2 -- False */
    std::vector<std::pair<flowTree*, char>> prev;      // Множество предыдущих блоков
    std::vector<std::pair<flowTree*, char>> next;      // Множество следующих блоков
    static std::vector<int> checked;                   // Для поиска
    flowTree* getFT(int id, bool head = true);         // Поиск
    flowTree* split(int id);                           // Разбиение ветки по номеру блока
};

class ControlFlowGraph {
    flowTree ft;
    std::vector<flowTree *> tails;
    std::ofstream graph;
    std::vector<int> drawed;
    int blocksNum;
    int jumpsNum;
    int funcsNum;
public:
    ControlFlowGraph(): ft(0), blocksNum(0), jumpsNum(0), funcsNum(0) {};

    void clear(void);
    void make(POLIZ * p);
    void info(void) const;
    flowTree * head(void);
    flowTree * tailStop(void);
    void findTails(flowTree * ft);
    std::vector<flowTree *> tailRet(void);
    IdentTable * decompose(IdentTable* IT, POLIZ* poliz);
    void newBlock(int blockId, POLIZ * p, flowTree * curBlock, char cond = 0);
    void makeBranch(POLIZ * p, flowTree * curBlock, flowTree * fb, bool exists);
    void deleteBranch(std::vector<std::pair<flowTree *, char>> vec, std::vector<flowTree*> * del);
    void newConn(POLIZ* poliz, flowTree * curBlock, std::vector<int> * ls, std::vector<flowTree *> * eb);
    void insertBlock(POLIZ* poliz, flowTree * curBlock, std::vector<int> * ls, std::vector<flowTree *> * eb);

    #ifdef CFG_STEPBYSTEP
    void draw(const std::string &  filename);
    void drawNode(flowTree p);
    void drawEdge(flowTree & p);
    #endif
    ~ControlFlowGraph();
};

#endif