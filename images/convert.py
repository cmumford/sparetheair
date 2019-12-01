#!/usr/bin/env python3

# Copyright 2019 Christopher Mumford
# This code is licensed under MIT license (see LICENSE for details)

# apt-get install python3-pil
from PIL import Image

def isWhite(pixel):
  return pixel[0] == 255 and pixel[1] == 255 and pixel[2] == 255

def isRed(pixel):
  return pixel[0] == 255 and pixel[1] == 0 and pixel[2] == 0

def isBlack(pixel):
  return pixel[0] == 0 and pixel[1] == 0 and pixel[2] == 0

def convertStream():
  img=Image.open('winter_logo.png')
  print('const uint8_t logo[%d] = {' % (img.width * img.height))
  for col in range(img.width):
    line = '  '
    for row in range(img.height):
      rgb = img.getpixel((row, col))
      if isWhite(rgb):
        line += '0,'
      elif isRed(rgb):
        line += '1,'
      elif isBlack(rgb):
        line += '2,'
      else:
        line += '0,'
    print(line);
  print('};')

convertStream()
