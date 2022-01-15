#include "tree.hpp"

namespace TSP
{
    Tree::Tree()
    {
        _numVertices = 0;
    }

    Tree::Tree(const Graph & graph): 
            Tree(graph, RFList(graph.getNumVertices())){}

    Tree::Tree(const Graph & graph, std::vector<int> lambda){
        RFList rf(graph.getNumVertices());
        Tree(graph, lambda, rf);
    }

    Tree::Tree(const Graph & graph, std::vector<int> lambda, const RFList & rf){
        Graph modified(graph, lambda);
        Tree(modified, rf);
    }


    //construct a new 1 tree with costs c_λ ({i, j}) := c({i, j}) + λ(i) + λ(j)
    Tree::Tree(const Graph & graph, const RFList & rf):
    _edges(),
    _vertexDegrees(graph.getNumVertices())
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
            //if (i == one)
            //    continue; //we exclude the 1 in the 1-tree
            vertexSets.push_back(Union::makeSet(i));
        }
        auto allEdges = graph.getEdges(rf);

        unsigned count = 0;
        for (auto & e : allEdges)
        {
            if (count < graph.getNumVertices()-1){
                NodeId u = e.a();
                NodeId v = e.b();
                if(Union::findSet(vertexSets[u])->value != Union::findSet(vertexSets[v])->value){
                    if (u == one || v == one) continue;
                    if(e.getWeight() == std::numeric_limits<int>::max()){
                        throw std::runtime_error("Forbidden edge in tree");
                    }
                    //make new weighted edge so we evalutate wrt the original edge weights
                    edges.push_back(WeightedEdge(u, v, graph.getEdgeWeight(u, v)));
                    Union::makeUnion(vertexSets[u], vertexSets[v]);
                    count++;
                    
                }
            }
        }

        //second, we connect the two cheapest edges connected to the 1 vertex to the tree
        auto oneEdges = graph.getConnectedEdges(one);
        std::sort(oneEdges.begin(), oneEdges.end(),
                [] (const WeightedEdge& lhs, const WeightedEdge& rhs) {
            return lhs.getWeight() < rhs.getWeight();
        });
        WeightedEdge first = oneEdges[0];
        WeightedEdge second = oneEdges[1];
        for(unsigned i = 2; i < oneEdges.size(); i++){
            WeightedEdge current = oneEdges[i];
            if (current < first)
            {
                second = first;
                first = current;
            }else if (current < second)
            {
                second = current;
            } 
        }

        edges.push_back(WeightedEdge(first.a(), first.b(), graph.getEdgeWeight(first.a(), first.b())));
        edges.push_back(WeightedEdge(second.a(), second.b(), graph.getEdgeWeight(second.a(), second.b())));
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
        if(!is2Regular()) throw std::runtime_error("Attemted to get tour of a 1-tree which isn't 2-regular");
        std::vector<std::vector<NodeId>> connections(this->_numVertices); 
        std::vector<NodeId> tour;
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
        return tour;
    }

    std::string Tree::toTsplibString(){
        if(!is2Regular()) throw std::runtime_error("Attemted to get tour string of a 1-tree which isn't 2-regular");

        std::stringstream ss;
        int numEdges = this->_edges.size();
        ss << "TYPE : TOUR" << std::endl;
        ss << "DIMENSION : " << numEdges << std::endl;
        ss <<"TOUR SECTION" << std::endl;
        for (auto v : this->getTour()){
            ss << v+1 << std::endl;
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
        return connected;
    }

    size_t Tree::getNumVertices() const
    {
        return _numVertices;
    }
}