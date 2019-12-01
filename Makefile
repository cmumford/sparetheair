
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
		sparetheair/unit_tests.ino \
		sparetheair/display.cpp \
		sparetheair/display.h \
		sparetheair/network.cpp \
		sparetheair/network.h \
		sparetheair/size.h \
		main.ino

.PHONY: verify
verify:
	${ARDUINO} --verify sparetheair/sparetheair.ino
	${ARDUINO} --verify sparetheair.ino

sparetheair/font_base.h: Makefile
	${FONTCONVERT} fonts/windows_command_prompt.ttf 12 > sparetheair/font_base.h

sparetheair/font_large.h: Makefile
	${FONTCONVERT} fonts/LibreBaskerville-Bold.ttf 20 > sparetheair/font_large.h

.PHONY: fonts
fonts: sparetheair/font_base.h sparetheair/font_large.h
