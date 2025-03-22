# dirictories
ROOT = ../..
LIB = $(ROOT)/lib
INCLUDE = $(ROOT)/include
BIN = $(ROOT)/bin



# compiler 
CC = clang
CFLAGS = -c -O3 -Wall -I/AFLplusplus/include -I. -I$(INCLUDE) -g

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

