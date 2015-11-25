#include "lockFreeQueue.h"

using namespace std;

template<class T>
lockFreeQueue<T>::lockFreeQueue()
{
    head.store(new Node( T() ));
    tail = head.load();
}

template<class T>
lockFreeQueue<T>::~lockFreeQueue()
{
    while(head != nullptr)
    {
        Node* p = head.load();
        head.store(p->next);
        delete p;
    }
}

template<class T>
void lockFreeQueue<T>::enq(const T& value)
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

template<class T>
void lockFreeQueue<T>::deq(T& res)
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
                    res = value;
                    return;
                }
            }
        }
    }
}

template<class T>
void lockFreeQueue<T>::print()
{
    Node* p;
    p = head.load()->next.load();
    cout << "HEAD -> ";
    while(p != nullptr)
    {
        cout << p->value;
        if(p->next.load() != nullptr) cout << "  ";
        else cout << " <- ";

        p = p->next.load();
    }
    cout << "TAIL\n";
}

template class lockFreeQueue<int>;
