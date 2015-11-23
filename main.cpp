#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <future>

#define numberOfThreads 10

using namespace std;

enum e{EMPTYQUEUE};

template<class T>
class lockFreeQueue
{
    struct Node
    {
        T value;
        atomic<Node*> next;
        Node(T value): value(value), next(nullptr) { }
        Node(): next(nullptr) { }
    };

    atomic<Node*> head;
    atomic<Node*> tail;

public:

    lockFreeQueue()
    {
        head.store(new Node( T() ));
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
        delete tail;
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

    void deq(promise<int>* res)
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
                        throw EMPTYQUEUE;
                    }
                    atomic_compare_exchange_weak(&tail,&last,next);
                }
                else
                {
                    T value = next->value;
                    if(atomic_compare_exchange_weak(&head,&first,next))
                    {
                        res->set_value(value);
                        return;
                    }
                }
            }
        }
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

int main()
{
    lockFreeQueue<int> q;
    vector<thread> thE;

    for(int i=0; i<numberOfThreads; ++i)
    {
        thE.push_back( thread( &lockFreeQueue<int>::enq, &q, i) );
    }

    for(int i=0; i<numberOfThreads; ++i)
    {
        thE[i].join();
    }
    q.print();

    vector<thread> thD;
    cout<<"\nDEQUE: ";

    vector<promise<int> > pro(numberOfThreads);
    vector<future<int> > fut;
    for(int i=0; i<numberOfThreads; ++i)
    {
        fut.push_back(pro[i].get_future());
    }

    for(int i=0; i<numberOfThreads; ++i)
    {
        try
        {
            thD.push_back( thread( &lockFreeQueue<int>::deq, &q, &pro[i] ) );
        }
        catch(enum e)
        {
            cout << "Dequeing from empty queue\n";
        }
    }
    for(int i=0; i<numberOfThreads; ++i)
    {
        thD[i].join();
    }

    for(int i=0; i<numberOfThreads; ++i)
    {
        cout << fut[i].get() << "  ";
    }
    cout<<"\n";

    return 0;
}















