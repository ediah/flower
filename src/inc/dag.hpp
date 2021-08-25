#ifndef DAG_HPP
#define DAG_HPP

#include <vector>
#include "exprtype.hpp"
#include "tables.hpp"
#include "poliz.hpp"

struct DAGRow {
    IdentTable * ident;
    op_t opcode;
    DAGRow * lvar;
    DAGRow * rvar;
    bool assigned;

    static std::vector<DAGRow *> deleted;

    DAGRow(): lvar(nullptr), rvar(nullptr), ident(nullptr), 
                    assigned(false), opcode((op_t) NONE) {};
    void decompose(POLIZ & p);

    friend bool operator==(DAGRow & a, DAGRow & b);

    ~DAGRow();
};

/*
 * #N OP #1OP #2OP VAR
 */
class DirectedAcyclicGraph {
    std::vector<DAGRow *> rows;
    POLIZ stashed;
public:
    DirectedAcyclicGraph() {};
    void make(POLIZ p);
    POLIZ decompose(void);

    void commonSubExpr(void);

    void replace(int from, int to);
    void remove(int idx);
    void stash(POLIZ & p);

    int findLast(IdentTable * var);

    ~DirectedAcyclicGraph();
};

#endif