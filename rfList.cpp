#include "rfList.hpp"

namespace TSP {
    RFList::RFList(size_t numVertices)
            : _r(std::vector<std::vector<bool>>(numVertices, std::vector<bool>(numVertices, false))),
              _f(std::vector<std::vector<bool>>(numVertices, std::vector<bool>(numVertices, false))),
              _rCnt(std::vector<unsigned int>(numVertices, 0)),
              _fCnt(std::vector<unsigned int>(numVertices, 0)) {}

    void RFList::require(Edge e) {
        if(_f[e.a()][e.b()]){
            throw std::runtime_error("Edge already forbidden.");
        }
        if(_r[e.a()][e.b()])
            return;
        if(_rCnt[e.a()] > 1 || _rCnt[e.b()] > 1){
            throw std::runtime_error("Cannot require more than two edges from a vertex.");
        }
        auto a = e.a();
        auto b = e.b();
        _r[a][b] = true;
        _r[b][a] = true;
        _rCnt[a]++;
        _rCnt[b]++;
        if(_rCnt[a] == 2){
            //forbid all unrequired edges from a
            for(unsigned int i = 0; i < _f.size(); i++){
                if(!_r[a][i] && !_f[a][i] && a != i){
                    forbid(Edge(a, i));
                }
            }
        }
        if(_rCnt[b] == 2){
            //forbid all unrequired edges from b
            for(unsigned int i = 0; i < _f.size(); i++){
                if(!_r[b][i] && !_f[b][i] && b != i){
                    forbid(Edge(b, i));
                }
            }
        }
    }

    void RFList::forbid(Edge e) {
        if(isRequired(e)){
            throw std::runtime_error("Edge already required.");
        }
        if(isForbidden(e))
            return;
        if(_fCnt[e.a()] > _f.size() - 2 || _fCnt[e.b()] > _f.size() - 2){
            throw std::runtime_error("Too many forbidden edges.");
        }
        auto a = e.a();
        auto b = e.b();
        _f[a][b] = true;
        _f[b][a] = true;
        _fCnt[a]++;
        _fCnt[b]++;
        if(_fCnt[a] == _f.size() - 3){
            //require the last two edges
            for(unsigned int i = 0; i < _f.size(); i++){
                if(!_f[a][i] && a != i){
                    require(Edge(a, i));
                }
            }
        }
        if(_fCnt[b] == _f.size() - 3){
            //require the last two edges
            for(unsigned int i = 0; i < _f.size(); i++){
                if(!_f[b][i] && b != i){
                    require(Edge(b, i));
                }
            }
        }
    }

    bool RFList::isRequired(Edge e) const {
        return _r[e.a()][e.b()];
    }

    bool RFList::isForbidden(Edge e) const {
        return _f[e.a()][e.b()];
    }

    bool RFList::is2Regular() const {
        for(unsigned int i = 0; i < _r.size(); i++){
            if(_rCnt[i] != 2){
                return false;
            }
        }
        return true;
    }

}