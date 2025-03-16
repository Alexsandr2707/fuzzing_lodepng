LIB = ../../lib
INCLUDE = ../../include

all: afl_vector.a test_vector install

clean:
		rm *.o *.a test_vector

CC = clang
CFLAGS = -c -O3 -g -Wall -I$(INCLUDE) -I. -I/AFLplusplus/include

afl_vector.a: afl_vector.c
		$(CC) $(CFLAGS) $^ -o$@

test_vector.o: test_vector.c 
		$(CC) $(CFLAGS) $^ -o$@

test_vector: afl_vector.a test_vector.o 
		$(CC) $^ -o$@

install: afl_vector.a 
		cp afl_vector.a $(LIB)
		cp afl_vector.h $(INCLUDE)
		make clean
