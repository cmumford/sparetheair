
CLANG_FORMAT='clang-format'

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

.PHONY: format
format:
	${CLANG_FORMAT} -i sparetheair.cpp
	${CLANG_FORMAT} -i sparetheair.h
	${CLANG_FORMAT} -i sparetheair.ino
