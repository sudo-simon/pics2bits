#!/bin/sh

mkdir -p build &&
cmake build/ &&
cmake --build ./build --config Release --target all