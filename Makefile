CC = g++
CFLAGS = -std=c++11 -Wextra

main: main.o uthreads.o UThread.o ThreadsVector.o ThreadsStates.o
	$(CC) $(CFLAGS) main.o uthreads.o UThread.o ThreadsVector.o ThreadsStates.o -o main

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o 

uthreads.o: uthreads.cpp 
	$(CC) $(CFLAGS) -c uthreads.cpp -o uthreads.o

UThread.o: UThread.cpp 
	$(CC) $(CFLAGS) -c UThread.cpp -o UThread.o

ThreadsVector.o: ThreadsVector.cpp 
	$(CC) $(CFLAGS) -c ThreadsVector.cpp -o ThreadsVector.o

ThreadsStates.o: ThreadsStates.cpp 
	$(CC) $(CFLAGS) -c ThreadsStates.cpp -o ThreadsStates.o


all:
	make UThread.o
	make ThreadsStates.o
	make ThreadsVector.o
	make uthreads.o
	make main.o

clean:
	*.o 