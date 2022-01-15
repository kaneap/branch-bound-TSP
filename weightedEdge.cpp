#include "weightedEdge.hpp"

namespace TSP {
    WeightedEdge::WeightedEdge(NodeId a, NodeId b, int weight):
    Edge(a,b),
    _weight(weight)
    {}

    int WeightedEdge::getWeight() const {return _weight;}
    void WeightedEdge::setWeight(int weight){_weight = weight;}
}