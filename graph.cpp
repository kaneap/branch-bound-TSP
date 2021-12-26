#include "graph.hpp"

namespace TSP {

    Graph::Graph(size_t const numVertices):
    _numVertices(numVertices),
    _edges(numVertices)
    {
        _numVertices = numVertices;
        for(int i = 0; i < numVertices; i++){
            // todo: do we have to do someting to initialize 2d vector array?
            for(int j = 0; j <= numVertices; j++){
                _edges[i][j] = -1;
            }
            
        }
    }

    void Graph::setEdgeWeight(NodeId const node1_id, NodeId const node2_id, int const weight){
        if(node1_id == node2_id)
            throw std::runtime_error("Cannot set edge weight for self-loops.");
        if(node1_id > _edges.size() || node2_id > _edges.size())
            throw std::runtime_error("Node id out of bounds.");
        if(node1_id < 0 || node2_id < 0)
            throw std::runtime_error("Node id must be non-negative.");
        if(weight < 0)
            throw std::runtime_error("Edge weight must be non-negative.");
        if(_edges[node1_id][node2_id] >= 0)
            throw std::runtime_error("Edge weight was already assigned.");    
        _edges[node1_id][node2_id] = weight;
        _edges[node2_id][node1_id] = weight;
   }

   int Graph::getEdgeWeight(NodeId const node1_id, NodeId const node2_id) const{
        if(node1_id == node2_id)
            throw std::runtime_error("Cannot get edge weight for self-loops.");
        if(node1_id > _edges.size() || node2_id > _edges.size())
            throw std::runtime_error("Node id out of bounds.");
        if(node1_id < 0 || node2_id < 0)
            throw std::runtime_error("Node id must be non-negative.");
        if(_edges[node1_id][node2_id] < 0)
            throw std::logic_error("Attemted to get an edge weight which hasn't been assigneg");
        return _edges[node1_id][node2_id];
   }
   size_t Graph::getNumVertices() const{
        return _numVertices;
   }
   size_t Graph::getNumEdges() const{
        return _numVertices * (_numVertices - 1) / 2;
   }
   
    std::vector<std::pair<std::pair<NodeId, NodeId>, int>> Graph::getEdges () const {
        std::vector<std::pair<std::pair<NodeId, NodeId>, int>> edges(getNumEdges());
        for(NodeId i = 0; i < _numVertices; i++){
            for(NodeId j = 0; j < _numVertices; j++){
                if (i>=j) continue; //avoid double counting edges
                auto edge = std::make_pair(i, j);
                int weight = getEdgeWeight(i, j);
                edges.push_back(std::make_pair(edge, weight));
            }
        }
        //todo: sort and save edges so we can use it again without recalculating it
    };
}
