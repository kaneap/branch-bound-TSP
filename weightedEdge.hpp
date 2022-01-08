#ifndef WEIGHTEDEDGE_HPP
#define WEIGHTEDEDGE_HPP

#include "edge.hpp"

namespace TSP {
    class WeightedEdge : public Edge{
    private:
        int _weight;
    public:
        WeightedEdge(NodeId a, NodeId b, int weight);
        int getWeight();
        void setWeight(int weight);
        bool operator< (const WeightedEdge& rhs) const;
    };
}

#endif
