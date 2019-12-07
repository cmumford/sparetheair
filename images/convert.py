#!/usr/bin/env python3

# Copyright 2019 Christopher Mumford
# This code is licensed under MIT license (see LICENSE for details)

# apt-get install python3-pil
from PIL import Image
import sys

def isTransparent(pixel):
  """White color is mapped to transparent."""
  return pixel[0] == 255 and pixel[1] == 255 and pixel[2] == 255

def isRed(pixel):
  return pixel[0] == 255 and pixel[1] == 0 and pixel[2] == 0

def isBlack(pixel):
  return pixel[0] == 0 and pixel[1] == 0 and pixel[2] == 0

def isWhite(pixel):
  """Green pixels in the source image are mapped to white pixels."""
  return pixel[0] == 0 and pixel[1] == 255 and pixel[2] == 0

def convertStream(fname):
  img=Image.open(fname)
  print('const uint8_t logo[] = {')
  for row in range(img.height):
    line = '  '
    for col in range(img.width):
      rgb = img.getpixel((col, row))
      if isTransparent(rgb):
        line += '0,'
      elif isRed(rgb):
        line += '1,'
      elif isBlack(rgb):
        line += '2,'
      elif isWhite(rgb):
        line += '3,'
      else:
        line += '0,'
    print(line);
  print('};')

if len(sys.argv) != 2:
  print('usage convert.py <fname>', file=sys.stderr)
  sys.exit(1)
convertStream(sys.argv[1])
