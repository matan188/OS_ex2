
#ifndef ThreadsVector_h
#define ThreadsVector_h

#include <stdio.h>
#include <vector>

class ThreadsVector {
private:
    int maxThreads;
    int minFreeTid;
    std::vector<int> threadIds;
public:
    ThreadsVector(int maxThreads);
    int addThread();
    int removeThread(int tid);
};

#endif /* ThreadsVector_h */
