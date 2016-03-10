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

#define SECOND 1000000

ThreadsVector activeThreads(MAX_THREAD_NUM);
ThreadsStates readyThreads;
ThreadsStates sleepingThreads;
ThreadsStates blockThreads;
int runningThread;

/* OUR METHODS */

void timer_handler(int sig) {
    for(int i = 0; i < 1000000; ++i) {
        std::cout << i << std::endl;
    }
    std::cout << "in handler. sig: " << sig << std::endl;
};

void printError(std::string error_text) {
    std::cerr << "thread library error: " << error_text << std::endl;
    exit(1);
};

void startTimer(int quantum_usecs) {
    struct sigaction sa;
    struct itimerval timer;
    
    // Install timer_handler as the signal handler for SIGVTALRM.
    sa.sa_handler = &timer_handler;
    if (sigaction(SIGVTALRM, &sa, NULL) < 0) {
        printError("sigaction error.");
    }
    
    // Calc timer times
    long usec = quantum_usecs % SECOND;
    long sec = quantum_usecs / SECOND;

    // Configure the timer to expire after quantum_usecs ... */
    timer.it_value.tv_sec = 1;		// first time interval, seconds part
    timer.it_value.tv_usec = 0;		// first time interval, microseconds part
    
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
    std::cout << activeThreads.addThread() << std::endl;
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
    int newThread = activeThreads.addThread();
    std::cout << "newThread:" << newThread << std::endl;
    readyThreads.addThread(newThread);
    return newThread;
};

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
    int oldThread = activeThreads.removeThread(tid);
    if(readyThreads.removeThread(tid)) {
        if(sleepingThreads.removeThread(tid)) {
            blockThreads.removeThread(tid);
        }
    }
    std::cout << "delete: " << oldThread << std::endl;
    return oldThread;
};

