#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <limits>
#include "graph.hpp"
#include "tree.hpp"
#include "edge.hpp"

namespace TSP{
    /**
     * @brief
     * NOTE: I have no idea whether this is correct.
     * @param filename
     * @return Graph*
     */
    Graph* graph_from_file(std::string const & filename){
        std::ifstream file(filename);
        if (!file.is_open()){
            throw std::runtime_error("Could not open file " + filename);
        }
        Graph * graph = nullptr;
        while(file.is_open()){
            std::string line;
            std::string n, a, u, v = "";
            while(getline(file, line)){
                std::stringstream ss(line);
                std::string token;
                for(int i = 0; getline(ss, token, ' ');){
                    if(i == 0 && token.compare("c") == 0) break;
                    if(i == 0){
                        a = token;
                    } else if (i == 1) {
                        if(a.compare("e") == 0){
                            u = token;
                        }
                    } else if (i == 2) {
                        if(a.compare("p") == 0){
                            n = token;    
                        }
                        else if(a.compare("e") == 0){
                            v = token;
                        }
                    }
                    i++;
                }
                if(a.compare("p") == 0){
                    if(graph != nullptr)
                        throw new std::runtime_error("DIMACS file error: Graph defined more than once.");
                    int vertices = std::stoi(n);
                    graph = new Graph(vertices);
                }
                else if(a.compare("e") == 0){
                    if (graph == nullptr) 
                        throw new std::runtime_error("DIMACS file error: edges must be listed after the definition of the graph.");
                    int v1 = std::stoi(u)-1;
                    int v2 = std::stoi(v)-1;
                    graph->setEdgeWeight(v1, v2, 1); //todo
                }
            }
            file.close();
        }
        return graph;
    }


    int findLowerbound(Graph * g){
        int N = g->getNumVertices(); // todo: do we need to fix the iterations N?
        std::vector<int> lowerbound (g->getNumVertices());
        for(int i = 0; i < N; i++){
            //make g1 a copy of g but with the lambda added to the edge weight (as shown in class)
            Graph g1 (g->getNumVertices());
            for(int j = 0; j < N; j++){
                for(int k = 0; k < N; k++){
                    if(j != k){
                        int w = g->getEdgeWeight(j, k) + lowerbound[j] + lowerbound[k];
                        g1.setEdgeWeight(j, k, w);
                    }
                }
            }
            Tree t (g1);
            double t_i = 1.0 / (i + 1); //may need to change this. See page 4 of the assignment
            for(int j = 0; j < lowerbound.size(); j++){
                lowerbound[j] += (g->getEdgeWeight(i, j) + t_i * (t.getDegree(j) - 2));
            }

        }
        for(int i = 0; i < g->getNumVertices(); i++){
            for(int j = i+1; j < g->getNumVertices(); j++){
                int weight = g->getEdgeWeight(i, j);
                if(weight > 0){
                    lowerbound += weight;
                }
            }
        }
        return lowerbound;
    }

    // TODO Should lamda be doubles?
    /**
     * @brief 
     * 
     * @param graph 
     * @param lambda 
     * @return int 
     */
    int findLowerBound(Graph * graph, std::vector<int> lambda){
        //todo
    }

    void branchAndBound(Graph& graph, NodeId start_node, NodeId end_node, int max_depth, int max_weight){
        //todo
        std::vector<std::pair<std::set<Edge>, std::set<Edge>>> Q;
        std::vector<Edge> seenEdges;
        Q.push_back(std::make_pair(std::set<Edge>(), std::set<Edge>()));
        int upperLimit =  std::numeric_limits<NodeId>::max(); //TODO: what should initial value be?
        Tree shortestTour;
         
        while(Q.size() > 0){
            std::pair<std::set<Edge>, std::set<Edge>> current_pair = Q.back();
            Q.pop_back();
            auto required = current_pair.first;
            auto forbidden = current_pair.second;

            // TODO: compute λ s.t. the weight of a min-c λ -cost 1-tree T with R ⊆ E(T ) ⊆ E(K n ) \ F is approximately maximum 
            // TODO: create a 1-tree from this λ
            Tree t;
            if(t.is2Regular()){
                if(t.getTourLength() < upperLimit) {
                   upperLimit = t.getTourLength();
                   shortestTour = t;
                }
            }
            else{
                
                //there needs to be a vertex 2 ≤ i ≤ n with |δ T (i)| > 2.
                NodeId i = invalid_node_id;
                //should looking for this vetex be randomized perhaps?
                for(NodeId v = 0; v < graph.getNumVertices(); v++){
                    int degree = t.getDegree(v);
                    if (degree > 2){
                        i = v;
                        break;
                    }
                }
                if(i == invalid_node_id) throw std::exception("No edge has degree > 2 in the tree... something is wrong...");

                std::set<WeightedEdge> connectedEdges = t.getConnectedEdges(i);
                for(WeightedEdge e : connectedEdges){
                    if(required.count(e) != 0 ||  forbidden.count(e) != 0){
                        connectedEdges.erase(e);
                }

                //remove R and F from connected edges
                for (auto iter = connectedEdges.begin(), end=connectedEdges.end(); iter != end; iter++) {
                    auto e = *iter;
                    if(required.count(e) != 0 ||  forbidden.count(e) != 0){
                        iter = connectedEdges.erase(iter);
                    }
                }
                if (connectedEdges.size() < 2) throw std::exception("No edge has |δ_T (i) \\ (R ∪ F)| ≥ 2, something is wrong...");

                auto iter = connectedEdges.begin();
                //add to q 3 new nodes
                Edge e1 = *iter;
                iter++;
                Edge e2 = *iter;
                auto R_e1 = required;
                R_e1.insert(e1);
                auto R_e1_e2 = R_e1;
                R_e1_e2.insert(e2);
                auto F_e1 = forbidden;
                F_e1.insert(e1);
                auto F_e2 = forbidden;
                F_e2.insert(e2);
                Q.push_back(std::make_pair(required, F_e1));
                Q.push_back(std::make_pair(R_e1, F_e2));
                //TODO: omit where the last node is omitted if there is already a required edge incident to i
                Q.push_back(std::make_pair(R_e1_e2, forbidden));
            }            

            // if c(T, λ) ≥ U then continue
            std::vector<int> lambda;

        }
    }
}


int main(int argc, char*argv[]){
    if(argc != 2) {
        std::cout << "Please specify a filename of a DIMACS format graph file!" << std::endl;
        return 1;
    }
    std::string filename (argv[1]);
    TSP::Graph* g = TSP::graph_from_file(filename);
    return 0;
}
