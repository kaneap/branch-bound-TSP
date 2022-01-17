#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <limits>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <utility>
#include <fstream>
#include <cmath>
#include <set>
#include <vector>
#include "nodeId.hpp"
#include "weightedEdge.hpp"
#include "rfList.hpp"

namespace TSP {

   /**
      @class Graph

      @brief A @c Graph stores an array of @c Node s, but no array of edges. The list of edges is implicitly given
      by the fact that the nodes know their neighbors.

      This class models undirected graphs only (in the sense that the method @c add_edge(node1, node2) adds both @c node1
      as a neighbor of @c node2 and @c node2 as a neighbor of @c node1). It also forbids loops, but parallel edges are
      legal.

      @warning Nodes are numbered starting at 0, as is usually done in programming, instead starting at 1, as is done in
      the DIMACS format that your program should take as input! Be careful.
   **/
   class Graph {

   public:

      typedef LargeUnsigned size_type;

      /**
         @brief Creates a @c Graph with @c num_nodes isolated nodes.

         Edge weights are assigned internally as -1 initially to indicate that the edge hasn't been assigned.
      **/
      Graph (size_type const numVertices);
      Graph (std::string const &filename);
      Graph (size_type const numVertices, std::vector<std::vector<int>> edges);
      Graph (const Graph & other, const std::vector<int> & lambda);

      /** @return The number of nodes in the graph. **/
      size_type num_nodes () const;

      /** @return The number of nodes in the graph that aren't deleted. **/
      size_type num_nodes_active() const;

      /** @return The number of edges in the graph. **/
      size_type num_edges () const;

      /**
         @brief Sets the edge weight for the edge between the two vertices

         Checks that @c node1_id and @c node2_id are distinct and throws an exception otherwise.
         This method adds both @c node1_id as a neighbor of @c node2_id and @c node2_id as a neighbor of @c node1_id.
         Also checks that the edge weight has been assigned and throws an execption if not.

         @warning Does not check that the edge does not already exist, so this class can be used to model non-simple
         graphs.
      **/
      void setEdgeWeight(NodeId node1_id, NodeId node2_id, int weight);
      int getEdgeWeight(NodeId const node1_id, NodeId const node2_id) const;
      size_type getNumVertices() const;
      size_type getNumEdges() const;
      // todo: this vector pair pair thing is ugly... probably should be an object...
      std::vector<WeightedEdge> getEdges (const RFList & rf) const;
      std::vector<WeightedEdge> getConnectedEdges(NodeId v) const;
      std::vector<std::vector<int>> updatedEdgeCosts(std::vector<int> lambda) const ;
      unsigned int getUpperBound() const;
   private:
      std::vector<std::vector<int>> _edges;
      size_type _numVertices;
      void initWeights();
      int getPosition(NodeId a, NodeId b) const;
   };
}

#endif
