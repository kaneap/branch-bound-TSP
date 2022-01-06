#ifndef TREE_HPP
#define TREE_HPP

#include <set>
#include <algorithm>
#include <string>
#include <exception>
#include <sstream>
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
        Tree();
        Tree(const Graph & graph);
        Tree(const Graph & graph, std::set<Edge> required, std::set<Edge> forbidden);
         Tree(const Graph & graph, std::vector<int> lambda);
        Tree(const Graph & graph, std::vector<int> lambda, std::set<Edge> required, std::set<Edge> forbidden);
        ~Tree();
        bool is2Regular() const;
        std::vector<NodeId> getTour();
        std::string toTsplibString();
        int getDegree(NodeId v) const;
        int getTourCost() const;
        int getNumVertices() const;
        std::set<WeightedEdge> getConnectedEdges (NodeId v);
    };
}

#endif
