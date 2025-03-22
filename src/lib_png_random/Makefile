# dirictories
ROOT = ../..
LIB = $(ROOT)/lib
INCLUDE = $(ROOT)/include
BIN = $(ROOT)/bin


# compiler 
CC = clang
CFLAGS = -c -O2 -Wall -I/AFLplusplus/include -I. -I$(INCLUDE) -g -fsanitize=address

AR = ar
ARFLAGS = rsc

#linker
LD_FLAGS = -lpng -lm -fsanitize=address



# compile
TARGET = png_random.a
CFILES = png_random.c
PNGRAND = png_random.o
OBJ = $(PNGRAND) $(BIN)/afl_vector.o $(BIN)/vector.o $(BIN)/png_processing.o $(BIN)/png_write.o

test.o: test.c 
		$(CC) $(CFLAGS) $^ -o $@

test: test.o $(LIB)/$(TARGET)
		$(CC) $^ -o $@ $(LD_FLAGS)

gen_png: gen_png.c
		$(CC) $(CFLAGS) $^ 
		$(CC) gen_png.o -o$@ $(LD_FLAGS)

$(PNGRAND): $(CFILES)
		$(CC) $(CFLAGS) $^ -o $@ 

$(TARGET): $(OBJ)
		$(AR) $(ARFLAGS) $@ $^


clean:
		rm -f *.o $(TARGET) test

install: 
		cp png_random.h $(INCLUDE)		
		cp $(TARGET) $(LIB)
		make clean

all: $(TARGET) install

