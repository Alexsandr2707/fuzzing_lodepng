all: build live_coverage

build: /fuzzing_lodepng/main.cpp /fuzzing_lodepng/lodepng.cpp /fuzzing_lodepng/lodepng.h
		mkdir -p source 
		cp /fuzzing_lodepng/main.cpp /fuzzing_lodepng/lodepng.cpp /fuzzing_lodepng/lodepng.h ./source
		cd source && g++ -Og -g -fprofile-arcs -ftest-coverage -Isource -I/AFLplusplus/include *.cpp -o main --coverage

live_coverage: build
		afl-cov -d /fuzzing_lodepng/output \
		--live \
		--coverage-cmd "/fuzzing_lodepng/coverage/source/main @@" \
		--code-dir /fuzzing_lodepng/coverage/source \
		--enable-branch-coverage
		

get_coverage:
		afl-cov -d /fuzzing_lodepng/output \
		--coverage-cmd "/fuzzing_lodepng/coverage/source/main @@" \
		--code-dir /fuzzing_lodepng/coverage/source \
		--enable-branch-coverage

clean:
		rm -rf ../output/cov
