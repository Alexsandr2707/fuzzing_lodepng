ROOT = ../..
BIN = $(ROOT)/bin
CMD = $(BIN)/worker

all: afl-min afl-tmin
afl-min:
		rm -rf cmin_inputs
		afl-cmin -i all_inputs -o cmin_inputs -- $(CMD) @@

afl-tmin:
		./make_tmin.sh	

copy_all_files:
		./copy_all_inputs.sh
