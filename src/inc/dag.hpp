#ifndef DAG_HPP
#define DAG_HPP

#include <vector>
#include <utility>
#include "exprtype.hpp"
#include "tables.hpp"
#include "poliz.hpp"

struct DAGRow {
    IdentTable * ident;
    op_t opcode;
    DAGRow * lvar;
    DAGRow * rvar;
    DAGRow * prev;
    bool assigned;

    static std::vector<DAGRow *> created;

    DAGRow(): ident(nullptr), opcode((op_t) NONE), lvar(nullptr), 
              rvar(nullptr), prev(nullptr), assigned(false) {
                  created.push_back(this);
              };
    void decompose(POLIZ & p, std::vector<DAGRow *> * asd);
    bool isLast(void) const;

    friend bool operator==(DAGRow & a, DAGRow & b);
    DAGRow & operator=(const DAGRow & dr);
    DAGRow(const DAGRow & dr);

};

/*
 * #N OP #1OP #2OP VAR
 */
class DirectedAcyclicGraph {
    std::vector<DAGRow *> rows;
    POLIZ stashed;
    bool verbose;
public:
    explicit DirectedAcyclicGraph(bool v): verbose(v) {};
    void make(POLIZ p);
    POLIZ decompose(void);

    void commonSubExpr(IdentTable * IT);

    void stash(POLIZ & p);

    std::pair<std::pair<DAGRow *, DAGRow *>, int> findCopies(
                DAGRow * left, DAGRow * right, int a, int b);

    ~DirectedAcyclicGraph();
};

#endif