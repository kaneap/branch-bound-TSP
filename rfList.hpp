#ifndef RFLIST_HPP
#define RFLIST_HPP

#include <stdexcept>
#include <vector>
#include "nodeId.hpp"
#include "edge.hpp"

namespace TSP {
    class RFList {
    private:
        std::vector<std::vector<bool>> _r;
        std::vector<std::vector<bool>> _f; 
        std::vector<unsigned int> _rCnt;
        std::vector<unsigned int> _fCnt;

    public:
        RFList(size_t numVertices);
        void require(Edge e);
        void forbid(Edge e);
        bool isRequired(Edge e) const;
        bool isForbidden(Edge e) const;
    };
}

#endif
