#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <limits>
#include <algorithm>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <utility>
#include "graph.hpp"
//#include "tree.hpp"


int main(int argc, char*argv[]){
    if(argc != 2) {
        std::cout << "Please specify a filename of a DIMACS format graph file!" << std::endl;
        return 1;
    }
    std::string filename (argv[1]);
    TSP::Graph g(filename);
    return 0;
}
