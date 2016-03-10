#include "uthreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

int main(void)
{
    std::cout << "calling init: " << std::endl;
    uthread_init(1000000);
    
    uthread_spawn([](){});
    uthread_spawn([](){});
    uthread_spawn([](){});
    uthread_terminate(2);
    uthread_spawn([](){});
    
    for(;;){};
    return 0;
}