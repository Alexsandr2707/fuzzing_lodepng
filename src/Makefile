REQLIB = lib_afl_vector lib_vector lib_png_processing lib_png_write lib_png_random
ROOT = /fuzzing_lodepng/src

all: config_png_random 

config_png_random:
		for i in ${REQLIB}; \
		do \
				cd "$(ROOT)/$$i" && make all; \
		done
