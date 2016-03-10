
#ifndef UThread_h
#define UThread_h

#include <stdio.h>

class UThread {
public:
    int getTid() { return tid; };
private:
    int tid;
}

#endif /* UThread_h */
