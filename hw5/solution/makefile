# define the commands we will use for compilation
CC = gcc
CXX = g++

# where the socket source code lives
SOCKDIR = ../socketCode

# hw4 code
HW4 = ./hw4

# define useful flags to cc/ld/etc.
LIBS=libhw2.a
CFLAGS = -g -Wall -Wpedantic -O0 -I$(SOCKDIR)
LDFLAGS = -L. -L./jansson/lib -lhw2 -l:libjansson.a
IFLAGS =  -I. -I../socketCode/ -I./jansson/include -I./hw4
GTKCFLAGS = `pkg-config --cflags gtk+-3.0`
GTKLDFLAGS = `pkg-config --libs gtk+-3.0`

# define common dependencies
HEADERS = $(SOCKDIR)/CSE333Socket.h $(HW4)/GameInstance.h
OBJS = $(SOCKDIR)/CSE333Socket.o $(SOCKDIR)/Verify333.o $(SOCKDIR)/ClientSocket.o $(HW4)/GameInstance.o $(HW4)/Array2DCallbacks.o

all: CandyClient CandyServer

CandyServer:  CandyServer.o $(OBJS) $(SOCKDIR)/ServerSocket.o
	cd ./hw4 && make
	cd $(SOCKDIR) && make
	$(CXX) $(CFLAGS) -o CandyServer $(SOCKDIR)/ServerSocket.o CandyServer.o $(OBJS) $(LDFLAGS) $(GTKLDFLAGS)

CandyClient: CandyClient.o $(OBJS)
	cd ./hw4 && make
	cd $(SOCKDIR) && make
	$(CXX) $(CFLAGS) -o CandyClient CandyClient.o $(OBJS) $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) -std=c++11 $(IFLAGS) $(GTKCFLAGS) $(CFLAGS) -c $<

%.o: %.cc $(HEADERS)
	$(CXX) -std=c++11 $(IFLAGS) $(GTKCFLAGS) $(CFLAGS) -c $<

%.o: %.c $(HEADERS)
	$(CC) -std=c11 $(CFLAGS) -c -std=c11 $<

clean: FORCE
	cd $(SOCKDIR) && make clean
	cd ./hw4 && make clean
	/bin/rm -f *.o *~ CandyServer CandyClient

FORCE:
