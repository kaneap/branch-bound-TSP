#include "weightedEdge.hpp"

namespace TSP {
    bool WeightedEdge::operator< (const WeightedEdge& rhs){return this->_weight < rhs._weight;}
}