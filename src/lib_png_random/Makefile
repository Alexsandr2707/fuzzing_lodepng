# dirictories
ROOT = ../..
LIB = $(ROOT)/lib
INCLUDE = $(ROOT)/include



# compiler 
CC = clang
CFLAGS = -c -fPIC -O3 -Wall -I/AFLplusplus/include -I. -I$(INCLUDE) -g



#linker
LD_FLAGS = -lpng 



# compile
TARGET = png_random.o
CFILES = png_random.c

test.o: test.c
		$(CC) $(CFLAGS) $^ -o $@

test: test.o $(LIB)/png_random.o $(LIB)/*.o $(LIB)/afl_vector.a
		$(CC) $^ -o $@ $(LD_FLAGS)


$(TARGET): $(CFILES)
		$(CC) $(CFLAGS) $^ -o $@ 



clean:
		rm -f *.o $(TARGET) test

install: $(TARGET)
		cp png_random.h $(INCLUDE)		
		cp $(TARGET) $(LIB)
		make clean

all: $(TARGET) install

