#ifndef MYQUEUE_HPP
#define MYQUEUE_HPP

#include <stdexcept>
#include <queue>
#include <set>
#include "edge.hpp"

namespace TSP {

    class QueueElement {
        std::set<Edge> required;
        std::set<Edge> forbidden;
        std::vector<int> lambda;
        int cost;
        public:
        QueueElement(std::set<Edge> required, std::set<Edge> forbidden, int cost, std::vector<int> lambda);
        bool operator< (const QueueElement& rhs) const;
        std::set<Edge> getRequired();
        std::set<Edge> getForbidden();
        std::vector<int> getLambda();
        int getCost();


    };

    class MyQueue {
    private:
        std::priority_queue<QueueElement> _queue;

    public:
        size_t size();
        void push(std::set<Edge> required, std::set<Edge> forbidden, int cost, std::vector<int> lambda);
        void push(const QueueElement& e);
        QueueElement pop();
    };
}

#endif
