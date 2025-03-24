REQLIB = lib_afl_vector lib_vector lib_png_processing lib_png_write lib_png_print lib_png_random custom_mutators
ROOT = .
AFL_HEADERS ?= /AFLplusplus/include

all: config_png_random 

config_png_random:
		@for i in ${REQLIB}; \
		do \
				(cd "$(ROOT)/$$i" && AFL_HEADERS=$(AFL_HEADERS) make all); \
		done
