
#include "ThreadsVector.h"

ThreadsVector::ThreadsVector(int maxThreads): maxThreads(maxThreads), minFreeTid(0) {};

int ThreadsVector::addThread() {
    int ret = minFreeTid;
    if(minFreeTid >= maxThreads) {
        return -1;
    } else {
        if(minFreeTid >= threadIds.size()) {
            threadIds.push_back(minFreeTid);
            ++minFreeTid;
        } else {
            threadIds.at(minFreeTid) = minFreeTid;
            minFreeTid = (int) threadIds.size();
            for(int i = minFreeTid; i < threadIds.size(); ++i) {
                if(threadIds.at(i) == -1) {
                    minFreeTid = i;
                    break;
                }
            }
            
        }
    }
    return ret;
};

int ThreadsVector::removeThread(int tid) {
    int vecSize = (int) threadIds.size();
    if(tid >= 0 && tid < vecSize) {
        // tid in vector boundaries
        if(threadIds.at(tid) != -1) {
            // Thread exists
            if(tid == vecSize - 1) {
                threadIds.pop_back();
            } else {
                threadIds.at(tid) = -1;
            }
            if(tid < minFreeTid) {
                minFreeTid = tid;
            }
            return 0;
        }
    }
    return -1;
};