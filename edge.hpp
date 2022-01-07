#ifndef EDGE_HPP
#define EDGE_HPP

#include <stdexcept>
#include "nodeId.hpp"

namespace TSP {
    class Edge {
    private:
        NodeId _a;
        NodeId _b;

    public:
        Edge(NodeId a, NodeId b);
        ~Edge();
        bool operator== (const Edge& other);
        const NodeId a();
        const NodeId b();
        NodeId getOtherVertex(NodeId other);
        bool connectsVertex(NodeId v);
    };
}

#endif
