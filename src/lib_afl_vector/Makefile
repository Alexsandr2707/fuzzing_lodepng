LIB = ../../lib
INCLUDE = ../../include
BIN = ../../bin

all: afl_vector.o install

clean:
		rm *.o test_vector

CC = clang
CFLAGS = -c -O3 -g -Wall -I$(INCLUDE) -I. -I/AFLplusplus/include

afl_vector.o: afl_vector.c
		$(CC) $(CFLAGS) $^ -o$@

test_vector.o: test_vector.c 
		$(CC) $(CFLAGS) $^ -o$@

test_vector: afl_vector.o test_vector.o 
		$(CC) $^ -o$@

install: 
		cp afl_vector.o $(BIN)
		cp afl_vector.h $(INCLUDE)
		make clean
