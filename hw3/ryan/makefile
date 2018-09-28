all: exampleWindow

exampleWindow: exampleWindow.c
	gcc `pkg-config --cflags gtk+-3.0` -o exampleWindow exampleWindow.c `pkg-config --libs gtk+-3.0`

clean:
	rm exampleWindow
