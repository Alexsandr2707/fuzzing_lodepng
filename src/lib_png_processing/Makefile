# dirictories
ROOT = ../..
LIB = $(ROOT)/lib
INCLUDE = $(ROOT)/include
BIN = $(ROOT)/bin
AFL_HEADERS ?= /AFLplusplus/include


# compiler 
CC = clang
CFLAGS = -c -fPIC -O3 -Wall -I$(AFL_HEADERS) -I. -I$(INCLUDE) -g

#linker
LD_FLAGS = 



# compile
CFILES = png_processing.c
TARGET = png_processing.o

$(TARGET): $(CFILES)
		$(CC) $(CFLAGS) $^ -o $@ 

clean:
		rm -f *.o *.a $(TARGET)

install: 
		cp png_processing.h $(INCLUDE)
		cp $(TARGET) $(BIN)
		make clean

all: $(TARGET) install 

