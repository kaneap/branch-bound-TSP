#ifndef WEIGHTEDEDGE_HPP
#define WEIGHTEDEDGE_HPP

#include "edge.hpp"

namespace TSP {
    class WeightedEdge : public Edge{
    private:
        int _weight;
    public:
        WeightedEdge(NodeId a, NodeId b, int weight);
        int getWeight() const;
        void setWeight(int weight);
    };

    
}

#endif
