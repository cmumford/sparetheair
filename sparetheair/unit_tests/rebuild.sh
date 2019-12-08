#!/bin/bash -e

rm -rf build
mkdir build
cd build
cmake -Wno-dev ..
make --jobs=4
