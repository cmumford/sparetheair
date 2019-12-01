
# Copyright 2019 Christopher Mumford
# This code is licensed under MIT license (see LICENSE for details)

CLANG_FORMAT=clang-format
ARDUINO=/Applications/Arduino.app/Contents/MacOS/Arduino

.PHONY: default
default: verify

.PHONY: format
format:
	${CLANG_FORMAT} -i \
		sparetheair/unit_tests.ino \
		sparetheair/display.cpp \
		sparetheair/display.h \
		sparetheair/network.cpp \
		sparetheair/network.h \
		main.ino

.PHONY: verify
verify:
	${ARDUINO} --verify sparetheair/sparetheair.ino
	${ARDUINO} --verify sparetheair.ino
