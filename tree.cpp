#include "tree.hpp"

namespace TSP
{
    Tree::Tree(): _numVertices(0), _illegal(true){}

    /**
     * @brief Constructs a 1-tree using Krushkal's algorithm
     * @param graph Graph to construct tree from
     * @param rf RFList to indicate which edges are required and forbidden
     */
    Tree::Tree(const Graph & graph, const RFList & rf):
    _edges(),
    _vertexDegrees(graph.getNumVertices()),
    _illegal(false)
    {
        if(!rf.isValid()) {
            _illegal = true;
        }
        NodeId one = 0;
        //first, use Kruskal's algorithm to create a minimum spanning tree
        std::vector<Element*> vertexSets;
        this->_numVertices = graph.getNumVertices();
        for (NodeId i = 0; i < graph.getNumVertices(); i++) {
            vertexSets.push_back(Union::makeSet(i));
        }
        auto allEdges = graph.getEdges(rf);

        unsigned edgeCount = 0;
        for (const auto & e : allEdges) { 
            //want n-2 edges (i.e. to connect all except the 1-vertex)
            if (edgeCount >= graph.getNumVertices()-2) break;
            NodeId u = e.a();
            NodeId v = e.b();
            if(u == one || v == one) continue;
            if(Union::findSet(vertexSets[u])->value != Union::findSet(vertexSets[v])->value){
                if(rf.isForbidden(e)){
                    _illegal = true;
                }
                //make new weighted edge so we evalutate wrt the original edge weights
                _edges.push_back(WeightedEdge(u, v, graph.getEdgeWeight(u, v)));
                Union::makeUnion(vertexSets[u], vertexSets[v]);
                edgeCount++;
            }
        }

        std::vector<WeightedEdge> oneEdges;
        for(const auto & e : allEdges){
            if(e.a() == one || e.b() == one){
                oneEdges.push_back(e);
            }
        }

        //free the memory allocated by the disjoint sets
        for(Element * e : vertexSets){
            delete e;
        }

        //second, we connect the two cheapest edges connected to the 1 vertex to the tree
        WeightedEdge first = oneEdges[0];
        WeightedEdge second = oneEdges[1];
        if(first.getWeight() > second.getWeight()){
            std::swap(first, second);
        }
        for(unsigned int i = 2; i < oneEdges.size(); i++){
            WeightedEdge current = oneEdges[i];
            if (current.getWeight() < first.getWeight()){
                second = first;
                first = current;
            } else if (current.getWeight() < second.getWeight()) {
                second = current;
            } 
        }

        if(rf.isForbidden(first) || rf.isForbidden(second)){
            _illegal = true;
        }

        _edges.push_back(WeightedEdge(first.a(), first.b(), graph.getEdgeWeight(first.a(), first.b())));
        _edges.push_back(WeightedEdge(second.a(), second.b(), graph.getEdgeWeight(second.a(), second.b())));

        for (auto e : _edges)
        {
            _vertexDegrees[e.a()]++;
            _vertexDegrees[e.b()]++;
        }
    }

    /**
     * @brief returns whether the 1-tree is 2 regular, and thus a possible solution
     * 
     * @return true if all the vertices have degree 2
     * @return false 
     */
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