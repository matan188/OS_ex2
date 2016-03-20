
#ifndef ThreadsVector_h
#define ThreadsVector_h

#include <stdio.h>
#include <vector>
#include "UThread.h"
#include <iostream>

class ThreadsVector {
private:
    int maxThreads;
    int minFreeTid;
    std::vector<UThread *> _threads;

public:
    ThreadsVector(int maxThreads);
    int addThread(void (*f)(void));
    int addMain();
    size_t size() { return _threads.size(); };
    int removeThread(int tid);
    UThread* at(int tid);
};

#endif /* ThreadsVector_h */
