//
//  ThreadsStates.hpp
//  os_ex2
//
//  Created by Eyal Cohen on 10/03/2016.
//  Copyright © 2016 Eyal Cohen. All rights reserved.
//

#ifndef ThreadsStates_h
#define ThreadsStates_h

#include <stdio.h>
#include <vector>
#include "UThread.h"

class ThreadsStates {
private:
    std::vector<int> threadsVec;
public:
    int pop();
    int removeThread(int tid);
    void addThread(int tid);
};

#endif /* ThreadsStates_h */