
#include "ThreadsVector.h"

ThreadsVector::ThreadsVector(int maxThreads): maxThreads(maxThreads), minFreeTid(0) {};


UThread * ThreadsVector::at(int tid) {
    return _threads.at(tid);
}


int ThreadsVector::addThread(void (*f)(void)) {
    int ret = minFreeTid;

    if(minFreeTid >= maxThreads) {
        return -1;
    } else {
        if(minFreeTid >= (int) _threads.size()) {
            _threads.push_back(new UThread(minFreeTid, f));
            ++minFreeTid;
        } else {
            _threads.at(minFreeTid) = new UThread(minFreeTid, f);
            minFreeTid = (int)_threads.size();
            for(long unsigned i = minFreeTid; i < _threads.size(); ++i) {
                if(_threads.at(i) == nullptr) {
                    minFreeTid = i;
                    break;
                }
            }
        }
    }
    return ret;
};


int ThreadsVector::addMain() {
    if(_threads.size() == 0) {
        _threads.push_back(new UThread());
        minFreeTid = 1;
        return 0;
    }
    return -1;
}

int ThreadsVector::removeThread(int tid) {
    int vecSize = (int) _threads.size();
    if(tid >= 0 && tid < vecSize) {
        // tid in vector boundaries
        if(_threads.at(tid) != nullptr) {
            // Thread exists
            delete _threads.at(tid);
            if(tid == vecSize - 1) {
                 _threads.pop_back();
            } else {
                 _threads.at(tid) = nullptr;
            }
            if(tid < minFreeTid) {
                minFreeTid = tid;
            }
            return 0;
        }
    }
    return -1;
};