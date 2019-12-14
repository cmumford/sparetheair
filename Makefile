
# Copyright 2019 Christopher Mumford
# This code is licensed under MIT license (see LICENSE for details)

CLANG_FORMAT=clang-format
ARDUINO=/Applications/Arduino.app/Contents/MacOS/Arduino
FONTCONVERT=../Adafruit-GFX-Library/fontconvert/fontconvert

.PHONY: default
default: verify

.PHONY: format
format:
	${CLANG_FORMAT} -i \
		sparetheair/sparetheair.ino \
		sparetheair/display.cpp \
		sparetheair/display.h \
		sparetheair/network.cpp \
		sparetheair/network.h \
		sparetheair/parser.cpp \
		sparetheair/parser.h \
		sparetheair/size.h \
		sparetheair/status.cpp \
		sparetheair/status.h \
		sparetheair.ino \
		sparetheair/unit_tests/main.cc \
		sparetheair/unit_tests/mock_arduino/Arduino.h \
		sparetheair/unit_tests/mock_arduino/HTTPClient.cpp \
		sparetheair/unit_tests/mock_arduino/HTTPClient.h \
		sparetheair/unit_tests/mock_arduino/Print.h \
		sparetheair/unit_tests/mock_arduino/SPI.h \
		sparetheair/unit_tests/mock_arduino/String.cpp \
		sparetheair/unit_tests/mock_arduino/String.h

.PHONY: verify
verify:
	${ARDUINO} --verify sparetheair/sparetheair.ino
	${ARDUINO} --verify sparetheair.ino

sparetheair/font_base.h: Makefile
	${FONTCONVERT} fonts/windows_command_prompt.ttf 11 > sparetheair/font_base.h

sparetheair/font_medium.h: Makefile
	${FONTCONVERT} fonts/LibreBaskerville-Bold.ttf 13 > sparetheair/font_medium.h

sparetheair/font_large.h: Makefile
	${FONTCONVERT} fonts/LibreBaskerville-Bold.ttf 18 > sparetheair/font_large.h

.PHONY: fonts
fonts: sparetheair/font_base.h sparetheair/font_medium.h sparetheair/font_large.h
