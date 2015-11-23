#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#define N 10

using namespace std;

template<class T>
class lockFreeQueue
{
    struct Node
    {
        T value;
        atomic<Node*> next;
        Node(T value): value(value), next(nullptr) { }
    //    Node(): value(-1), next(nullptr) { }
    };

    atomic<Node*> head;
    atomic<Node*> tail;

public:

    lockFreeQueue()
    {
        head.store(new Node( T(-1) ));
        tail = head.load();
    }
    ~lockFreeQueue()
    {
        while(head != nullptr)
        {
            Node* p = head.load();
            head.store(p->next);
            delete p;
        }
    }

    void enq(const T& value)
    {
        Node* node = new Node(value);
        Node* last;
        Node* next;
        while(true)
        {
            last = tail.load();
            next = last->next.load();
            if(last == tail.load())
            {
                if(next == nullptr)
                {
                    if(atomic_compare_exchange_weak(&(last->next), &next, node))
                    {
                        atomic_compare_exchange_weak(&tail, &last, node);
                        return;
                    }
                }
                else
                {
                    atomic_compare_exchange_weak(&tail, &last, next);
                }
            }
        }
    }

    T deq()
    {
        Node* first;
        Node* last;
        Node* next;
        while(true)
        {
            first = head.load();
            last = tail.load();
            next = first->next.load();
            if(first == head.load())
            {
                if(first == last)
                {
                    if(next == nullptr)
                    {
                        return -1;
                    }
                    atomic_compare_exchange_weak(&tail,&last,next);
                }
                else
                {
                    T value = next->value;
                    if(atomic_compare_exchange_weak(&head,&first,next))
                    {
                        return value;
                    }
                }
            }
        }
        delete first, last, next;
    }

    void print()
    {
        Node* p;
        p = head.load()->next.load();
        cout << "HEAD -> ";
        while(p != nullptr)
        {
            if(p->value == -1) cout << "nullptr -> ";
            else if(p->next.load() != nullptr) cout << p->value << " -> ";
            else cout << p->value << " <- ";

            p = p->next.load();
        }
        cout << "TAIL\n";
    }
};

template<class C>
void asd() {}

class lol
{
public:

        void lel() {}
};

int main()
{
    lockFreeQueue<int> q;
    vector<thread> th;

    for(int i=0; i<N; ++i)
    {
        th.push_back(thread(&lockFreeQueue<int>::enq,&q,i));
    }
    q.print();

    for(int i=0; i<N; ++i)
    {
        th[i].join();
    }

    int a = q.deq();
    return 0;
}















