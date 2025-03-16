LIB = ../../lib
INCLUDE = ../../include

CC = clang
CFLAGS = -c -O3 -g -Wall -I$(INCLUDE) -I. -I/AFLplusplus/include

vector.a: vector.c
		$(CC) $(CFLAGS) $^ -o$@

test_vector.o: test_vector.c 
		$(CC) $(CFLAGS) $^ -o$@

test_vector: vector.a test_vector.o 
		$(CC) $^ -o$@


install: vector.a 
		cp vector.a $(LIB)
		cp vector.h $(INCLUDE)
		make clean

all: vector.a test_vector install

clean:
		rm *.o *.a test_vector

