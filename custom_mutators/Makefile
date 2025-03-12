all: lib_post_program.so testlib

CC = clang
CFLAGS = -shared -O3 -Wall -I/AFLplusplus/include -I. -g
LD_FLAGS = -fPIC -L/fuzzing_lodepng/no_mod_libpng -lpng -lm

testlib: testlib.c
		gcc -I/AFLplusplus/include -I. testlib.c -lpng -lm -o$@

clean:
		rm -f  lib_post_program.so
lib_post_program.so: compress_png.c
		$(CC) $(CFLAGS) $^ -o $@ $(LD_FLAGS)

