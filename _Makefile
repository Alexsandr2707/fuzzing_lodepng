.PHONY: all coverage 

all: cmploger worker run_fuzzer

clean:
		rm -f worker cmploger

CFLAGS = -O3 -g -Wall -I. -I/fuzzing_lodepng/custom_mutators -Wextra -pedantic -ansi
CC = afl-clang-fast++
LDFLAGS =  

FUZZER = afl-fuzz

RESUME = AFL_AUTORESUME=1
CUSTOM_MUTATORS = #AFL_CUSTOM_MUTATOR_LIBRARY="/fuzzing_lodepng/custom_mutators/lib_post_program.so"

MAIN_FUZZER_FLAGS = -t 100  -c 0 -i - -o output -x /AFLplusplus/dictionaries/png.dict
WORKER_FUZZER_FLAGS = -t 100 -c 0 -i - -o output -x /AFLplusplus/dictionaries/png.dict
CMPLOG_FUZZER_FLAGS = -t 100 -c 0 -i - -o output -x /AFLplusplus/dictionaries/png.dict

cmploger: main.cpp lodepng.cpp
	AFL_LLVM_CMPLOG=1 \
	AFL_LLVM_LAF_ALL=1 \
	$(CC) $(CFLAGS) $^  -o $@ $(LDFLAGS)

worker: main.cpp lodepng.cpp
	$(CC) $(CFLAGS)  $^ -o $@  $(LDFLAGS)


compres_corpus:
		AFL_CUSTOM_MUTATOR_LIBRARY="/fuzzing_lodepng/custom_mutators/lib_post_program.so"	\
		afl-cmin -i base_input/ -o input/ -- ./worker  @@

config_docker:
		apt update
		apt upgrade
		(cd /no_mod_libpng && make install)
		cp ./.vimrc	/root/.vimrc 
		
		apt-get install build-essential git make \
		pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
		libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
		libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
		libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
		libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libpipewire-0.3-dev libwayland-dev libdecor-0-dev liburing-dev
	
		mkdir -p /SDL/build && cd /SDL/build && \
		CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=Release
		

	
coverage:
		afl-clang-fast++ -O2 -fprofile-arcs -ftest-coverage main.c -o$@ $(LDFLAGS)
		afl-cov -d output -c ./  --cover-corpus --lcov-web-all -e ./coverage


valgrind:
		valgrind --leak-check=full  --show-leak-kinds=all $(FUZZER) $(MAIN_FUZZER_FLAGS)  -M main -- ./worker  @@  
		#AFL_CUSTOM_MUTATOR_LIBRARY="/fuzzing_lodepng/custom_mutators/lib_post_program.so" \

run_main:
		mkdir -p output
		AFL_FINAL_SYNC=1 \
		$(RESUME) \
		$(CUSTOM_MUTATORS) \
		$(FUZZER) $(MAIN_FUZZER_FLAGS) -M main -- ./worker  @@ 

run_cmplog:
		$(RESUME) \
		$(CUSTOM_MUTATORS) \
		$(FUZZER) $(CMPLOG_FUZZER_FLAGS) -S cmplog -- ./cmploger @@ #> /dev/null &

run_workers:
		$(RESUME) \
		$(CUSTOM_MUTATORS) \
		$(FUZZER) $(WORKER_FUZZER_FLAGS) -S worker1 -- ./worker @@ # > /dev/null &

run_coverage:
		cd coverage && make live_coverage

run_fuzzer: run_main run_cmplog run_workers 
