#include "myQueue.hpp"

namespace TSP {

    QueueElement::QueueElement(std::set<Edge> required, std::set<Edge> forbidden, int cost, std::vector<int> lambda) {
        this->required = required;
        this->forbidden = forbidden;
        this->cost = cost;
        this->lambda = lambda;
    }

    bool QueueElement::operator< (const QueueElement& rhs) const {
        //reversed because elements of low cost are at the top of the queue
        return this->cost > rhs.cost;
    }

    std::set<Edge> QueueElement::getRequired() {
        return this->required;
    }


    std::set<Edge> QueueElement::getForbidden() {
        return this->forbidden;
    }

    std::vector<int> QueueElement::getLambda() {
        return this->lambda;
    }

    int QueueElement::getCost() {
        return this->cost;
    }

    size_t MyQueue::size() {
        return _queue.size();
    }

    void MyQueue::push(std::set<Edge> required, std::set<Edge> forbidden, int cost, std::vector<int> lambda) {
        QueueElement e(required, forbidden, cost, lambda);
        _queue.push(e);
    }

    void MyQueue::push(const QueueElement& e) {
        _queue.push(e);
    }

    QueueElement MyQueue::pop() {
        QueueElement e = _queue.top();
        _queue.pop();
        return e;
    }
}