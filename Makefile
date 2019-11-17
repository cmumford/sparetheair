
.PHONY: default
default: run

test: Makefile main.cc
	clang main.cc -std=c++11 -lstdc++ -o test

.PHONY: run
run: test
	./test

.PHONY: clean
clean:
	rm test
