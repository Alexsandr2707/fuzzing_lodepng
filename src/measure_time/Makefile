LIB = ../../lib
INCLUDE = ../../include
BIN = ../../bin
AFL_HEADERS ?= /AFLplusplus/include

CC = clang
CFLAGS = -c -fPIC -O3 -g -Wall -I$(INCLUDE) -I. -I$(AFL_HEADERS)

measure_time.o: measure_time.c
		$(CC) $(CFLAGS) $^ -o$@

install: 
		cp measure_time.o $(BIN)
		cp measure_time.h $(INCLUDE)
		make clean

all: measure_time.o install

clean:
		rm *.o

