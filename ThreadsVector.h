
#ifndef ThreadsVector_h
#define ThreadsVector_h

#include <stdio.h>
#include <vector>
#include "UThread.h"
#include <iostream>

/**
 * Wrapper for managing threads.
 * Contains pointers to threads and functions to manage them
 * properly.
 */
class ThreadsVector {
private:
    int _maxThreads;
    int _minFreeTid;
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