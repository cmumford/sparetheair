
# Copyright 2019 Christopher Mumford
# This code is licensed under MIT license (see LICENSE for details)

CLANG_FORMAT=clang-format
ARDUINO=/Applications/Arduino.app/Contents/MacOS/Arduino
FONTCONVERT=../Adafruit-GFX-Library/fontconvert/fontconvert

# Arduino default treats warnings as errors, but there is a
# bug in TinyXML which causes command-line builds to fail.
# Removing -Wextra (which means -Werror) to allow build to
# Succeed.
ARDUINO_FLAGS=--pref compiler.warning_flags.all='-Wall'

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
		sparetheair/size.h \
		sparetheair.ino

.PHONY: verify
verify:
	${ARDUINO} --verify ${ARDUINO_FLAGS} sparetheair/sparetheair.ino
	${ARDUINO} --verify ${ARDUINO_FLAGS} sparetheair.ino

sparetheair/font_base.h: Makefile
	${FONTCONVERT} fonts/windows_command_prompt.ttf 11 > sparetheair/font_base.h

sparetheair/font_medium.h: Makefile
	${FONTCONVERT} fonts/LibreBaskerville-Bold.ttf 13 > sparetheair/font_medium.h

sparetheair/font_large.h: Makefile
	${FONTCONVERT} fonts/LibreBaskerville-Bold.ttf 18 > sparetheair/font_large.h

.PHONY: fonts
fonts: sparetheair/font_base.h sparetheair/font_medium.h sparetheair/font_large.h
