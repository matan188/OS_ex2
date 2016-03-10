//
//  ThreadsStates.cpp
//  os_ex2
//
//  Created by Eyal Cohen on 10/03/2016.
//  Copyright © 2016 Eyal Cohen. All rights reserved.
//

#include "ThreadsStates.h"

int ThreadsStates::pop() {
    if(threadsVec.size() == 0) {
        return -1;
    }
    int ret = threadsVec.front();
    threadsVec.erase(threadsVec.begin());
    return ret;
};

int ThreadsStates::removeThread(int tid) {
    for(int i = 0; i < threadsVec.size(); ++i) {
        if(threadsVec.at(i) == tid) {
            threadsVec.erase(threadsVec.begin() + i);
            return 0;
        }
    }
    return -1;
};

void ThreadsStates::addThread(int tid) {
    threadsVec.push_back(tid);
};