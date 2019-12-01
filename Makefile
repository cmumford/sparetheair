
# Copyright 2019 Christopher Mumford
# This code is licensed under MIT license (see LICENSE for details)

CLANG_FORMAT=clang-format
ARDUINO=/Applications/Arduino.app/Contents/MacOS/Arduino

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
	${CLANG_FORMAT} -i \
		lib/lib.ino \
		lib/sparetheair.cpp \
		lib/sparetheair.h \
		sparetheair.ino

.PHONY: verify
verify:
	${ARDUINO} --verify sparetheair.ino
