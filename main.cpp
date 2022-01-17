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
    int getCost(const Tree & tree, std::vector<int> & lambda, const Graph & graph){
        int degeeSum = 0;
        for(unsigned int i = 0; i < tree.getNumVertices(); i++){
            degeeSum += ((tree.getDegree(i) - 2) * lambda[i]);
        }
        //TODO: shoulde we evaluate wrt the original costs?
        return tree.getTourCost(graph) + degeeSum;
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
    std::vector<int> findNextLambdaVJ(const Tree & tree, const Tree & lastTree, const std::vector<int> & lambda, float t_i){
        if(tree.getNumVertices() != lambda.size() || lastTree.getNumVertices() != lambda.size())
            throw std::runtime_error("Graph and lambda must have the same number of vertices.");
        std::vector<int> nextLambda (tree.getNumVertices());
        for(unsigned int i = 0; i < lambda.size(); i++){
            //if we have time, might want to get rid of magic numbers
            nextLambda[i] = lambda[i] + t_i * (0.6 * (tree.getDegree(i) - 2) + 0.4 * (lastTree.getDegree(i) - 2));    
        }
        return nextLambda;
    }



    std::vector<int> HK(
            const Graph & graph, 
            std::vector<int> lambda, 
            float t_0, 
            const RFList & rf){
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
            tree = new Tree(updated, rf);
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
        float t = tree->getTourCost(graph) / (2.0 * n);
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
        int rootCost = getCost(rootTree, lambda, graph);
        Q.push(RFList(numVertices), rootCost, lambda);

        int upperLimit = std::numeric_limits<int>::max();
        //TODO: this is a tour of nothing, might need to make a special case for this
        Tree shortestTour;
        float t_0 = std::accumulate(lambda.begin(),lambda.end(),0) / (2.0 * lambda.size());

        //When Q is empty, we have a solution.
        while(Q.size() > 0){
            auto elem = Q.pop();
            auto rf = elem.getRF();

            // Compute λ s.t. the weight of a min-c λ -cost 1-tree T with R ⊆ E(T ) ⊆ E(K n ) \ F is approximately maximum 
            lambda = elem.getLambda();
            Graph modified (graph, lambda);
            Tree t (modified, rf);
            int cost = elem.getCost();
            //cost = getCost(t, lambda);
            if(t.is2Regular()) {
                //the tree is 2 regular, so it is a tour
                //maybe we should do this step already before adding to Q (according to the assignment)
                if(cost < upperLimit){
                    //we check if this tour is better than the best tour we have seen so far
                   upperLimit = t.getTourCost(graph);
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
                    if(rf.isRequired(e)){
                        iter = edgesNotRF.erase(iter);
                        requiredCount++;
                    } else if (rf.isForbidden(e)){
                        iter = edgesNotRF.erase(iter);
                        forbiddenCount++;
                    } else {
                        iter++;
                    }
                }
            
                if (edgesNotRF.size() < 2){
                    Tree test (modified, rf);
                    throw std::runtime_error("An edge has |δ_T (i) \\ (R ∪ F)| ≥ 2, something is wrong...");
                }
                

                iter = edgesNotRF.begin();
                Edge e1 = *iter;
                Edge e2 = *(++iter);
                
                if(iter != edgesNotRF.end()){
                    RFList rf1 = rf;
                    RFList rf2 = rf;
                    RFList rf3 = rf;

                    rf1.forbid(e1);
                    std::vector<int> lambda1 = HK(graph, lambda, t_0, rf1);
                    Graph modified1 (graph, lambda1);
                    Tree t1 (modified1, rf1);
                    int cost1 = getCost(t1, lambda1, graph);
                    if(cost1 < upperLimit && !t1.isIllegal()){
                        Q.push(rf1, cost1, lambda1);
                    }
                    
                    rf2.require(e1);
                    if(!rf2.is2Regular())
                        rf2.forbid(e2);
                    std::vector<int> lambda2 = HK(graph, lambda, t_0, rf2);
                    Graph modified2 (graph, lambda2);
                    Tree t2 (modified2, rf2);
                    int cost2 = getCost(t2, lambda2, graph);
                    if(cost2 < upperLimit && !t2.isIllegal()){
                        Q.push(rf2, cost2, lambda2);
                    }
                    
                    if(requiredCount == 0){
                        rf3.require(e1);
                        if(rf3.is2Regular()) continue;
                        rf3.require(e2);
                        std::vector<int> lambda3 = HK(graph, lambda, t_0, rf3);
                        Graph modified3 (graph, lambda3);
                        Tree t3 (modified3, rf3);
                        int cost3 = getCost(t3, lambda3, graph);
                        if(cost3 < upperLimit && !t3.isIllegal()){
                            Q.push(rf3, cost3, lambda3);
                        }
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
    std::cout << "Cost: " << t.getTourCost(g) << std::endl;
    std::cout << t.toTsplibString() << std::endl;
    return 0;
}

