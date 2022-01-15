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
#include "myQueue.hpp"
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
        for(unsigned int i = 0; i < tree.getNumVertices(); i++){
            degeeSum += ((tree.getDegree(i) - 2) * lambda[i]);
        }
        return tree.getTourCost() + degeeSum;
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
        for(unsigned int i = 0; i < lambda.size(); i++){
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
        for(unsigned int i = 0; i < lambda.size(); i++){
            //if we have time, might want to get rid of magic numbers
            nextLambda[i] = lambda[i] + t_i * (0.6 * (tree.getDegree(i) - 2) + 0.4 * (lastTree.getDegree(i) - 2));    
        }
        return nextLambda;
    }



    std::vector<int> HK(const Graph & graph, std::vector<int> lambda, float t_0, std::set<Edge> required, std::set<Edge> forbidden){
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
            lambda = lastTree == nullptr?
                    findNextLambda(*tree, lambda, t) : 
                    findNextLambdaVJ(*tree, *lastTree, lambda, t);
            t -= delta;
            delta -= deltaStep;
            updated = Graph(updated, lambda);
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
            lambda = lastTree == nullptr?
                findNextLambda(*tree, lambda, t) : 
                findNextLambdaVJ(*tree, *lastTree, lambda, t);
            t -= delta;
            delta -= deltaStep;
            updated = Graph(updated, lambda);
            delete lastTree;
        }
        delete tree;
        return lambda;
    }

    Tree branchAndBound(Graph& graph){

        size_t numVertices = graph.getNumVertices();
        MyQueue Q;

        std::vector<std::vector<bool>> seenEdges (numVertices, std::vector<bool>(numVertices, false));
        std::vector<int> lambda = HK_root(graph);
        Tree rootTree(Graph(graph, lambda));
        int cost = getCost(rootTree, lambda);
        Q.push(std::set<Edge>(), std::set<Edge>(), cost, lambda);

        int upperLimit = std::numeric_limits<int>::max();
        //TODO: this is a tour of nothing, might need to make a special case for this
        Tree shortestTour;
        float t_0 = accumulate(lambda.begin(),lambda.end(),0) / (2.0 * lambda.size());

        //When Q is empty, we have a solution.
        while(Q.size() > 0){
            //TODO: there are more optimal ways to select the next node. See page 3 of the assignment.
            auto elem = Q.pop();
            auto required = elem.getRequired();
            auto forbidden = elem.getForbidden();

            // Compute λ s.t. the weight of a min-c λ -cost 1-tree T with R ⊆ E(T ) ⊆ E(K n ) \ F is approximately maximum 
            lambda = elem.getLambda();
            //lambda = HK(graph, lambda, t_0, required, forbidden);
            Graph modified (graph, lambda);
            Tree t (modified, required, forbidden);
            //cost = getCost(t, lambda);
            if(t.is2Regular()) {
                //the tree is 2 regular, so it is a tour
                //maybe we should do this step already before adding to Q (according to the assignment)
                if(cost < upperLimit){
                    //we check if this tour is better than the best tour we have seen so far
                   upperLimit = t.getTourCost();
                   shortestTour = t;
                }
            }else{
                //there needs to be a vertex 2 ≤ i ≤ n with degree > 2.
                NodeId i = invalid_node_id;
                //TODO: should looking for this vetex be randomized perhaps?
                for(NodeId v = 0; v < numVertices; v++){
                    int degree = t.getDegree(v);
                    if (degree > 2){
                        i = v;
                        break;
                    }
                }
                if(i == invalid_node_id) throw std::runtime_error("No edge has degree > 2 in the tree... something is wrong...");


                auto connectedEdges = t.getConnectedEdges(i);
                //the connected edges not in R or F
                auto edgesNotRF = connectedEdges;
                unsigned int requiredCount = 0;
                unsigned int forbiddenCount = 0;

                //remove R and F from connected edges
                auto iter = edgesNotRF.begin();
                while (iter != edgesNotRF.end()){
                    auto e = *iter;
                    //TODO: may need to optimize this. Required and forbidden could be n*n matrices,
                    //but the real performance of the hash map is probably fine.
                    if(required.count(e) != 0){
                        iter = edgesNotRF.erase(iter);
                        requiredCount++;
                    } else if (forbidden.count(e) != 0){
                        iter = edgesNotRF.erase(iter);
                        forbiddenCount++;
                    } else {
                        iter++;
                    }
                }
            
                if (edgesNotRF.size() < 2) throw std::runtime_error("An edge has |δ_T (i) \\ (R ∪ F)| ≥ 2, something is wrong...");


                iter = edgesNotRF.begin();
                //add to q 3 new nodes
                Edge e = *iter;
                //TODO: is this what is meant by "two distinct edges in T incident to i that we have not yet branched on"?
                //repeat until we have an unseen edge
                while(seenEdges[e.a()][e.b()]){
                    if(iter == edgesNotRF.end()) throw std::runtime_error("No unseen edge in connected edges, something is wrong?");
                    iter++;
                    e = *iter;
                }
                seenEdges[e.a()][e.b()] = true;
                Edge e1 = *iter;
                //repeat until we have an unseen edge
                while(seenEdges[e.a()][e.b()]){
                    if(iter == edgesNotRF.end()) throw std::runtime_error("No unseen edge in connected edges, something is wrong?");
                    iter++;
                    e = *iter;
                }
                Edge e2 = *iter;
                auto R_e1 = required;
                R_e1.insert(e1);
                auto R_e1_e2 = R_e1;
                R_e1_e2.insert(e2);
                auto F_e1 = forbidden;
                F_e1.insert(e1);
                auto F_e2 = forbidden;
                F_e2.insert(e2);

                if(forbiddenCount + 1 == numVertices - 3){
                    //require all remaining edges
                    for(Edge e : modified.getConnectedEdges(i)){
                        if(F_e1.count(e) == 0){
                            required.insert(e);
                        }
                    }
                }
                std::vector<int> lambda1 = HK(graph, lambda, t_0, required, F_e1);
                Graph modified1 (graph, lambda1);
                Tree t1 (modified1, required, F_e1);
                int cost1 = getCost(t1, lambda1);
                if(cost1 < upperLimit){
                    Q.push(required, F_e1,cost1, lambda1);
                }
                
                if(requiredCount == 1){
                    for(Edge e : modified.getConnectedEdges(i)){
                        if(R_e1.count(e) == 0){
                            F_e2.insert(e);
                        }
                    }
                }else if(forbiddenCount + 1 == numVertices - 3){
                    //require all remaining edges
                    for(Edge e : modified.getConnectedEdges(i)){
                        //forbid if not in F_e1
                        if(F_e2.count(e) == 0){
                            R_e1.insert(e);
                        }
                    }
                }
                std::vector<int> lambda2 = HK(graph, lambda, t_0, R_e1, F_e2);
                Graph modified2 (graph, lambda2);
                Tree t2 (modified2, R_e1, F_e2);
                int cost2 = getCost(t2, lambda2);
                if(cost2 < upperLimit){
                    Q.push(R_e1, F_e2, cost2, lambda2);
                }
                
                if(requiredCount == 0){
                    for(Edge e : modified.getConnectedEdges(i)){
                        //add to F if not in R
                        if(e1 != e && e2 != e){
                            forbidden.insert(e);
                        }
                    }
                    std::vector<int> lambda3 = HK(graph, lambda, t_0, R_e1_e2, forbidden);
                    Graph modified3 (graph, lambda3);
                    Tree t3 (modified3, R_e1_e2, forbidden);
                    int cost3 = getCost(t3, lambda3);
                    if(cost3 < upperLimit){
                        Q.push(R_e1_e2, forbidden, cost3, lambda3);
                    }
                }          
            }
        }
        if(upperLimit == std::numeric_limits<int>::max()) throw std::runtime_error("No tour found");
        return shortestTour;
    }
}


int main(int argc, char*argv[]){
    if(argc != 2) {
        std::cout << "Please specify a filename of a TSPLIB format graph file!" << std::endl;
        return 1;
    }
    std::string filename (argv[1]);
    TSP::Graph g(filename);
    TSP::Tree t = TSP::branchAndBound(g);
    std::cout << t.toTsplibString() << std::endl;
    return 0;
}

