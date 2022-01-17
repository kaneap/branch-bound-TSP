#include "tree.hpp"

namespace TSP
{
    Tree::Tree(): _numVertices(0), _illegal(true){}

    Tree::Tree(const Graph & graph): 
            Tree(graph, RFList(graph.getNumVertices())){}

    Tree::Tree(const Graph & graph, std::vector<int> lambda): 
            Tree(graph, lambda, RFList(graph.getNumVertices())){}

    Tree::Tree(const Graph & graph, std::vector<int> lambda, const RFList & rf):
            Tree(Graph(graph, lambda), rf){}

    //construct a new 1 tree with costs c_λ ({i, j}) := c({i, j}) + λ(i) + λ(j)
    Tree::Tree(const Graph & graph, const RFList & rf):
    _edges(),
    _vertexDegrees(graph.getNumVertices()),
    _illegal(false)
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
        std::vector<WeightedEdge> oneEdges;

        unsigned count = 0;
        for (auto & e : allEdges)
        {
            if (count >= graph.getNumVertices()-1) break;
            NodeId u = e.a();
            NodeId v = e.b();
            if(Union::findSet(vertexSets[u])->value != Union::findSet(vertexSets[v])->value){
                if (u == one || v == one){
                    oneEdges.push_back(e);
                    continue;
                }
                if(e.getWeight() == std::numeric_limits<int>::max()){
                    _illegal = true;
                }
                //make new weighted edge so we evalutate wrt the original edge weights
                edges.push_back(WeightedEdge(u, v, graph.getEdgeWeight(u, v)));
                Union::makeUnion(vertexSets[u], vertexSets[v]);
                count++;
                
            }
            
        }

        //second, we connect the two cheapest edges connected to the 1 vertex to the tree
        WeightedEdge first = oneEdges[0];
        WeightedEdge second = oneEdges[1];
        for(unsigned int i = 2; i < oneEdges.size(); i++){
            WeightedEdge current = oneEdges[i];
            if (current.getWeight() < first.getWeight())
            {
                second = first;
                first = current;
            }else if (current.getWeight() < second.getWeight())
            {
                second = current;
            } 
        }

        if(first.getWeight() == std::numeric_limits<int>::max() || second.getWeight() == std::numeric_limits<int>::max()){
            _illegal = true;
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

    std::vector<NodeId> Tree::getTour() const {
        if(isIllegal()) throw std::runtime_error("Attempted to get the tour of an llegal tree");
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

    std::string Tree::toTsplibString() const {
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


    /**
     * @brief 
     * 
     * @param graph the graph with the cost that we evaluate the costs with
     * @return int cost of the 1 tree w.r.t. the given graph
     */
    int Tree::getTourCost(const Graph & graph) const {
        int sum = 0;
        for(WeightedEdge e : _edges){
            sum += graph.getEdgeWeight(e.a(), e.b());
        }
        return sum;
    }

    std::set<WeightedEdge> Tree::getConnectedEdges(NodeId v) const {
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

    bool Tree::isIllegal() const
    {
        return _illegal;
    }

    void Tree::makeTrivialTour(const Graph & Graph){
        _illegal = false;
        _numVertices = Graph.getNumVertices();
        _vertexDegrees = std::vector<int>(_numVertices, 2);
        size_t numVertices = Graph.getNumVertices();
        for(unsigned int i = 0; i < _numVertices; i++){
            _edges.push_back(WeightedEdge(i, (i+1)%numVertices, Graph.getEdgeWeight(i, (i+1)%numVertices)));
        }
    }
}