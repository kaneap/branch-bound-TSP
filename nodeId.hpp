#ifndef NODEID_HPP
#define NODEID_HPP
#include <limits>

namespace TSP {
   typedef std::size_t LargeUnsigned;

   /** Always use this typedef to identify nodes by their numbers. **/
   typedef LargeUnsigned NodeId;
   /** Useful constant different from the id of any actual node: **/
   NodeId const invalid_node_id = std::numeric_limits<NodeId>::max();
}
#endif