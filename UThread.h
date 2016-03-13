
#ifndef UThread_h
#define UThread_h

#include <setjmp.h>
#include <stdio.h>
#include "uthreads.h"
#include <signal.h>


class UThread {
public:
    UThread();
    UThread(int tid, void (*f)(void));
    int getTid() { return _tid; };
    void setTid();
    void setEnv(){ };
    sigjmp_buf * getEnvPtr(){ &_env; };

    
private:
    sigjmp_buf _env;
    char _stack[STACK_SIZE];
    int _tid;
    long unsigned _quantumCount;
};

#endif /* UThread_h */
