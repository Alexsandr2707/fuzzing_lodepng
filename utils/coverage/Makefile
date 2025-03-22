ROOT = ../..
SRC = $(ROOT)/src
INCLUDE = $(ROOT)/include
OUTPUT = $(ROOT)/output

COV = afl-cov
COVFLAGS = --coverage-cmd "source/main @@" --code-dir source --enable-branch-coverage


all: build live_coverage

build: $(SRC)/fuzz_program/main.cpp $(SRC)/lodepng/lodepng.cpp $(INCLUDE)/lodepng.h
		mkdir -p source 
		cp $^  ./source
		cd source && g++ -Og -g -fprofile-arcs -ftest-coverage -Isource -I/AFLplusplus/include *.cpp -o main --coverage

live_coverage: build
		$(COV) -d $(OUTPUT)  $(COVFLAGS) --live 
		

get_coverage: build
		$(COV) -d $(OUTPUT)  $(COVFLAGS) 

clean:
		rm -rf ../output/cov
