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
        bool operator== (const Edge& other) const;
        bool operator!= (const Edge& other) const;
        bool operator< (const Edge& other) const;
        NodeId a() const;
        NodeId b() const;
        NodeId getOtherVertex(NodeId other);
        bool connectsVertex(NodeId v);
    };

    struct EdgeHasher {
        size_t operator()(const Edge& e) const {
            return (e.a() * 31 + e.b()) % SIZE_MAX;
        }
    };
}

#endif
