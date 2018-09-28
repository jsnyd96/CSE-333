# define the commands we will use for compilation
CC = gcc
CXX = g++

# where the socket source code lives
SOCKDIR = ./socketCode

# hw4 code
HW4 = ./hw4

# define useful flags to cc/ld/etc.
LIBS=libhw2.a
CFLAGS = -g -Wall -Wpedantic -O0 -I$(SOCKDIR)
LDFLAGS = -L. -L./jansson/lib -lhw2 -l:libjansson.a -lpthread
IFLAGS =  -I. -I./socketCode/ -I./jansson/include

# define common dependencies
HEADERS = $(SOCKDIR)/CSE333Socket.h $(HW4)/GameInstance.h Move.h
OBJS = $(SOCKDIR)/CSE333Socket.o $(SOCKDIR)/Verify333.o $(SOCKDIR)/ClientSocket.o $(HW4)/GameInstance.o $(HW4)/Array2DCallbacks.o Move.o

all: CandyClient

CandyClient: CandyClient.o $(OBJS)
	cd $(SOCKDIR) && make
	cd $(HW4) && make
	$(CXX) $(CFLAGS) -o CandyClient CandyClient.o $(OBJS) $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) -std=c++11 $(IFLAGS) $(CFLAGS) -c $<

%.o: %.cc $(HEADERS)
	$(CXX) -std=c++11 $(IFLAGS) $(CFLAGS) -c $<

%.o: %.c $(HEADERS)
	$(CC) -std=c11 $(CFLAGS) -c -std=c11 $<

clean: FORCE
	cd $(SOCKDIR) && make clean
	cd $(HW4) && make clean
	/bin/rm -f *.o *~ CandyServer CandyClient

FORCE:
