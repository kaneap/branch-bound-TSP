#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include "graph.hpp"
#include "tree.hpp"

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

    void branchAndBound(Graph* graph, NodeId start_node, NodeId end_node, int max_depth, int max_weight){
        //todo
        std::vector<std::pair<std::set<NodeId>, std::set<NodeId>>> Q;
        Q.push_back(std::make_pair(std::set<NodeId>(), std::set<NodeId>()));
         
        while(Q.size() > 0){
            std::pair<std::set<NodeId>, std::set<NodeId>> current_pair = Q.back();
            Q.pop_back();
            auto required = current_pair.first;
            auto forbidden = current_pair.second;

            // TODO: compute λ s.t. the weight of a min-c λ -cost 1-tree T with R ⊆ E(T ) ⊆ E(K n ) \ F is approximately maximum 
            // TODO: create a 1-tree from this λ
            Tree t;
            if(t.is2Regular()){
                //update U
            }
            else{
                //there needs to be a vertex 2 ≤ i ≤ n with |δ T (i)| > 2.

                //add to q 3 new nodes
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
