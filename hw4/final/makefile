SRC= hw4.cpp GameInstance.cpp Array2DCallbacks.cpp  # change these to your source file names
HEADERS= hw4.h GameInstance.h Array2DCallbacks.h # change these to your header file names
LIBS=libhw2.a
CFLAGS = -Wall -Wpedantic -std=gnu++0x 
IFLAGS =  -I. -I./jansson/include
GTKCFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS =  -L. -L./jansson/lib -lhw2 -l:libjansson.a
GTKLDFLAGS = `pkg-config --libs gtk+-3.0`

all: hw4

hw4: $(SRC) $(HEADERS) $(LIBS)
	g++ $(CFLAGS) -g -O0 $(IFLAGS) $(GTKCFLAGS) -o hw4 $(SRC) $(LDFLAGS) $(GTKLDFLAGS)


clean:
	rm -f *.o *~ hw4
