# dirictories
ROOT = ../..
LIB = $(ROOT)/lib
INCLUDE = $(ROOT)/include



# compiler 
CC = clang
CFLAGS = -c -fPIC -O3 -Wall -I/AFLplusplus/include -I. -I$(INCLUDE) -g



#linker
LD_FLAGS = 



# compile
TARGET = lib_png_processing.o
CFILES = png_processing.c

$(TARGET): $(CFILES)
		$(CC) $(CFLAGS) $^ -o $@ $(LD_FLAGS)



clean:
		rm -f *.o $(TARGET)

install: 
		cp png_processing.h $(INCLUDE)
		cp $(TARGET) $(LIB)
		make clean

all: $(TARGET) install

