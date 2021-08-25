#include "controlflow.hpp"
#include "tables.hpp"
#include "obstacle.hpp"
#include "util.hpp"
#include <iterator>
#include <iostream>

std::vector<int> flowTree::checked {0};

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
    fb->splitted = true;

    copyPOLIZ(block, fb->block, id - ID, block.getSize());

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

void ControlFlowGraph::newBlock(int blockId, POLIZ * p, flowTree * curBlock, char cond) {
    #ifdef DEBUG
    std::cout << "newBlock " << blockId << "\n";
    #endif
    bool exists = true;
    flowTree * fb = curBlock->getFT(blockId);
    if (fb == nullptr) {
        fb = new flowTree(blockId);
        exists = false;
        blocksNum++;
    }
    if (fb->splitted) {
        fb->splitted = false;
        jumpsNum++;
        blocksNum++;
    }
    fb->prev.push_back(std::make_pair(curBlock, cond));
    curBlock->next.push_back(std::make_pair(fb, cond));
    jumpsNum++;
    makeBranch(p, curBlock, fb, exists);
}

#ifdef CFG_STEPBYSTEP
int drawIterator = 0;
#endif

void ControlFlowGraph::makeBranch(POLIZ * p, flowTree * curBlock, flowTree * fb, bool exists) {
    #ifdef CFG_STEPBYSTEP
    draw(std::to_string(drawIterator++));
    #endif

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
        flowTree * existingPart = curBlock->getFT(eip);

        if ((eip != curBlock->ID) && (existingPart != nullptr)) {
            newBlock(eip, p, curBlock);
            break;
        }
 
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
                //curBlock = newBlock(*(int*)blockId, p, curBlock, 1); // Блок True
                int bsize = curBlock->block.getSize();
                newBlock(*(int*)blockId, p, curBlock, 1); // Блок True
                if (curBlock->block.getSize() != bsize)
                    curBlock = curBlock->next[0].first;
                newBlock(eip + 1, p, curBlock, 2);        // Блок False
                break;
            }

            if ((op & 0xFF) == CALL) {
                curBlock->block.pop(); // удалить LABEL
                curBlock->block.push(op, true);
                blockId = ((IdentTable *) p->getProg()[eip-1])->getVal();
                newBlock(*(int*)blockId, p, curBlock, 1);
                newBlock(eip + 1, p, curBlock, 2);
                break;
            }

            if ((op & 0xFF) == RET) {
                curBlock->block.push(op, true);
                break;
            }
        }
        curBlock->block.push(op, p->getEB()[eip]);
        eip++;
    }
}

void ControlFlowGraph::make(POLIZ * p) {
    makeBranch(p, &ft, nullptr, false);
    blocksNum++;
    ft.checked.clear();
    findTails(&ft);
}

void ControlFlowGraph::findTails(flowTree * ft) {
    int s = ft->block.getSize();
    if (s > 0) {
        operation_t op = (operation_t) (ft->block.getProg()[s-1] & 0xFF);
        if ((op == STOP) || (op == RET)) {
            for (auto node: ft->next) {
                auto vec = node.first->prev;
                int idx = find(vec, ft);
                if (idx == -1) throw Obstacle(PANIC);
                vec.erase(vec.begin() + idx);
            }
            ft->next.clear();
        }
        if (op == STOP) {
            tails.insert(tails.begin(), ft);
            return;
        }
        if (op == RET) {
            funcsNum++;
            tails.insert(tails.end(), ft);
            return;
        }
    }
    for (auto node: ft->next) {
        if (find(ft->checked, node.first->ID) == -1) {
            ft->checked.push_back(node.first->ID);
            findTails(node.first);
        }
    }
}

void ControlFlowGraph::info(void) {
    std::cout << "Граф потока управления построен. Статистика:\n";
    std::cout << "\tВсего блоков: " << blocksNum << "\n";
    std::cout << "\tВсего переходов: " << jumpsNum << "\n";
    std::cout << "\tВсего функций: " << funcsNum << "\n";
}

void ControlFlowGraph::draw(std::string filename) {
    graph.open(filename + ".dot");

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

    std::system(("dot -v -Tpng -o./" + filename + ".png ./" +
                filename + ".dot 2>&1 | grep -i error").data() );
}

void ControlFlowGraph::drawNode(flowTree p) {
    graph << "\tNODE" << p.ID << "  [shape=record, label=\"{";
    graph << "BlockID: " << p.ID << " | ";
    p.block.repr(true);
    graph << "}\"];\n";
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

void ControlFlowGraph::newConn(POLIZ* poliz, flowTree * curBlock, 
                                   std::vector<int> * ls, std::vector<flowTree *> * eb) {
    if (find(*eb, curBlock) == -1) {
        // Вписываем сюда
        insertBlock(poliz, curBlock, ls, eb);
    } else {
        // Прыжок назад
        ls->push_back(poliz->getSize());
        poliz->push((op_t) curBlock, false);
        poliz->pushOp(_NONE_, _NONE_, JMP);
    }
}

void ControlFlowGraph::insertBlock(POLIZ* poliz, flowTree * curBlock, 
                                   std::vector<int> * ls, std::vector<flowTree *> * eb) {
    int bsize = curBlock->block.getSize();
    curBlock->ID = poliz->getSize();
    eb->push_back(curBlock);
    copyPOLIZ(curBlock->block, *poliz, 0, bsize);

    if (curBlock->next.size() == 1) {
        newConn(poliz, curBlock->next[0].first, ls, eb);
    } else if (curBlock->next.size() == 2) {
        flowTree * falseb = curBlock->next[find(curBlock->next, (char)2)].first;
        flowTree * trueb  = curBlock->next[find(curBlock->next, (char)1)].first;

        op_t lastop = curBlock->block.getProg()[bsize - 1];
        if ((lastop & 0xFF) == CALL) poliz->pop();
        ls->push_back(poliz->getSize());
        poliz->push((op_t) trueb, false);
        if ((lastop & 0xFF) == CALL)
            poliz->push(lastop, true);
        else
            poliz->pushOp(_BOOLEAN_, _INT_, JIT);
        newConn(poliz, falseb, ls, eb);
        newConn(poliz, trueb, ls, eb);
    } else if (curBlock->next.size() > 2) throw Obstacle(PANIC);
}

void ControlFlowGraph::decompose(IdentTable* IT, POLIZ* poliz) {
    //*IT = * IT->deleteLabels(); // эта функция всё портит
    poliz->clear();
    std::vector<int> labelStorage;
    std::vector<flowTree*> existingBlocks;
    insertBlock(poliz, &ft, &labelStorage, &existingBlocks);
    for (auto lpos: labelStorage) {
        flowTree * block = (flowTree *) poliz->getProg()[lpos];
        IT->pushVal(new int (block->ID));
        IT->pushType(_LABEL_);
        poliz->getProg()[lpos] = (op_t) IT->confirm();
    }
}

void ControlFlowGraph::deleteBranch(std::vector<std::pair<flowTree *, char>> vec, std::vector<flowTree*> * del) {
    for (auto node: vec) {
        if (find(*del, node.first) == -1) {
            del->push_back(node.first);
            deleteBranch(node.first->next, del);
            delete node.first;
        }
    }
}

void ControlFlowGraph::clear(void) {
    std::vector<flowTree*> deleted;

    deleteBranch(ft.next, &deleted);
    ft.next.clear();
    ft.prev.clear();
    drawed.clear();
    blocksNum = 0;
    jumpsNum = 0;
}

flowTree * ControlFlowGraph::head(void) {
    return &ft;
}

flowTree * ControlFlowGraph::tailStop(void) {
    if (funcsNum + 1 != tails.size())
        throw Obstacle(PANIC);
    
    return tails[0];
}

std::vector<flowTree *> ControlFlowGraph::tailRet(void) {
    std::vector<flowTree *> ret;
    for (auto it = tails.begin() + 1; it != tails.end(); ++it)
        ret.push_back(*it);

    return ret;
}

ControlFlowGraph::~ControlFlowGraph() {
    clear();
}