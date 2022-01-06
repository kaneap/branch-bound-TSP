#include "edge.hpp"

namespace TSP {
    Edge::Edge(NodeId a, NodeId b) {
        // a is always smaller than b
        if(a < b){
            _a = a;
            _b = b;
        } else {
            _a = b;
            _b = a;
        }
    }

    bool Edge::operator== (const Edge& other){
        return ((this->_a == other._a) && (this->_b == other._b));
    }

    NodeId Edge::a(){return this->_a;}

    NodeId Edge::b(){return this->_b;}

    NodeId Edge::getOtherVertex(NodeId other){
        if(this->a() != other && this->b() != other)
            throw std::runtime_error("Neither end of the edge has id " + other);
        return this->a() == other? this->a() : this->b();
    }

    bool Edge::connectsVertex(NodeId v){
        return this->a() == v || this->b() == v;
    }    

}