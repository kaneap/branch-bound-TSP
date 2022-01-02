#ifndef TREE_HPP
#define TREE_HPP

#include <set>
#include <algorithm>
#include "graph.hpp"
#include "union.hpp"
#include "edge.hpp"

namespace TSP {
    class Tree {
    private:
        std::vector<WeightedEdge> _edges;
        std::vector<int> _vertexDegrees;
        int _numVertices;

    public:
        Tree(const Graph & graph, NodeId one);
        ~Tree();
        bool is2Regular();
        int getDegree(NodeId v);
        int getTourLength();
        std::set<WeightedEdge> getConnectedEdges (NodeId v);
    };
}

#endif
