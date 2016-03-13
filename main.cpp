#include "uthreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>


void f() {
    while(true){
       // std::cout << "f" << std::endl;
       // usleep(10000);
    }
}

void g() {
    while(true){
       // std::cout << "g" << std::endl;
       // usleep(10000);
    }
}

int main(void)
{
    std::cout << "calling init: " << std::endl;
    uthread_init(1000000);

    uthread_spawn(&f);
    uthread_spawn(&g);



    for(;;){
        //std::cout << "main" << std::endl;
        //usleep(10000);
    };


    return 0;

}