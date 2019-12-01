#!/usr/bin/env python3

import fileinput

print('const char var[] =')
for line in fileinput.input():
  line = line.strip()
  line = line.replace('"', r'\"')
  print(r' "%s\n"' % line)
print(';')
