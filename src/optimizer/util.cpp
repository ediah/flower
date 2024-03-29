#include "optimizer/util.hpp"
#include "optimizer/controlflow.hpp"
#include "optimizer/acyclicgraph.hpp"

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

void copyPOLIZ(POLIZ & src, POLIZ & dst, int start, int end) {
    int j = dst.getSize();
    for (int i = start; i < end; i++) {
        dst.getEB()[j]   = src.getEB()[i];
        dst.getProg()[j] = src.getProg()[i];
        dst.incIter();
        j++;
    }
}

template int find(std::vector<int> vec, int x);
template int find(std::vector<flowTree*>, flowTree*);
template int find(std::vector<DAGRow*>, DAGRow*);
template int find(std::vector<std::pair<flowTree*, char>>, flowTree*);
template int find(std::vector<std::pair<flowTree*, char>>, char);
template int find(std::vector<std::pair<IdentTable*, DAGRow*>>, IdentTable*);