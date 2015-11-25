#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include "lockFreeQueue.h"
#include <boost/lockfree/queue.hpp>
#include <chrono>

#define numberOfThreads 20000
#define numberOfTests 1

using namespace std;

lockFreeQueue<int> myQ;
boost::lockfree::queue<int> bQ(64); //? elvileg a meretet adja meg, de nem nem zavarja, ha tobb elemet teszek bele, es nem lesz gyorsabb, ha novelem

lockFreeQueue<int> myQ2;
boost::lockfree::queue<int> bQ2(64);

void myProduce(int x)
{
    myQ.enq(x);
}

void myConsume(int* x)
{
    int y;
    myQ.deq(y);
    *x = y;
}

void bProduce(int x)
{
    bQ.bounded_push(x);
}

void bConsume(int* x)
{
    int y;
    bQ.pop(y);
    *x = y;
}
int main()
{
////////////////////////////////my LFQ//
    /*vector<chrono::steady_clock::time_point> startE;
    vector<chrono::steady_clock::time_point> endE;
    vector<chrono::steady_clock::time_point> startD;
    vector<chrono::steady_clock::time_point> endD;
    for(int t=0; t<numberOfTests; ++t)
    {
        startE.push_back(chrono::steady_clock::now());
    //Enque
        vector<thread> thE;

        for(int i=0; i<numberOfThreads; ++i)
        {
            thE.push_back( thread(&myProduce,i) );
        }

        for(int i=0; i<numberOfThreads; ++i)
        {
            thE[i].join();
        }
        endE.push_back(chrono::steady_clock::now());
    //Deque
        startD.push_back(chrono::steady_clock::now());
        vector<thread> thD;
        vector<int> res(numberOfThreads);

        for(int i=0; i<numberOfThreads; ++i)
        {
            try
            {
                thD.push_back( thread( &myConsume, &res[i] ) );
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
      /*  for(int i=0; i<numberOfThreads; ++i)
        {
            cout << res[i] << "  ";
        }
        cout << endl;*/
     /*   endD.push_back(chrono::steady_clock::now());
    }
////////////////////////////////Boost//
    vector<chrono::steady_clock::time_point> startE2;
    vector<chrono::steady_clock::time_point> endE2;
    vector<chrono::steady_clock::time_point> startD2;
    vector<chrono::steady_clock::time_point> endD2;
    for(int t=0; t<numberOfTests; ++t)
    {
        startE2.push_back(chrono::steady_clock::now());
    //Enque
        vector<thread> thE2;

        for(int i=0; i<numberOfThreads; ++i)
        {
            thE2.push_back( thread( &bProduce, i ) );
        }

        for(int i=0; i<numberOfThreads; ++i)
        {
            thE2[i].join();
        }
        endE2.push_back(chrono::steady_clock::now());
    //Deque
        startD2.push_back(chrono::steady_clock::now());
        vector<thread> thD2;
        vector<int> res2(numberOfThreads);

        for(int i=0; i<numberOfThreads; ++i)
        {
            try
            {
                thD2.push_back( thread( &bConsume, &res2[i] ) );
            }
            catch(exception& e)
            {
                cout << e.what();
            }
        }
        for(int i=0; i<numberOfThreads; ++i)
        {
            thD2[i].join();
        }
     /*   for(int i=0; i<numberOfThreads; ++i)
        {
            cout << res2[i] << "  ";
        }*/
    /*    endD2.push_back(chrono::steady_clock::now());
    }
////////////////////////////////Results//
    chrono::steady_clock::duration diffE, diffD, diffE2, diffD2;
    diffE = chrono::steady_clock::duration::zero();
    diffD = chrono::steady_clock::duration::zero();
    diffE2 = chrono::steady_clock::duration::zero();
    diffD2 = chrono::steady_clock::duration::zero();

    for(int i=0; i<numberOfTests; ++i)
    {
        diffE += endE[i] - startE[i];
        diffD += endD[i] - startD[i];
        diffE2 += endE2[i] - startE2[i];
        diffD2 += endD2[i] - startD2[i];
    }
    diffE /= numberOfTests;
    diffD /= numberOfTests;
    diffE2 /= numberOfTests;
    diffD2 /= numberOfTests;

    cout << "my LFQ:\nEnque: " << chrono::duration <double, milli> (diffE).count() << " ms\nDeque: " << chrono::duration <double, milli> (diffD).count() << " ms\nTotal: ";
    cout << chrono::duration <double, milli> (diffE + diffD).count();
    cout << "\n\nBoost's LFQ:\nEnque: " << chrono::duration <double, milli> (diffE2).count() << " ms\nDeque: " << chrono::duration <double, milli> (diffD2).count() << " ms\nTotal: ";
    cout << chrono::duration <double, milli> (diffE2 + diffD2).count();
*/
////////////////////////////////EQ+DQ//
    int sep;
    if(numberOfThreads < 101) sep = numberOfThreads / 10;
    else if(numberOfThreads < 10001) sep = numberOfThreads / 100;
    else sep = numberOfThreads / 1000;
    int change = -1;
    //Enque and deque together
    vector<chrono::steady_clock::time_point> start;
    vector<chrono::steady_clock::time_point> end;
    vector<int> res(numberOfThreads / 2);

    for(int t=0; t<numberOfTests; ++t)
    {
        start.push_back(chrono::steady_clock::now());

        vector<thread> th;
        int p = 0;
        int c = 0;

        for(int i=0; i<numberOfThreads; ++i)
        {
            if(i % sep == 0) change *= -1;
            if(change == 1)
            {
                th.push_back( thread(&myProduce,p++) );
             //   cout << p-1 << " ";
            }
            else
            {
                try
                {
                    th.push_back( thread(&myConsume,&res[c++]) );
                }
                catch(exception& e)
                {
                    cout << e.what();
                }
            }
        }

        for(int i=0; i<numberOfThreads; ++i)
        {
            th[i].join();
        }
        end.push_back(chrono::steady_clock::now());
    }

    /*cout<<endl;
    for(int i=0; i<numberOfThreads / 2; ++i)
    {
        cout << res[i] << " ";
    }
    cout << endl;*/
    ////////////////////////////////Boost//
    change = -1;
    vector<chrono::steady_clock::time_point> start2;
    vector<chrono::steady_clock::time_point> end2;
    vector<int> res2(numberOfThreads / 2);

    for(int t=0; t<numberOfTests; ++t)
    {
        start2.push_back(chrono::steady_clock::now());

        vector<thread> th2;
        vector<int> res2(numberOfThreads / 2);
        int p2 = 0;
        int c2 = 0;

        for(int i=0; i<numberOfThreads; ++i)
        {
            if(i % sep == 0) change *= -1;
            if(change == 1)
            {
                th2.push_back( thread(&bProduce,p2++) );
             //   cout << p2-1 << " ";
            }
            else
            {
                try
                {
                    th2.push_back( thread(&bConsume,&res2[c2++]) );
                }
                catch(exception& e)
                {
                    cout << e.what();
                }
            }
        }

        for(int i=0; i<numberOfThreads; ++i)
        {
            th2[i].join();
        }
        end2.push_back(chrono::steady_clock::now());
    }
  /*      cout<<endl;
        for(int i=0; i<numberOfThreads / 2; ++i)
        {
            cout << res2[i] << " ";
        }
        cout << endl;
    */

//////////////


////////////////////////////////Results//
    chrono::steady_clock::duration diff;
    chrono::steady_clock::duration diff2;
    diff = chrono::steady_clock::duration::zero();
    diff2 = chrono::steady_clock::duration::zero();

    for(int i=0; i<numberOfTests; ++i)
    {
        diff += end[i] - start[i];
        diff2 += end2[i] - start2[i];
    }
    diff /= numberOfTests;
    diff2 /= numberOfTests;

    cout << "my LFQ:\nTotal: " << chrono::duration <double, milli> (diff).count() << " ms";
    cout << "\n\nBoost's LFQ:\nTotal: " << chrono::duration <double, milli> (diff2).count() << " ms";

    return 0;
}















