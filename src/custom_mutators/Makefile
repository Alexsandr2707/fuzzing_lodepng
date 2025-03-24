# dirictories
ROOT = ../..
LIB = $(ROOT)/lib
INCLUDE = $(ROOT)/include
AFL_HEADERS ?= /AFLplusplus/include


# compiler 
CC = clang
CFLAGS = -c -O3 -fPIC -Wall -I$(AFL_HEADERS) -I. -I$(INCLUDE) -g



#linker
LD_FLAGS = -shared  -L$(LIB) -lpng_random -lpng -lm



# compile
TARGET = libafl_custom_mutator.so

CFILES = afl_custom_mutator.c
MUTATOR_OBJ = afl_custom_mutator.o

OBJ = $(MUTATOR_OBJ) 


$(MUTATOR_OBJ): $(CFILES)
		$(CC) $(CFLAGS) $^ -o $@

$(TARGET): $(OBJ)
		$(CC) $^ -o$@ $(LD_FLAGS)


clean:
		rm -f *.o $(TARGET)

install: 
		cp $(TARGET) $(LIB)
		make clean

all: $(TARGET) install

