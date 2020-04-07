.phony: all clean test

all:
	cmake -B build
	cmake --build build
	ln -fs build/src/main jolly

clean:
	rm -fr build/ jolly

test: build
	./build/tests/vm_tests
