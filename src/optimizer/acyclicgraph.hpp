#ifndef DAG_HPP
#define DAG_HPP

#include <vector>
#include <utility>
#include "common/exprtype.hpp"
#include "common/tables.hpp"
#include "common/poliz.hpp"

struct DAGRow {
    type_t type;
    IdentTable * ident;
    op_t opcode;
    DAGRow * lvar;
    DAGRow * rvar;
    DAGRow * prev;
    bool assigned;

    static std::vector<DAGRow *> created;

    DAGRow(): type(_NONE_), ident(nullptr), opcode((op_t) NONE), lvar(nullptr), 
              rvar(nullptr), prev(nullptr), assigned(false) {
                  created.push_back(this);
              };
    void decompose(POLIZ & p, std::vector<DAGRow *> * asd);
    bool isLast(void) const;
    type_t updateType(std::vector<type_t> * typeOnStack = nullptr);

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
    POLIZ decompose(std::vector<type_t> * typeOnStack);
    void updateTypes(void);

    void commonSubExpr(IdentTable * IT);

    void stash(POLIZ & p);

    std::pair<std::pair<DAGRow *, DAGRow *>, int> findCopies(
                DAGRow * left, DAGRow * right, int a, int b);

    ~DirectedAcyclicGraph();
};

#endif