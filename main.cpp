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

    /**
     * @brief Now, let T_λ be a min-c_λ-cost 1-tree (which also minimizes c(T_λ , λ)), and set
     *  HK(K n , c, λ) := c(T λ , λ)
    */   
    int HK(const Graph & graph, std::vector<int> & lambda){
        //todo  
    }

    std::vector<int> HK(const Graph & graph, std::vector<int> lambda, float t_0, std::set<Edge> required, std::set<Edge> forbidden){
        std::vector<int> lambda(graph.getNumVertices(), 0);
        int n = graph.getNumVertices();
        int N = ceil((n) / 4) + 5;
        Tree tree (graph, required, forbidden);
        float t = t_0;
        float delta = (3 * t) / (2 * N);
        float deltaStep = t / (N*N - N);
        for(int i = 0; i < N; i++){
            tree = Tree(graph, lambda, required, forbidden);
            for(int v : lambda){
                lambda = findNextLambda(tree, lambda, t);
                //todo: also use VJ lambda
            }
            t -= delta;
            delta -= deltaStep;
        }
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
        Tree tree (graph);
        float t = tree.getTourCost() / (2.0 * n);
        float delta = (3 * t) / (2 * N);
        float deltaStep = t / (N*N - N);
        for(int i = 0; i < N; i++){
            tree = Tree(graph, lambda);
            for(int v : lambda){
                lambda = findNextLambda(tree, lambda, t);
                //todo: also use VJ lambda
            }
            t -= delta;
            delta -= deltaStep;
        }
        return lambda;
    }

    //probably don't need this
    /*
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
    */


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


    void branchAndBound(Graph& graph){
        //todo
        std::vector<std::pair<std::set<Edge>, std::set<Edge>>> Q;
        std::vector<Edge> seenEdges;
        std::vector<int> lambda;
        Q.push_back(std::make_pair(std::set<Edge>(), std::set<Edge>()));
        int upperLimit =  std::numeric_limits<NodeId>::max(); //TODO: what should initial value be? See page 3 of assignment.
        //TODO: this is a tour of nothing, might need to make a special case for this
        Tree shortestTour;
        float t_0 = -1;
         
        bool root = true;
        while(Q.size() > 0){
            std::pair<std::set<Edge>, std::set<Edge>> current_pair = Q.back();
            Q.pop_back();
            auto required = current_pair.first;
            auto forbidden = current_pair.second;

            // TODO: compute λ s.t. the weight of a min-c λ -cost 1-tree T with R ⊆ E(T ) ⊆ E(K n ) \ F is approximately maximum 
            // TODO: create a 1-tree from this λ
            if(root){
                lambda = HK_root(graph);
                t_0 = accumulate(lambda.begin(),lambda.end(),0) / (2.0 * lambda.size());
                root = false;
            }
            else{
                lambda = HK(graph, lambda, t_0, required, forbidden);
            }
            Tree t (graph, required, forbidden);
            if(t.is2Regular()){
                if(t.getTourCost() < upperLimit) {
                   upperLimit = t.getTourCost();
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
                if(i == invalid_node_id) throw std::runtime_error("No edge has degree > 2 in the tree... something is wrong...");

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
                if (connectedEdges.size() < 2) throw std::runtime_error("No edge has |δ_T (i) \\ (R ∪ F)| ≥ 2, something is wrong...");

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
        }
    }
}


int main(int argc, char*argv[]){
    if(argc != 2) {
        std::cout << "Please specify a filename of a DIMACS format graph file!" << std::endl;
        return 1;
    }
    std::string filename (argv[1]);
    TSP::Graph g(filename);
    return 0;
}
