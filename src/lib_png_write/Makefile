# dirictories
ROOT = ../..
LIB = $(ROOT)/lib
INCLUDE = $(ROOT)/include
BIN = $(ROOT)/bin


# compiler 
CC = clang
CFLAGS = -c -fPIC -O3 -Wall -I/AFLplusplus/include -I. -I$(INCLUDE) -g



#linker
LD_FLAGS = 



# compile
TARGET = png_write.o
CFILES = png_write.c

$(TARGET): $(CFILES)
		$(CC) $(CFLAGS) $^ -o $@ $(LD_FLAGS)



clean:
		rm -f *.o $(TARGET)

install: 
		cp png_write.h $(INCLUDE)
		cp $(TARGET) $(BIN)
		make clean

all: $(TARGET) install

