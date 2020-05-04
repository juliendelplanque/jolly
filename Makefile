.phony: all clean test

all:
	cmake -B build
	cmake --build build
	ln -fs build/src/main jolly

clean:
	rm -fr build/ jolly

test: all
	cd build && env CTEST_OUTPUT_ON_FAILURE=1 ctest 
	cd ..
