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
#include <sys/time.h>
#include <queue>
#include <chrono>

#define SECOND 1000000

ThreadsVector activeThreads(MAX_THREAD_NUM);
ThreadsStates readyThreads;
ThreadsStates sleepingThreads;
std::queue<int> toDel;

int totalQuantum = 1;
int runningThread;
struct itimerval timer;

//TODO delete useless comments
//TODO add makro and consts
//TODO run valgrind? not mandatory
//TODO delete to print from state and threads vectors
//TODO decide upon pending method
//int isPending = 0;

/*** OUR METHODS ***/

/**
 * Handle Main termination
 */
void terminateMain(int tid) {
    if(tid == 0) {
        for(int i = 1; i < (int) activeThreads.size(); ++i) {
            activeThreads.removeThread(i);
        }
        exit(0);
    }
}

static void printError(std::string error_text) {
    std::cerr << "thread library error: " << error_text << std::endl;
};

static void printSysError(std::string error_text) {
    std::cerr << "system error: " << error_text << std::endl;
    exit(1);
};

/**
 * Reset timer
 */
void resetTimer() {
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL)) {
        printError("setitimer error.");
    }
}


/**
 * Manages self termination\deletion of threads.
 * After switching thread to the next one, we check if previous
 * thread tried to delete itself and delete it in this thread's run.
 */
void manageDeletion() {
    if(!toDel.empty()) {
        if(toDel.front() != runningThread) {
            activeThreads.removeThread(toDel.front());
            readyThreads.removeThread(toDel.front());
            toDel.pop();
        }
    }
}

/**
 * Block SIGVTALARM
 */
void blockSigalarm() {
    sigset_t mSet;
    sigemptyset(&mSet);
    sigaddset(&mSet, SIGVTALRM);
    sigprocmask(SIG_SETMASK, &mSet, NULL);
}

/**
 * Unblock SIGVTALARM
 */
void unblockSigalarm() {
    sigset_t mSet;
    sigemptyset(&mSet);
    sigaddset(&mSet, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &mSet, NULL);
}


/**
 * Clear SIGVTALARM from pending
 */
void clearPending() {
    sigset_t mSet;
    sigpending(&mSet);
    int isMember = sigismember(&mSet, SIGVTALRM);
    int x;
    if(isMember) {
        sigwait(&mSet, &x);
    }
}

/**
 * Check if given tid exists and valid
 */
int tidVal(int tid) {
    if( (size_t) tid >= activeThreads.size() || tid < 0) {
        return -1;
    }
    else if(activeThreads.at(tid) == nullptr) {
        return -1;
    }
    return 0;
}

/**
 * Update sleeping threads quantum till wake-up
 */
void updateSleepingThreads() {
    for(size_t i = 0; i < sleepingThreads.size(); ++i) {
        int tid = sleepingThreads.at((int) i);
        UThread * thread = activeThreads.at(tid);
        int timeUntillWakeup = thread->getQuantumsUntilWakeup();
        if(timeUntillWakeup == 0) {
            thread->setState(ready);
            sleepingThreads.removeThread(tid);
            readyThreads.addThread(tid);
        }
        --timeUntillWakeup;
        thread->setQuantumsUntilWakeup(timeUntillWakeup);
    }
}

/**
 * Switch Running with the next ready
 */
void switchRunning() {
    if(activeThreads.at(runningThread)->getState() != blocked) {
        if(activeThreads.at(runningThread)->getState() != sleeping) {
            readyThreads.addThread(runningThread);
            activeThreads.at(runningThread)->setState(ready);
        }
    }

    runningThread = readyThreads.pop();
    activeThreads.at(runningThread)->setState(running);
}

/**** Exercise's functions ***/

/**
 * Switching threads
 */
void switchThreads(int sig) {
    int retVal = sigsetjmp(*(activeThreads.at(runningThread)->getEnvPtr()), 1);

    if (retVal == 1) {
        return;
    }

    /* Switch running with next ready */
    switchRunning();

    /* Handle sleeps */
    updateSleepingThreads();

    activeThreads.at(runningThread)->updateQuantumCount();
    ++totalQuantum;

    //TODO check if needed
    if(sig == -1) {
        // when switch called manually
        resetTimer();
        /*if (setitimer(ITIMER_VIRTUAL, &timer, NULL)) {
            printError("setitimer error.");
        }*/
    }

    clearPending();

    /*sigset_t mSet;
    sigpending(&mSet);
    int isMember = sigismember(&mSet, SIGVTALRM);
    if(isMember) {
        isPending = 1;
    }*/

    siglongjmp(*(activeThreads.at(runningThread)->getEnvPtr()), 1);
}

/**
 * Handle SIGALRM signals
 */
void timer_handler(int sig) {
    if(true /*isPending == 0*/){

        /* If self-erase */
        manageDeletion();

        switchThreads(sig);
        resetTimer();

    } else{
        //isPending = 0;
    }

    clearPending();
    /*
    sigset_t mSet;
    sigpending(&mSet);
    int isMember = sigismember(&mSet, SIGVTALRM);
    if(isMember) {
        isPending = 1;
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
    resetTimer();
};

/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * the length of a quantum in micro-seconds. It is an error to call this
 * function with non-positive quantum_usecs.
 * Return value: On success, return 0. On failure, return -1.
 */
int uthread_init(int quantum_usecs) {
    if(quantum_usecs <= 0) {
        printError("Quantum must be positive");
        return -1;
    }

    activeThreads.addMain();
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
    if(tid == 0) {
        printError("Blocking main thread is illegal");
        return -1;
    }
    if(tidVal(tid)) {
        printError("Invalid tid");
        return -1;
    }

    blockSigalarm();

    UThread * thread = activeThreads.at(tid);
    if(thread == nullptr) {
        return -1;
    }
    state currentState = thread->getState();

    if(currentState == ready) {
        readyThreads.removeThread(tid);
        thread->setState(blocked);
    } else if(currentState == running) {
        thread->setState(blocked);
        switchThreads(-1);
    }

    unblockSigalarm();
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
    if(tidVal(tid)) {
        printError("Invalid tid");
        return -1;
    }

    blockSigalarm();

    terminateMain(tid);


    int oldThread = 0;
    if(activeThreads.at(tid)->getState() == running) {
        toDel.push(tid);
        raise(SIGVTALRM);
    } else {
        oldThread = activeThreads.removeThread(tid);
        if(readyThreads.removeThread(tid)) {
            sleepingThreads.removeThread(tid);
        }
    }

    /* UNBLOCK SIGVTALRM */
    unblockSigalarm();
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
    if(tidVal(tid)) {
        printError("Invalid tid");
        return -1;
    }

    blockSigalarm();

    UThread * thread = activeThreads.at(tid);

    if(thread->getState() == blocked) {
        thread->setState(ready);
        readyThreads.addThread(tid);
    }

    unblockSigalarm();
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

    if(num_quantums <= 0) {
        printError("Quantum must be positive");
        return -1;
    }

    blockSigalarm();
    
    UThread * thread = activeThreads.at(runningThread);

    thread->setState(sleeping);
    thread->setQuantumsUntilWakeup(num_quantums);
    sleepingThreads.addThread(runningThread);
    raise(SIGVTALRM);

    unblockSigalarm();
    return 0;
}


/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
 */
int uthread_get_tid() {
    return runningThread;
}

/*
 * Description: This function returns the total number of quantums that were
 * started since the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
 */
int uthread_get_total_quantums() {
    return totalQuantum;
}


/*
 * Description: This function returns the number of quantums the thread with
 * ID tid was in RUNNING state. On the first time a thread runs, the function
 * should return 1. Every additional quantum that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantum). If no
 * thread with ID tid exists it is considered as an error.
 * Return value: On success, return the number of quantums of the thread with
 * ID tid. On failure, return -1.
 */
int uthread_get_quantums(int tid) {
    if(tidVal(tid)) {
        printError("Invalid tid");
        return -1;
    }
    return (int) activeThreads.at(tid)->getQuantumsCount();
}

