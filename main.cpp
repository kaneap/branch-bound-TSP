#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <limits>
#include <limits>
#include <algorithm>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <utility>
#include "graph.hpp"
#include "graph.hpp"
#include "tree.hpp"
#include "union.hpp"
#include "edge.hpp"
#include <math.h>
#include <numeric>

namespace TSP{

    /**
     * @brief This correspond to c(T, λ) on page 1 of the assignment.
     * @param Tree
     * @return lambda
     */
    int getCost(const Tree & tree, std::vector<int> & lambda){
        int degeeSum = 0;
        for(int i = 0; i < tree.getNumVertices(); i++){
            degeeSum += ((tree.getDegree(i) - 2) * lambda[i]);
        }
        return tree.getTourCost() + degeeSum;
    }


    std::vector<int> HK(const Graph & graph, std::vector<int> lambda, float t_0, std::set<Edge> required, std::set<Edge> forbidden){
        std::vector<int> lambda(graph.getNumVertices(), 0);
        int n = graph.getNumVertices();
        int N = ceil((n) / 4) + 5;
        Tree* tree = nullptr;
        Tree* lastTree = nullptr;
        float t = t_0;
        float delta = (3 * t) / (2 * N);
        float deltaStep = t / (N*N - N);

        Graph updated = graph;
        for(int i = 0; i < N; i++){
            lastTree = tree;
            tree = new Tree(updated, required, forbidden);
            for(int v : lambda){
                lambda = lastTree == nullptr?
                    findNextLambda(*tree, lambda, t) : 
                    findNextLambdaVJ(*tree, *lastTree, lambda, t);
            }
            t -= delta;
            delta -= deltaStep;
            Graph updated(updated.getNumVertices(), updated.updatedEdgeCosts(lambda));
            delete lastTree;
        }
        delete tree;
        return lambda;
    }

    /**
     * @brief Finds a lambda that maximizes HK(G, lambda);
     * @param Tree
     * @return lambda
     */
    std::vector<int> HK_root(const Graph & graph){
        std::vector<int> lambda(graph.getNumVertices(), 0);
        int n = graph.getNumVertices();
        int N = ceil((n*n) / 50) + n + 15;
        Tree* tree = new Tree(graph);
        Tree* lastTree = nullptr;
        float t = tree->getTourCost() / (2.0 * n);
        delete tree;
        tree = nullptr;
        float delta = (3 * t) / (2 * N);
        float deltaStep = t / (N*N - N);
        Graph updated = graph;
        for(int i = 0; i < N; i++){
            lastTree = tree;
            tree = new Tree(updated);
            for(int v : lambda){
                lambda = lastTree == nullptr?
                    findNextLambda(*tree, lambda, t) : 
                    findNextLambdaVJ(*tree, *lastTree, lambda, t);
            }
            t -= delta;
            delta -= deltaStep;
            Graph updated(updated.getNumVertices(), updated.updatedEdgeCosts(lambda));
            delete lastTree;
        }
        delete tree;
        return lambda;
    }

    /**
     * @brief 
     * 
     * @param graph 
     * @param lambda 
     * @return int 
     */
    std::vector<int> findNextLambda(const Tree & tree, const std::vector<int> & lambda, float t_i){
        if(tree.getNumVertices() != lambda.size())
            throw std::runtime_error("Graph and lambda must have the same number of vertices.");
        std::vector<int> nextLambda (tree.getNumVertices());
        for(int i = 0; i < lambda.size(); i++){
            nextLambda[i] = lambda[i] + t_i * (tree.getDegree(i) - 2);    
        }
        return nextLambda;
    }

    /**
     * @brief 
     * 
     * @param graph 
     * @param lambda 
     * @return int 
     */
    std::vector<int> findNextLambdaVJ(const Tree & tree, const Tree & lastTree, std::vector<int> lambda, float t_i){
        if(tree.getNumVertices() != lambda.size() || lastTree.getNumVertices() != lambda.size())
            throw std::runtime_error("Graph and lambda must have the same number of vertices.");
        std::vector<int> nextLambda (tree.getNumVertices());
        for(int i = 0; i < lambda.size(); i++){
            //if we have time, might want to get rid of magic numbers
            nextLambda[i] = lambda[i] + t_i * (0.6 * (tree.getDegree(i) - 2) + 0.4 * (lastTree.getDegree(i) - 2));    
        }
        return nextLambda;
    }

    Tree branchAndBound(Graph& graph){
        //todo
        int numVertices = graph.getNumVertices();
        std::vector<std::pair<std::set<Edge>, std::set<Edge>>> Q;
        std::vector<std::vector<bool>> seenEdges (numVertices, std::vector<bool>((numVertices, false)));
        Q.push_back(std::make_pair(std::set<Edge>(), std::set<Edge>()));
        int upperLimit = std::numeric_limits<int>::max();
        //TODO: this is a tour of nothing, might need to make a special case for this
        Tree shortestTour;
        float t_0 = -1;
        bool root = true;

        //When Q is empty, we have a solution.
        while(Q.size() > 0){
            //TODO: there are more optimal ways to select the next node. See page 3 of the assignment.
            std::pair<std::set<Edge>, std::set<Edge>> current_pair = Q.back();
            Q.pop_back();
            auto required = current_pair.first;
            auto forbidden = current_pair.second;

            // Compute λ s.t. the weight of a min-c λ -cost 1-tree T with R ⊆ E(T ) ⊆ E(K n ) \ F is approximately maximum 
            std::vector<int> lambda;
            if(root){
                //if we are at the root node in Q, we call HK_root as it runs more iterations
                lambda = HK_root(graph);
                t_0 = accumulate(lambda.begin(),lambda.end(),0) / (2.0 * lambda.size());
                root = false;
            }else{
                lambda = HK(graph, lambda, t_0, required, forbidden);
            }

            Tree t (graph, required, forbidden);
            if(t.is2Regular()) {
                //the tree is 2 regular, so it is a tour
                if(t.getTourCost() < upperLimit){
                    //we check if this tour is better than the best tour we have seen so far
                   upperLimit = t.getTourCost();
                   shortestTour = t;
                }
            }else{
                //there needs to be a vertex 2 ≤ i ≤ n with degree > 2.
                NodeId i = invalid_node_id;
                //TODO: should looking for this vetex be randomized perhaps?
                for(NodeId v = 0; v < graph.getNumVertices(); v++){
                    int degree = t.getDegree(v);
                    if (degree > 2){
                        i = v;
                        break;
                    }
                }
                if(i == invalid_node_id) throw std::runtime_error("No edge has degree > 2 in the tree... something is wrong...");

                std::set<WeightedEdge> connectedEdges = t.getConnectedEdges(i);
                for(WeightedEdge e : connectedEdges){
                    if(required.count(e) != 0 ||  forbidden.count(e) != 0){
                        connectedEdges.erase(e);
                    }
                }

                //remove R and F from connected edges
                for (auto iter = connectedEdges.begin(), end=connectedEdges.end(); iter != end; iter++) {
                    auto e = *iter;
                    //TODO: may need to optimize this. Required and forbidden could be n*n matrices,
                    //but the real performance of the hash map is probably fine.
                    if(required.count(e) != 0 ||  forbidden.count(e) != 0){
                        iter = connectedEdges.erase(iter);
                    }
                }
                if (connectedEdges.size() < 2) throw std::runtime_error("No edge has |δ_T (i) \\ (R ∪ F)| ≥ 2, something is wrong...");

                //calculate the number of required edges incident to e
                int incidentRequired = 0;
                for(Edge e : required){
                    if(e.connectsVertex(i)) incidentRequired++;
                }

                // TODO: need to check that e1 and e2 haven't been branched on already
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
                if(incidentRequired > 0) Q.push_back(std::make_pair(R_e1_e2, forbidden));            
            }
        }
        return shortestTour;
    }
}


int main(int argc, char*argv[]){
    if(argc != 2) {
        std::cout << "Please specify a filename of a DIMACS format graph file!" << std::endl;
        return 1;
    }
    std::string filename (argv[1]);
    TSP::Graph g(filename);
    TSP::Tree t = TSP::branchAndBound(g);
    std::cout << t.toTsplibString() << std::endl;
    return 0;
}

