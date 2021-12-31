#ifndef TREE_HPP
#define TREE_HPP

#include <set>
#include <algorithm>
#include "graph.hpp"
#include "union.hpp"

namespace TSP {
    class Tree {
    private:
        std::vector<std::pair<NodeId, NodeId>> _edges;
        std::vector<int> vertexDegrees;

    public:
        Tree(const Graph & graph, NodeId one);
        ~Tree();
        bool is2Regular();
    };
}

#endif
