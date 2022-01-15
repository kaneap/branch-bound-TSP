#include "edge.hpp"

namespace TSP {
    Edge::Edge(NodeId a, NodeId b) {
        if(a == b)
            throw std::invalid_argument("Edge::Edge(NodeId a, NodeId b): a == b");
        // a is always smaller than b
        if(a < b){
            _a = a;
            _b = b;
        } else {
            _a = b;
            _b = a;
        }
    }

    bool Edge::operator== (const Edge& other) const {
        return ((this->_a == other._a) && (this->_b == other._b));
    }

    bool Edge::operator!= (const Edge& other) const {
        return !(*this == other);
    }

    bool Edge::operator< (const Edge& other) const {
        if(this->_a < other._a){
            return true;
        } else if(this->_a == other._a){
            return this->_b < other._b;
        } else {
            return false;
        }
    }

    NodeId Edge::a() const {return this->_a;}

    NodeId Edge::b() const {return this->_b;}

    NodeId Edge::getOtherVertex(NodeId other){
        if(this->a() != other && this->b() != other)
            throw std::runtime_error("Neither end of the edge has id " + other);
        return this->a() == other? this->a() : this->b();
    }

    bool Edge::connectsVertex(NodeId v){
        return this->a() == v || this->b() == v;
    }    

}