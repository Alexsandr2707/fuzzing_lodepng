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
TARGET = lib_png_write.o
CFILES = png_write.c

$(TARGET): $(CFILES)
		$(CC) $(CFLAGS) $^ -o $@ $(LD_FLAGS)



clean:
		rm -f *.o $(TARGET)

install: $(TARGET)
		cp png_write.h $(INCLUDE)
		cp $(TARGET) $(LIB)
		make clean

all: $(TARGET) install

