# dirictories
ROOT = ../..
LIB = $(ROOT)/lib
INCLUDE = $(ROOT)/include



# compiler 
CC = clang
CFLAGS = -c -fPIC -O2 -Wall -I/AFLplusplus/include -I. -I$(INCLUDE) -g #-fsanitize=address,leak  



#linker
LD_FLAGS = -lpng -lm #-fsanitize=address,leak



# compile
TARGET = png_random.o
CFILES = png_random.c

test.o: test.c 
		$(CC) $(CFLAGS) $^ -o $@

test: test.o $(LIB)/png_random.o $(LIB)/*.o $(LIB)/afl_vector.a $(LIB)/vector.a
		$(CC) $^ -o $@ $(LD_FLAGS)

gen_png: gen_png.c
		$(CC) $(CFLAGS) $^ 
		$(CC) gen_png.o -o$@ $(LD_FLAGS)

$(TARGET): $(CFILES)
		$(CC) $(CFLAGS) $^ -o $@ 



clean:
		rm -f *.o $(TARGET) test

install: $(TARGET)
		cp png_random.h $(INCLUDE)		
		cp $(TARGET) $(LIB)
		make clean

all: $(TARGET) install

