#include "graph.hpp"
#include <cmath>


namespace TSP {
    void Graph::initWeights(){
        _edges = std::vector<std::vector<int>>(_numVertices, std::vector<int>(_numVertices,-1));
    }

    Graph::Graph(size_t const numVertices):
    _numVertices(numVertices),
    _edges(numVertices)
    {
        _numVertices = numVertices;
        initWeights();
    }

    Graph::Graph (std::string const &filename){
        std::vector<int> xcoordinates;
        std::vector<int> ycoordinates;
        std::ifstream file(filename);
        bool coordSection = false;
        if (!file.is_open()){
            throw std::runtime_error("Could not open file " + filename);
        }
        while(file.is_open()){
            std::string line;
            while(getline(file, line)){
                std::stringstream ss(line);
                std::string token;
                getline(ss, token, ' ');
                if(!coordSection){
                    if(token.compare("DIMENSION:") == 0){
                        getline(ss, token, ' ');
                        _numVertices = std::stoi(token);
                    }else if(token.compare("DIMENSION") == 0){
                        getline(ss, token, ' ');
                        getline(ss, token, ' ');
                        _numVertices = std::stoi(token);
                    }
                    else if(token.compare("NODE_COORD_SECTION")==0){
                        coordSection = true;
                    } 
                }
                else{
                    if(token.compare("EOF")==0 || token.compare("-1")==0){
                        break;
                    }
                    try{
                        while(token.compare("")==0){
                            getline(ss, token, ' ');
                        }
                        int i = std::stoi(token);
                        getline(ss, token, ' ');
                        while(token.compare("")==0){
                            getline(ss, token, ' ');
                        }
                        xcoordinates.push_back(std::stoi(token));
                        getline(ss, token, ' ');
                        while(token.compare("")==0){
                            getline(ss, token, ' ');
                        }
                        ycoordinates.push_back(std::stoi(token));
                        std::cout << token << "\n";
                        //double distance = sqrt(x*x+y*y);
                    }catch(const std::invalid_argument& ia){
                        std::cout<< line;
                        throw std::invalid_argument("wrong format in input file");
                    }
                }
            }
            file.close();
        }

        initWeights();
        for(int i = 0; i< _numVertices;i++){
            for(int j = 0; j< i; j++){
                int difX = xcoordinates[i]-xcoordinates[j];
                int difY = ycoordinates[i]-ycoordinates[j];
                int distance = round(sqrt(difX*difX + difY*difY));
                setEdgeWeight(i,j,distance);
            }
        }
    }


    void Graph::setEdgeWeight(NodeId const node1_id, NodeId const node2_id, int const weight){
        if(node1_id == node2_id && weight!=0)
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
        std::cout << "added edge " << node1_id << " - " <<node2_id <<" with weight " << weight<< "\n";
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
   
    std::vector<WeightedEdge> Graph::getEdges () const {
        std::vector<WeightedEdge> edges(getNumEdges());
        for(NodeId i = 0; i < _numVertices; i++){
            for(NodeId j = 0; j < _numVertices; j++){
                if (i>=j) continue; //avoid double counting edges
                int weight = getEdgeWeight(i, j);
                WeightedEdge edge (i, j, weight);
                edges.push_back(edge);
            }
        }
        //todo: should we cache this?
        //for now can use std::sort, but to get the runtime better, we can use radix sort
        std::sort(edges.begin(), edges.end());
        return edges;
    };
}
