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
TARGET = png_io.o
CFILES = png_io.c

$(TARGET): $(CFILES)
		$(CC) $(CFLAGS) $^ -o $@ $(LD_FLAGS)



clean:
		rm -f *.o $(TARGET)

install: 
		cp png_io.h $(INCLUDE)
		cp $(TARGET) $(BIN)
		make clean

all: $(TARGET) install

