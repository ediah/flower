#include "controlflow.hpp"
#include "tables.hpp"
#include <algorithm>
#include <iostream>

std::set<int> flowTree::checked {0};

flowTree* flowTree::getFT(int id, bool head) {
    if (ID == id) return this;
    flowTree * ret = nullptr;
    checked.emplace(ID);

    for (auto node: next) {
        if (checked.find(node->ID) == checked.end()) {
            ret = node->getFT(id, false);
        }
        if (ret != nullptr) break;
    }

    if (ret == nullptr)
        for (auto node: prev) {
            if (checked.find(node->ID) == checked.end()) {
                ret = node->getFT(id, false);
            }
            if (ret != nullptr) break;
        }
    
    if (head) checked.clear();
    return ret;
}

ControlFlowGraph::ControlFlowGraph(): ft(0) {
}

ControlFlowGraph::~ControlFlowGraph() {
    
}

void ControlFlowGraph::newBlock(int blockId, POLIZ * p, flowTree * curBlock, bool cond) {
    #ifdef DEBUG
    std::cout << "newBlock " << blockId << "\n";
    #endif
    flowTree * fb = curBlock->getFT(blockId);
    if (fb == nullptr)
        fb = new flowTree(blockId, cond);
    curBlock->next.emplace(fb);
    makeBranch(p, curBlock, fb);
}

void ControlFlowGraph::makeBranch(POLIZ * p, flowTree * curBlock, flowTree * fb) {
    if (fb != nullptr) {
        #ifdef DEBUG
        std::cout << "makeBranch " << curBlock->ID << " (";
        std::cout << curBlock << ") -> " << fb->ID << " (";
        std::cout << fb << ")\n";
        #endif
        if (fb->prev.find(curBlock) != fb->prev.end())
            return;
        fb->prev.emplace(curBlock);
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
                newBlock(*(int*)blockId, p, curBlock); // Блок True
                newBlock(eip + 1, p, curBlock, false); // Блок False
                break;
            }
            type_t lval = (type_t) ((op >> 16) & 0xFF);
            type_t rval = (type_t) ((op >>  8) & 0xFF);
            curBlock->block.pushOp(lval, rval, (operation_t) (op & 0xFF));
        } else {
            curBlock->block.pushVal((IdentTable *) op);
        }

        eip++;
    }
}

void ControlFlowGraph::make(POLIZ * p) {
    makeBranch(p, &ft, nullptr);
}

void ControlFlowGraph::merge() {
    
}

void ControlFlowGraph::draw(void) {
    graph.open("graph.dot");

    graph << "digraph CFG {\n";

    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(graph.rdbuf());
    
    drawNode(ft);
    drawed.clear();
    drawEdge(ft);
    
    std::cout.rdbuf(coutbuf);

    graph << "}\n";
    graph.close();

    std::system("dot -v -Tpng -o./graph.png ./graph.dot 2>&1 | grep -i error");
}

void ControlFlowGraph::drawNode(flowTree p) {
    graph << "\tNODE" << p.ID << "  [shape=box, label=\"";
    p.block.repr(true);
    graph << "\"];\n";
    drawed.emplace(p.ID);
    if (!p.next.empty()) {
        for (auto node: p.next) {
            if (drawed.find(node->ID) == drawed.end())
                drawNode(*node);
        }
    }
}

void ControlFlowGraph::drawEdge(flowTree p) {
    int size = p.next.size();
    drawed.emplace(p.ID);
    if (size != 0)
        for (auto node: p.next) {
            graph << "\tNODE" << p.ID << " -> NODE" << node->ID;

            if (size > 1) {
                graph << " [label=\"" << (node->cond? "True" : "False");
                graph << "\"]";
            }

            graph << ";\n";
            
            if (drawed.find(node->ID) == drawed.end()) {
                drawEdge(*node);
            }
        }
}