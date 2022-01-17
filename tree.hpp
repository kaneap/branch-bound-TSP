#ifndef TREE_HPP
#define TREE_HPP

#include <algorithm>
#include <string>
#include <exception>
#include <sstream>
#include "graph.hpp"
#include "union.hpp"
#include "edge.hpp"
#include "rfList.hpp"

namespace TSP {
    class Tree {
    private:
        std::vector<WeightedEdge> _edges;
        std::vector<int> _vertexDegrees;
        size_t _numVertices;
        bool _illegal;
    public:
        Tree();
        Tree(const Graph & graph, const RFList & rf);
        bool is2Regular() const;
        std::vector<NodeId> getTour() const;
        std::string toTsplibString() const;
        int getDegree(NodeId v) const;
        int getTourCost(const Graph & graph) const;
        size_t getNumVertices() const;
        std::set<WeightedEdge> getConnectedEdges (NodeId v) const;
        bool isIllegal() const;
        void makeTrivialTour(const Graph & graph);
    };
}

#endif
