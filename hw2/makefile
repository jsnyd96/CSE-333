#replace these definitions with lists of your source and header files
SRC= example_program_array2d.c Array2D.c
HEADER=Array2D.h Array2D_priv.h

hw2:  ${SRC} ${HEADER}
	gcc -std=c11 -Wall -g -Wno-pointer-to-int-cast -I ./jansson/include ${SRC} -L ./jansson/lib -l:libjansson.a -o hw2

run: hw2
	valgrind --leak-check=full ./hw2 test.json

clean:
	rm -f *.o *~ hw2 json.out


