#!/bin/bash -e

rm -rf build
mkdir build
cd build
cmake ..
make --jobs=4
