#ifndef MYQUEUE_HPP
#define MYQUEUE_HPP

#include <stdexcept>
#include <queue>
#include <set>
#include "edge.hpp"
#include "rfList.hpp"

namespace TSP {

    class QueueElement {
        private:
        RFList rf;
        int cost;
        std::vector<int> lambda;
        public:
        QueueElement(RFList rf, int cost, std::vector<int> lambda);
        bool operator< (const QueueElement& rhs) const;
        RFList getRF();
        std::vector<int> getLambda();
        int getCost();


    };

    class MyQueue {
    private:
        std::priority_queue<QueueElement> _queue;

    public:
        size_t size();
        void push(RFList rf, int cost, std::vector<int> lambda);
        void push(const QueueElement& e);
        QueueElement pop();
    };
}

#endif
