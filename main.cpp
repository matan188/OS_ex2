#include "uthreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>


void f() {

    long i = 0;
    while(true){
        if(i%100000000 == 0) {
            std::cout << "f" << std::endl;

        }
        ++i;
    }
}

void g() {
    //uthread_block(2);
    uthread_sleep(5);
    long i=0;
    while(true){
        if(i%100000000 == 0) {
            //uthread_terminate(0);
            std::cout << "g" << std::endl;

        }
        ++i;
    }
}

int main(void)
{
    std::cout << "calling init: " << std::endl;
    uthread_init(1000000);

    uthread_spawn(&f);
    uthread_spawn(&g);


    long i=0;
    for(;;){
        if(i%100000000== 0) {
            std::cout << "main" << std::endl;
            std::cout << "total quantum " << uthread_get_total_quantums() << std::endl;
            if(uthread_get_total_quantums() >= 6){
                uthread_resume(2);
            }
        }
        ++i;
    };


    return 0;

}