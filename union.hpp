#ifndef UNION_HPP
#define UNION_HPP
#include <vector>
#include <sstream>
#include "nodeId.hpp"

namespace TSP {
    struct Element {
        NodeId value;
        Element* parent;
        int size;
    };

    class Union{
    public:
        Union() = default;
        static Element* makeSet(NodeId v);
        static std::vector<Element*>* makeAllSets(size_t values);
        static Element* findSet(Element* x);
        static Element* makeUnion(Element* x, Element* y);
        static std::string toString(std::vector<Element*> u);
    };
}


#endif
