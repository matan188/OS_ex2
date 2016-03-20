
CC = gcc
RANLIB = ranlib

LIBSRC = uthreads.cpp uthreads.h UThread.h UThread.cpp ThreadsVector.cpp ThreadsVector.h ThreadsStates.h ThreadsStates.cpp
LIBOBJ = $(LIBSRC:.cpp=.o)

INCS = -I.
CFLAGS = -Wall -std=c++11 -g $(INCS) 
LOADLIBES = -L./ 

UTHREADSLIB = libuthreads.a
TARGETS = $(UTHREADSLIB) tar

TAR = tar
TARFLAGS = -cvf
TARNAME = ex2.tar
TARSRCS = $(LIBSRC) Makefile README

all: $(TARGETS)

main: $(UTHREADSLIB) main.cpp
	g++ main.cpp $(UTHREADSLIB) -o main

uthreads.o: uthreads.cpp 
	$(CC) $(CFLAGS) -c uthreads.cpp -o uthreads.o

UThread.o: UThread.cpp 
	$(CC) $(CFLAGS) -c UThread.cpp -o UThread.o

ThreadsVector.o: ThreadsVector.cpp 
	$(CC) $(CFLAGS) -c ThreadsVector.cpp -o ThreadsVector.o

ThreadsStates.o: ThreadsStates.cpp 
	$(CC) $(CFLAGS) -c ThreadsStates.cpp -o ThreadsStates.o

$(UTHREADSLIB): uthreads.o UThread.o ThreadsVector.o ThreadsStates.o
	ar rcs $(UTHREADSLIB) uthreads.o UThread.o ThreadsVector.o ThreadsStates.o
	ranlib $(UTHREADSLIB)

clean:
	rm *.a *.o *.tar

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
	

