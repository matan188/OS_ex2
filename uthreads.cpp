//
//  uthreads.cpp
//  os_ex2
//
//  Created by Eyal Cohen on 10/03/2016.
//  Copyright © 2016 Eyal Cohen. All rights reserved.
//

#include "uthreads.h"
#include "UThread.h"
#include "ThreadsVector.h"
#include "ThreadsStates.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <signal.h>
#include <sys/time.h>
#include <vector>

#include <chrono>
#include <ctime>

#define SECOND 1000000

ThreadsVector activeThreads(MAX_THREAD_NUM);
ThreadsStates readyThreads;
ThreadsStates sleepingThreads;
int toDelete = 0;

int runningThread;
struct itimerval timer;

/* OUR METHODS */

/**
 * Error printer
 */
void printError(std::string error_text) {
    std::cerr << "thread library error: " << error_text << std::endl;
    exit(1);
};

/**
 * Switching threads
 */
void switchThreads(int sig) {
    int retVal = sigsetjmp(*(activeThreads.at(runningThread)->getEnvPtr()), 1);

    if (retVal == 1) {
        std::cout << "retval = 1" << std::endl;
        return;
    }
    if(activeThreads.at(runningThread)->getState() != blocked) {
        if(activeThreads.at(runningThread)->getState() != sleeping) {
            readyThreads.addThread(runningThread);
            activeThreads.at(runningThread)->setState(ready);
        }
    }
    runningThread = readyThreads.pop();
    activeThreads.at(runningThread)->setState(running);

    if(sig == -1) {
        // when switch called manually
        if (setitimer(ITIMER_VIRTUAL, &timer, NULL)) {
            printError("setitimer error.");
        }
        
        sigset_t mSet;
        sigpending(&mSet);
        int isMember = sigismember(&mSet, SIGVTALRM);
        int * signal;
        if(isMember) {
            /*
            std::cout << "^^^^^^^^^unblock3" << std::endl;
            sigprocmask(SIG_UNBLOCK, &mSet, NULL);
            sigwait(&mSet, signal);
            
            
            
            sigset_t mSet2;
            sigpending(&mSet2);
            int isMember2 = sigismember(&mSet2, SIGVTALRM);
            if(isMember2) {
                std::cout << "ITS STILL A MEMBER!!" << std::endl;
            }
            /*
            std::cout << "ITS A MEMBER!!" << std::endl;
            sigdelset(&mSet, SIGVTALRM);
            isMember = sigismember(&mSet, SIGVTALRM);
            if(isMember) {
                std::cout << "ITS STILL A MEMBER!!" << std::endl;
            }
            */
        }
        
    }
    
    siglongjmp(*(activeThreads.at(runningThread)->getEnvPtr()), 1);
}

/**
 * Handle SIGALRM signals
 */
void timer_handler(int sig) {

    for(size_t i = 0; i < sleepingThreads.size(); ++i) {
        int tid = sleepingThreads.at(i);
        UThread * thread = activeThreads.at(tid);
        int timeUntillWakeup = thread->getQuantumsUntilWakeup();
        std::cout << "Thread: " << tid << " timeUntillWakeup: " << timeUntillWakeup << std::endl;
        --timeUntillWakeup;
        thread->setQuantumsUntilWakeup(timeUntillWakeup);
        if(timeUntillWakeup == 0) {
            thread->setState(ready);
            sleepingThreads.removeThread(tid);
            readyThreads.addThread(tid);
            std::cout << "currRunning " << runningThread << std::endl;
        }
    }
    
    switchThreads(sig);
    if(toDelete) {
        activeThreads.removeThread(toDelete);
        readyThreads.removeThread(toDelete);
        toDelete = 0;
    }
    activeThreads.at(runningThread)->updateQuantumCount();
    
    //std::cout << "threadId: " << runningThread << " Q count: " << activeThreads.at(runningThread)->getQuantumsCount() << std::endl;

    if (setitimer(ITIMER_VIRTUAL, &timer, NULL)) {
        printError("setitimer error.");
    }

    /*
    // timer_handler called manually
    if(sig == -1) {
        sigset_t mSet;
        sigpending(&mSet);
        int isMember = sigismember(&mSet, SIGVTALRM);
        if(isMember) {
            std::cout << "ITS A MEMBER!!" << std::endl;
        }
    }
    */
};

/**
 * Init Timer
 */
void startTimer(int quantum_usecs) {
    struct sigaction sa;
    /* Avoid VIRTUAL TIME ERROR */
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    // Install timer_handler as the signal handler for SIGVTALRM.
    sa.sa_handler = &timer_handler;
    if (sigaction(SIGVTALRM, &sa, NULL) < 0) {
        printError("sigaction error.");
    }
    
    // Calc timer times
    long usec = quantum_usecs % SECOND;
    long sec = quantum_usecs / SECOND;

    // Configure the timer to expire after quantum_usecs ... */
    timer.it_value.tv_sec = sec;		// first time interval, seconds part
    timer.it_value.tv_usec = usec;		// first time interval, microseconds part
    
    // configure the timer to expire every quantum_usecs after that.
    timer.it_interval.tv_sec = sec;	// following time intervals, seconds part
    timer.it_interval.tv_usec = usec;	// following time intervals, microseconds part

    // Start a virtual timer. It counts down whenever this process is executing.
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL)) {
        printError("setitimer error.");
    }
};




/* END OUR METHODS */

/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * the length of a quantum in micro-seconds. It is an error to call this
 * function with non-positive quantum_usecs.
 * Return value: On success, return 0. On failure, return -1.
 */
int uthread_init(int quantum_usecs) {
    std::cout << "add main " << activeThreads.addMain() << std::endl;
    runningThread = 0;

    startTimer(quantum_usecs);
    return 0;
};

/*
 * Description: This function creates a new thread, whose entry point is the
 * function f with the signature void f(void). The thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * Return value: On success, return the ID of the created thread.
 * On failure, return -1.
 */
int uthread_spawn(void (*f)(void)) {
    int newThread = activeThreads.addThread(*f);
    std::cout << "newThread:" << newThread << std::endl;
    readyThreads.addThread(newThread);
    return newThread;
};


/*
 * Description: This function blocks the thread with ID tid. The thread may
 * be resumed later using uthread_resume. If no thread with ID tid exists it
 * is considered as an error. In addition, it is an error to try blocking the
 * main thread (tid == 0). If a thread blocks itself, a scheduling decision
 * should be made. Blocking a thread in BLOCKED or SLEEPING states has no
 * effect and is not considered as an error.
 * Return value: On success, return 0. On failure, return -1.
 */
int uthread_block(int tid) {
    
    sigset_t mSet;
    sigemptyset(&mSet);
    sigaddset(&mSet, SIGVTALRM);
    sigprocmask(SIG_SETMASK, &mSet, NULL);

    UThread * thread = activeThreads.at(tid);
    if(thread == nullptr) {
        return -1;
    }
    state currentState = thread->getState();
    switch (currentState) {
        case ready:
            std::cout << "** READY **" << std::endl;
            readyThreads.removeThread(tid);
            thread->setState(blocked);
            break;
        case running:
            std::cout << "** RUNNING **" << std::endl;
            thread->setState(blocked);
            /*
            for(int i = 0; i < 10000000; i++) {
                for(int j = 0; j < 100; j++) {
                }
            }
            */
            
            switchThreads(-1);
            break;
    }
    
    std::cout << "^^^^^^^^^unblock" << std::endl;
    sigprocmask(SIG_UNBLOCK, &mSet, NULL);
    
    return 0;
}



/*
 * Description: This function terminates the thread with ID tid and deletes
 * it from all relevant control structures. All the resources allocated by
 * the library for this thread should be released. If no thread with ID tid
 * exists it is considered as an error. Terminating the main thread
 * (tid == 0) will result in the termination of the entire process using
 * exit(0) [after releasing the assigned library memory].
 * Return value: The function returns 0 if the thread was successfully
 * terminated and -1 otherwise. If a thread terminates itself or the main
 * thread is terminated, the function does not return.
 */
int uthread_terminate(int tid) {
    sigset_t mSet;
    sigemptyset(&mSet);
    sigaddset(&mSet, SIGVTALRM);
    sigprocmask(SIG_SETMASK, &mSet, NULL);

    if(tid == 0) {
        for(int i = 1; i < activeThreads.size(); ++i) {
            activeThreads.removeThread(i);
        }
        exit(0);
    }

    int oldThread = 0;
    if(activeThreads.at(tid)->getState() == running) {
        toDelete = tid;
        raise(SIGVTALRM);
    } else {
        oldThread = activeThreads.removeThread(tid);
        if(readyThreads.removeThread(tid)) {
            sleepingThreads.removeThread(tid);
        }
    }


    /* UNBLOCK SIGVTALRM */
    sigprocmask(SIG_UNBLOCK, &mSet, NULL);

    std::cout << "delete: " << oldThread << std::endl;
    return oldThread;
};

/*
 * Description: This function resumes a blocked thread with ID tid and moves
 * it to the READY state. Resuming a thread in the RUNNING, READY or SLEEPING
 * state has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered as an error.
 * Return value: On success, return 0. On failure, return -1.
 */
int uthread_resume(int tid) {
    sigset_t mSet;
    sigemptyset(&mSet);
    sigaddset(&mSet, SIGVTALRM);
    sigprocmask(SIG_SETMASK, &mSet, NULL);

    UThread * thread = activeThreads.at(tid);
    if(thread == nullptr) {
        return -1;
    }
    thread->setState(ready);
    readyThreads.addThread(tid);

    sigprocmask(SIG_UNBLOCK, &mSet, NULL);
    return 0;
}

/*
 * Description: This function puts the RUNNING thread to sleep for a period
 * of num_quantums (not including the current quantum) after which it is moved
 * to the READY state. num_quantums must be a positive number. It is an error
 * to try to put the main thread (tid==0) to sleep. Immediately after a thread
 * transitions to the SLEEPING state a scheduling decision should be made.
 * Return value: On success, return 0. On failure, return -1.
 */
int uthread_sleep(int num_quantums) {
    sigset_t mSet;
    sigemptyset(&mSet);
    sigaddset(&mSet, SIGVTALRM);
    sigprocmask(SIG_SETMASK, &mSet, NULL);
    
    UThread * thread = activeThreads.at(runningThread);

    thread->setState(sleeping);
    thread->setQuantumsUntilWakeup(num_quantums);
    sleepingThreads.addThread(runningThread);
    raise(SIGVTALRM);

    sigprocmask(SIG_UNBLOCK, &mSet, NULL);
    return 0;
}