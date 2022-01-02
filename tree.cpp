#include "tree.hpp"

namespace TSP
{
    Tree::Tree(const Graph &graph, NodeId one)
    {
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
        auto allEdges = graph.getEdges();

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
            _vertexDegrees[e.first]++;
            _vertexDegrees[e.second]++;
        }
    }

    bool Tree::is2Regular()
    {
        for (int degree : _vertexDegrees)
        {
            if (degree != 0)
                return false;
        }
        return true;
    }

    int Tree::getDegree(NodeId v)
    {
        return _vertexDegrees[v];
    }

    int Tree::getTourLength()
    {
        if (!is2Regular()) return -1; //TODO: return exeption?? Or just invalid value?
        int sum = 0;
        for(WeightedEdge e : _edges){
            sum += e.getWeight();
        }
        return sum;
    }

    std::set<WeightedEdge> Tree::getConnectedEdges(NodeId v){
        std::set<WeightedEdge> connected;
        for(WeightedEdge e : _edges){
            if(e.connectsVertex(v)) connected.insert(e);
        }
        //TODO: should I return reference?
        return connected;
    }
}