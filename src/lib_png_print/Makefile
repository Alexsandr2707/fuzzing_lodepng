# dirictories
ROOT = ../..
LIB = $(ROOT)/lib
INCLUDE = $(ROOT)/include
BIN = $(ROOT)/bin


# compiler 
CC = clang
SANITIZERS = #-fsanitize=memory
CFLAGS = -c -fPIC -O2 -Wall -I/AFLplusplus/include -I. -I$(INCLUDE) -g $(SANITIZERS)

# compile
TARGET = png_print.o
CFILES = png_print.c

$(TARGET): $(CFILES)
		$(CC) $(CFLAGS) $^ -o$@

clean:
		rm -f *.o $(TARGET)

install: 
		cp png_print.h $(INCLUDE)		
		cp $(TARGET) $(BIN)
		make clean

all: $(TARGET) install

