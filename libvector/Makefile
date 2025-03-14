all: vector.o test_vector

clean:
		rm *.o test_vector

CC = gcc
CFLAGS = -c -O0 -g -Wall

vector.o: vector.c
		$(CC) $(CFLAGS) $^ -o$@

test_vector.o: test_vector.c 
		$(CC) $(CFLAGS) $^ -o$@

test_vector: vector.o test_vector.o 
		$(CC) test_vector.o vector.o -o$@
		
