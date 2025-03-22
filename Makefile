# directories
INCLUDE = include
INPUT = input/start_input
OUTPUT = output 
LIB = lib
SRC = src
UTILS = utils
DICTIONARIES = dictionaries
BIN = bin



# compiler
CC = afl-clang-fast
CXX = afl-clang-fast++

CFLAGS = -O3 -g -Wall -I. -Iinclude
CXXFLAGS = -O3 -g -Wall -I. -Iinclude -Wextra -pedantic -ansi
COVFLAGS = -fprofile-arcs -ftest-coverage 



# linker 
LDFLAGS = 



# fuzzer 
FUZZER = afl-fuzz

DICTIONARY = $(DICTIONARIES)/png.dict
CUSTOM_MUTATOR_LIBRARY="$(LIB)/libafl_custom_mutator.so"

MAIN_FUZZER_FLAGS = -t 100 -c 0 -i $(INPUT) -o $(OUTPUT) -x $(DICTIONARY)
WORKER_FUZZER_FLAGS = -t 100 -c 0 -i $(INPUT) -o $(OUTPUT) -x $(DICTIONARY)
CMPLOG_FUZZER_FLAGS = -t 100 -c 0 -i $(INPUT) -o $(OUTPUT) -x $(DICTIONARY)
COVERAGE_FLAGS = -d $(OUTPUT) -c ./ --cover-corpus --lcov-web-all 

RESUME_FLAG = #AFL_AUTORESUME=1
CUSTOM_MUTATORS = AFL_CUSTOM_MUTATOR_LIBRARY=$(CUSTOM_MUTATOR_LIBRARY)



# compile fuzz programs
CXXFILES = $(SRC)/fuzz_program/main.cpp $(SRC)/lodepng/lodepng.cpp

WORKER = $(BIN)/worker
CMPLOGER = $(BIN)/cmploger
COVERAGE = $(BIN)/coverage

$(CMPLOGER): $(CXXFILES)
		AFL_LLVM_CMPLOG=1 \
		AFL_LLVM_LAF_ALL=1 \
		$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(WORKER): $(CXXFILES)
		$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
		
$(COVERAGE): $(CXXFILES)
		$(CXX) $(CXXFLAGS) $(COVFLAGS) $^ -o $@ $(LDFLAGS)
		

# fuzzing
run_main: $(WORKER)
		AFL_FINAL_SYNC=1 \
		$(RESUME) \
		$(CUSTOM_MUTATORS) \
		$(FUZZER) $(MAIN_FUZZER_FLAGS) -M $@ -- $(WORKER)  @@


run_workers: $(WORKER)
		$(RESUME) \
		$(CUSTOM_MUTATORS) \
		$(FUZZER) $(WORKER_FUZZER_FLAGS) -S $@ -- $(WORKER) @@ 

run_coverage: $(COVERAGE)
		cd $(UTILS)/coverage && make live_coverage


all:  $(WORKER) $(CMPLOGER) $(COVERAGE)

clean:
		rn -f $(BIN)/*
