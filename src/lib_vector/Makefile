LIB = ../../lib
INCLUDE = ../../include
BIN = ../../bin
AFL_HEADERS ?= /AFLplusplus/include

CC = clang
CFLAGS = -c -fPIC -O3 -g -Wall -I$(INCLUDE) -I. -I$(AFL_HEADERS)

vector.o: vector.c
		$(CC) $(CFLAGS) $^ -o$@

test_vector.o: test_vector.c 
		$(CC) $(CFLAGS) $^ -o$@

test_vector: vector.o test_vector.o 
		$(CC) $^ -o$@


install: 
		cp vector.o $(BIN)
		cp vector.h $(INCLUDE)
		make clean

all: vector.o install

clean:
		rm *.o test_vector

