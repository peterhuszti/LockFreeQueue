#ifndef LOCKFREEQUEUE_H
#define LOCKFREEQUEUE_H

#include <iostream>
#include <atomic>
#include "exc.h"

template<class T>
class lockFreeQueue
{
    struct Node
    {
        T value;
        std::atomic<Node*> next;
        Node(T value): value(value), next(nullptr) { }
        Node(): next(nullptr) { }
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;
    exc emptyQ;

public:

    lockFreeQueue();
    ~lockFreeQueue();

    void enq(const T& value);
    void deq(T& res);

    void print();
};

#endif // LOCKFREEQUEUE_H
