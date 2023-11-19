#!/bin/sh

mkdir -p build &&
cmake -DCMAKE_BUILD_TYPE=Release build/ &&
cmake --build ./build --target all
