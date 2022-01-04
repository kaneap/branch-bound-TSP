#include "tree.hpp"

namespace TSP {
    Tree::Tree(const Graph & graph, NodeId one) {
        //first, use Kruskal's algorithm to create a minimum spanning tree
        std::vector<std::pair<NodeId, NodeId>> edges;
        //might make sense for vertexSets to be a member of a disjoint set (union) class
        std::vector<Element*> vertexSets;
        for(NodeId i = 0; i < graph.getNumVertices(); i++){
            if (i == one) continue; //we exclude the 1 in the 1-tree
            vertexSets[i] = Union::makeSet(i);
        }
        auto allEdges = graph.getEdges();
        
        for(auto e : allEdges){
            NodeId u = e.first.first;
            NodeId v = e.first.second;
            if(u == one || v == one) continue;
            Element* uSet = Union::findSet(vertexSets[u]);
            Element* vSet = Union::findSet(vertexSets[v]);
            if(uSet != vSet){
                edges.push_back(std::make_pair(u,v));
                Union::makeUnion(uSet, vSet);
            }
        }
        
        //second, we connect the two cheapest edges connected to the 1 vertex to the tree
        auto oneEdges = graph.getConnectedEdgeWeights(one);
        //todo: sort this
        edges.push_back(std::make_pair(one, oneEdges[0].first));
        edges.push_back(std::make_pair(one, oneEdges[1].first));
        _edges = edges;
    }

    bool Tree::is2Regular(){
        std::vector<int> vertexDegrees;
        for(auto e : _edges){
            vertexDegrees[e.first]++;
            vertexDegrees[e.second]++;
        }
        for(int degree : vertexDegrees){
            if(degree != 0) return false;
        }
        return true;
    }

    std::vector<NodeId> Tree::getTour(){
        if(!is2Regular) throw std::runtime_error("Attemted to get tour of a 1-tree which isn't 2-regular");
        std::vector<std::vector<NodeId>> connections; 
        std::vector<NodeId> tour(this->_numVertices);
        for(auto e : _edges){
            connections[e.a()].push_back(b);
            connections[e.b()].push_back(a);
        }
        NodeId curr = 0;
        NodeId prev = invalid_node_id;
        do{
            //add the current to the tour
            tour.push_back(curr);
            NodeId a = connections[curr][0];
            NodeId b = connections[curr][1];
            NodeId next = (a == prev)? b : a;
            prev = curr;
            curr = next;
        }while(curr != 0);
        

    }

    std::string Tree::toTsplibString(){
        if(!is2Regular) throw std::runtime_error("Attemted to get tour string of a 1-tree which isn't 2-regular");

        std::stringstream ss;
        int numEdges = this->_edges.size();
        ss << "TYPE : TOUR" << std::endl;
        ss << "DIMENSION : " << numEdges << std::endl;
        ss <<"TOUR SECTION" << std::endl;
            ss << "e " << (pair.first+1) << " " << (pair.second + 1) << std::endl;
        }
        ss << "-1" << std::endl << "EOF";
      return ss.str();
    }
}