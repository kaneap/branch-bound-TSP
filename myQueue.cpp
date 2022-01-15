#include "myQueue.hpp"

namespace TSP {

    QueueElement::QueueElement(RFList rf, int cost, std::vector<int> lambda): rf(rf), cost(cost), lambda(lambda) {}

    bool QueueElement::operator< (const QueueElement& rhs) const {
        //reversed because elements of low cost are at the top of the queue
        return this->cost > rhs.cost;
    }

    RFList QueueElement::getRF() {
        return this->rf;
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

    void MyQueue::push(RFList rf, int cost, std::vector<int> lambda) {
        QueueElement e(rf, cost, lambda);
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