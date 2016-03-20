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
#include <iostream>
#include <vector>
#include "UThread.h"

class ThreadsStates {
private:
    std::vector<int> _threadsVec;
public:
    int pop();
    int removeThread(int tid);
    void addThread(int tid);
    void print();
    size_t size() { return _threadsVec.size(); };
    int at(int index) { return _threadsVec.at(index); };
};

#endif /* ThreadsStates_h */