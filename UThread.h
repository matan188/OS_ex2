
#ifndef UThread_h
#define UThread_h

#include <setjmp.h>
#include <stdio.h>
#include "uthreads.h"
#include <signal.h>

enum state {ready, sleeping, running, blocked};

class UThread {
public:
    UThread();
    UThread(int tid, void (*f)(void));
    int getTid() { return _tid; };
    int getQuantumsUntilWakeup() { return _quantumsUntilWakeup; };
    void setQuantumsUntilWakeup(int sleepingTime) { _quantumsUntilWakeup = sleepingTime; };
    void setTid();
    long unsigned getQuantumsCount() { return _quantumsCount; };
    void updateQuantumCount() { _quantumsCount++; };
    sigjmp_buf * getEnvPtr(){ return &_env; };
    void setState(state s) { _state = s; };
    state getState() { return _state; };
    
private:
    sigjmp_buf _env;
    char _stack[STACK_SIZE];
    int _tid;
    state _state;
    int _quantumsUntilWakeup;
    long unsigned _quantumsCount;
};

#endif /* UThread_h */
