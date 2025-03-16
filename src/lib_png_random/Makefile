# dirictories
ROOT = ../..
LIB = $(ROOT)/lib
INCLUDE = $(ROOT)/include



# compiler 
CC = clang
CFLAGS = -fPIC -O3 -Wall -I/AFLplusplus/include -I. -I$(INCLUDE) -g



#linker
LD_FLAGS = 



# compile
TARGET = lib_compress_png.so
CFILES = compress_png.c

$(TARGET): $(CFILES)
		$(CC) $(CFLAGS) $^ -o $@ $(LD_FLAGS)



clean:
		rm -f *.o $(TARGET)

install: $(CFILES)
		cp $(TARGET) $(LIB)
		make clean

all: $(TARGET) install

