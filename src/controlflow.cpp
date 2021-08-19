#include "controlflow.hpp"
#include "tables.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <iostream>

std::vector<int> flowTree::checked {0};

template<typename T>
int find(std::vector<T> vec, T x) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == x) return i;
    }
    return -1;
}

template<typename K, typename V>
int find(std::vector<std::pair<K, V>> vec, K x) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i].first == x) return i;
    }
    return -1;
}

template<typename K, typename V>
int find(std::vector<std::pair<K, V>> vec, V x) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i].second == x) return i;
    }
    return -1;
}

flowTree* flowTree::getFT(int id, bool head) {
    if (ID == id) return this;
    
    if ((ID < id) && (id < ID + block.getSize()))
        return split(id);

    flowTree * ret = nullptr;
    checked.emplace(checked.end(), ID);

    for (auto node: next) {
        if (find(checked, node.first->ID) == -1) {
            ret = node.first->getFT(id, false);
        }
        if (ret != nullptr) break;
    }

    if (ret == nullptr)
        for (auto node: prev) {
            if (find(checked, node.first->ID) == -1) {
                ret = node.first->getFT(id, false);
            }
            if (ret != nullptr) break;
        }
    
    if (head) checked.clear();
    return ret;
}

flowTree* flowTree::split(int id) {
    #ifdef DEBUG
    std::cout << "split " << ID << " | " << id << "\n";
    #endif

    flowTree * fb = new flowTree(id);

    for (int i = id - ID; i < block.getSize(); i++) {
        op_t op = block.getProg()[i];
        if (block.getEB()[i]) {
            type_t lval = (type_t) ((op >> 16) & 0xFF);
            type_t rval = (type_t) ((op >>  8) & 0xFF);
            fb->block.pushOp(lval, rval, (operation_t) (op & 0xFF));
        } else {
            fb->block.pushVal((IdentTable *) op);
        }
    }

    for (int i = block.getSize(); i > id - ID; i--)
        block.pop();

    int idx;
    for (auto p: next) {
        fb->next.push_back(p);
        while ((idx = find(p.first->prev, this)) != -1) {
            p.first->prev[idx].first = fb;
        }
    }

    next.clear();
    next.push_back(std::make_pair(fb, 0));

    fb->prev.push_back(std::make_pair(this, 0));

    return fb;
}

flowTree* ControlFlowGraph::newBlock(int blockId, POLIZ * p, flowTree * curBlock, char cond) {
    #ifdef DEBUG
    std::cout << "newBlock " << blockId << "\n";
    #endif
    bool exists = true;
    flowTree * fb = curBlock->getFT(blockId);
    if (fb == nullptr) {
        fb = new flowTree(blockId);
        exists = false;
    } else {
        //if (find(fb->prev, curBlock) != -1)
        //    return;
    }
    fb->prev.push_back(std::make_pair(curBlock, cond));
    curBlock->next.push_back(std::make_pair(fb, cond));
    makeBranch(p, curBlock, fb, exists);
    return fb->prev[0].first;
}

void ControlFlowGraph::makeBranch(POLIZ * p, flowTree * curBlock, flowTree * fb, bool exists) {
    if (fb != nullptr) {
        #ifdef DEBUG
        std::cout << "makeBranch " << curBlock->ID << " (";
        std::cout << curBlock << ") -> " << fb->ID << " (";
        std::cout << fb << ")\n";
        #endif
        if (exists) return;
        curBlock = fb;
    }

    int eip = curBlock->ID;
    void* blockId;
    while(eip < p->getSize()) {
        op_t op = p->getProg()[eip];
        if (p->getEB()[eip]) {
            if ((op & 0xFF) == JMP) {
                curBlock->block.pop(); // удалить LABEL
                blockId = ((IdentTable *) p->getProg()[eip-1])->getVal();
                newBlock(*(int*)blockId, p, curBlock);
                break;
            }

            if ((op & 0xFF) == JIT) {
                curBlock->block.pop(); // удалить LABEL
                blockId = ((IdentTable *) p->getProg()[eip-1])->getVal();
                curBlock = newBlock(*(int*)blockId, p, curBlock, 1); // Блок True
                newBlock(eip + 1, p, curBlock, 2);        // Блок False
                break;
            }

            //if (!exists) {
                type_t lval = (type_t) ((op >> 16) & 0xFF);
                type_t rval = (type_t) ((op >>  8) & 0xFF);
                curBlock->block.pushOp(lval, rval, (operation_t) (op & 0xFF));
            //}
        } else {
            //if (!exists)
                curBlock->block.pushVal((IdentTable *) op);
        }

        eip++;
    }
}

void ControlFlowGraph::make(POLIZ * p) {
    makeBranch(p, &ft, nullptr, false);
}

void ControlFlowGraph::draw(void) {
    graph.open("graph.dot");

    graph << "digraph CFG {\n";

    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(graph.rdbuf());
    
    drawNode(ft);
    drawed.clear();
    drawEdge(ft);
    drawed.clear();
    
    std::cout.rdbuf(coutbuf);

    graph << "}\n";
    graph.close();

    std::system("dot -v -Tpng -o./graph.png ./graph.dot 2>&1 | grep -i error");
}

void ControlFlowGraph::drawNode(flowTree p) {
    graph << "\tNODE" << p.ID << "  [shape=box, label=\"";
    p.block.repr(true);
    graph << "\"];\n";
    drawed.emplace(drawed.end(), p.ID);
    if (!p.next.empty()) {
        for (auto node: p.next) {
            if (find(drawed, node.first->ID) == -1)
                drawNode(* node.first);
        }
    }
}

void ControlFlowGraph::drawEdge(flowTree & p) {
    int size = p.next.size();

    if (find(drawed, p.ID) != -1)
        return;
    
    drawed.push_back(p.ID);

    for (auto node: p.next) {
        graph << "\tNODE" << p.ID << " -> NODE" << node.first->ID;

        if (node.second == 1)
            graph << " [label=\"True\"]";
        else if (node.second == 2)
            graph << " [label=\"False\"]";

        graph << ";\n";

        drawEdge(* node.first);
    }
}