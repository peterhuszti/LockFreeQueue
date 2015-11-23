#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <future>

#define numberOfThreads 20

using namespace std;

class exc: public exception
{
    const char* what() const throw()
    {
        return "\n\nDequeing from an empty queue!\n\n";
    }
} emptyQ;

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
        while(true)
        {
            Node* last = tail.load();
            Node* next = last->next.load();
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

    void deq(promise<T>* res)
    {
        while(true)
        {
            Node* first = head.load();
            Node* last = tail.load();
            Node* next = first->next.load();
            if(first == head.load())
            {
                if(first == last)
                {
                    if(next == nullptr)
                    {
                        throw emptyQ;
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
            cout << p->value;
            if(p->next.load() != nullptr) cout << " -> ";
            else cout << " <- ";

            p = p->next.load();
        }
        cout << "TAIL\n";
    }
};

int main()
{
    lockFreeQueue<int> q;

//Enque
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

//Deque
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
            cout << fut[i].get() << "  ";
        }
        catch(exception& e)
        {
            cout << e.what();
        }
    }
    for(int i=0; i<numberOfThreads; ++i)
    {
        thD[i].join();
    }
    cout<<"\n";

//Enque and deque together
    cout << "\nEQ and DQ together:\n";

    lockFreeQueue<int> lfq;
    vector<thread> th;
    vector<promise<int> > prom(numberOfThreads / 2);
    vector<future<int> > futu;
    for(int i=0; i<numberOfThreads/2; ++i)
    {
        futu.push_back(prom[i].get_future());
    }

    for(int i=0; i<numberOfThreads; ++i)
    {
        if(i%2 == 0)
        {
            th.push_back( thread( &lockFreeQueue<int>::enq, &lfq, i/2) );
        }
        else
        {
            try
            {
                th.push_back( thread( &lockFreeQueue<int>::deq, &lfq, &prom[i/2] ) );
                cout << futu[i/2].get() << "  ";
            }
            catch(exception e)
            {
                cout << e.what();
            }

        }
    }

    for(int i=0; i<numberOfThreads; ++i)
    {
        th[i].join();
    }
    cout<<"\n";

    return 0;
}















