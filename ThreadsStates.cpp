//
//  ThreadsStates.cpp
//  os_ex2
//
//  Created by Eyal Cohen on 10/03/2016.
//  Copyright © 2016 Eyal Cohen. All rights reserved.
//

#include "ThreadsStates.h"


/**
 * Pop
 */
int ThreadsStates::pop() {
    if(_threadsVec.size() == 0) {
        return -1;
    }
    int ret = _threadsVec.front();
    _threadsVec.erase(_threadsVec.begin());
    return ret;
};


/**
 * Remove thread
 */
int ThreadsStates::removeThread(int tid) {
    for(long unsigned i = 0; i < _threadsVec.size(); ++i) {
        if(_threadsVec.at(i) == tid) {
            _threadsVec.erase(_threadsVec.begin() + i);
            return 0;
        }
    }
    return -1;
};


/**
 * Add thread
 */
void ThreadsStates::addThread(int tid) {
    _threadsVec.push_back(tid);
};

void ThreadsStates::print() {
    for(int i = 0; i < _threadsVec.size(); ++i) {
        std::cout << _threadsVec.at(i) << ", ";
    }
    std::cout << std::endl;
}