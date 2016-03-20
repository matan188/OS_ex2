
#include "ThreadsVector.h"


/**
 * Error printer
 */
static void printError(std::string error_text) {
    std::cerr << "thread library error: " << error_text << std::endl;
};

/**
 * Error printer
 */
static void printSysError(std::string error_text) {
    std::cerr << "system error: " << error_text << std::endl;
    exit(1);
};

ThreadsVector::ThreadsVector(int maxThreads): maxThreads(maxThreads), minFreeTid(0) {};


UThread * ThreadsVector::at(int tid) {
    if(tid < 0 || (size_t) tid >= _threads.size()) {
        return nullptr;
    }
    return _threads.at(tid);
}


int ThreadsVector::addThread(void (*f)(void)) {
    int ret = minFreeTid;

    if(minFreeTid >= maxThreads) {
        printError("Exceeds number of Max Threads");
        return -1;
    } else {
        UThread * t;
        try
        {
            t = new UThread(minFreeTid, f);
        } catch(std::bad_alloc &b){
            printSysError("Bad Allocation error");
        }

        if(minFreeTid >= (int) _threads.size()) {
            _threads.push_back(t);
            ++minFreeTid;
        } else {
            _threads.at(minFreeTid) = t;
            //minFreeTid = (int)_threads.size();
            long unsigned i;
            for(i = minFreeTid; i < _threads.size(); ++i) {
                if(_threads.at(i) == nullptr) {
                    minFreeTid = i;
                    break;
                }
            }
            minFreeTid = i;
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