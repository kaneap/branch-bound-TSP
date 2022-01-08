#include "tree.hpp"

namespace TSP
{
    Tree::Tree()
    {
        _numVertices = 0;
    }

    Tree::Tree(const Graph & graph): Tree(graph, std::set<Edge>(), std::set<Edge>()){}

    //construct a new 1 tree with costs c_λ ({i, j}) := c({i, j}) + λ(i) + λ(j)
    Tree::Tree(const Graph & graph, std::set<Edge> required, std::set<Edge> forbidden)
    {
        //DONE: require and forbid edges
        //DONE in main: use modified weights via lambda

        //TODO: does it matter what we set the 1-vertex to? Should it be random?
        NodeId one = 0;
        //first, use Kruskal's algorithm to create a minimum spanning tree
        std::vector<WeightedEdge> edges;
        //might make sense for vertexSets to be a member of a disjoint set (union) class
        std::vector<Element *> vertexSets;
        this->_numVertices = graph.getNumVertices();
        for (NodeId i = 0; i < graph.getNumVertices(); i++)
        {
            if (i == one)
                continue; //we exclude the 1 in the 1-tree
            vertexSets[i] = Union::makeSet(i);
        }
        auto allEdges = graph.getEdges(required, forbidden);

        for (auto e : allEdges)
        {
            edges.push_back(e);
            NodeId u = e.a();
            NodeId v = e.b();
            if (u == one || v == one)
                continue;
        }

        //second, we connect the two cheapest edges connected to the 1 vertex to the tree
        auto oneEdges = graph.getConnectedEdges(one);
        //todo: sort this
        edges.push_back(oneEdges[0]);
        edges.push_back(oneEdges[1]);
        _edges = edges;
        for (auto e : _edges)
        {
            _vertexDegrees[e.a()]++;
            _vertexDegrees[e.b()]++;
        }
    }

    bool Tree::is2Regular() const
    {
        for (int degree : _vertexDegrees)
        {
            if (degree != 2)
                return false;
        }
        return true;
    }

    std::vector<NodeId> Tree::getTour(){
        if(!is2Regular) throw std::runtime_error("Attemted to get tour of a 1-tree which isn't 2-regular");
        std::vector<std::vector<NodeId>> connections; 
        std::vector<NodeId> tour(this->_numVertices);
        for(auto e : _edges){
            connections[e.a()].push_back(e.b());
            connections[e.b()].push_back(e.a());
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
        for (auto v : this->getTour()){
            ss << v << std::endl;
        }
        ss << "-1" << std::endl << "EOF";
      return ss.str();
    }

    int Tree::getDegree(NodeId v) const
    {
        return _vertexDegrees[v];
    }

    int Tree::getTourCost() const
    {
        //if (!is2Regular()) return -1; //TODO: return exeption?? Or just invalid value?
        int sum = 0;
        for(WeightedEdge e : _edges){
            sum += e.getWeight();
        }
        return sum;
    }

    std::set<WeightedEdge> Tree::getConnectedEdges(NodeId v) {
        std::set<WeightedEdge> connected;
        for(WeightedEdge e : _edges){
            if(e.connectsVertex(v)) connected.insert(e);
        }
        //TODO: should I return reference?
        return connected;
    }

    int Tree::getNumVertices() const
    {
        return _numVertices;
    }
}