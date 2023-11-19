#!/bin/sh

sudo echo "~~~ OpenCV latest release installer ~~~" && echo "" &&
sudo apt update &&
sudo apt install git cmake make g++-12 &&
git clone https://github.com/opencv/opencv.git &&
cd opencv && mkdir -p build && cd build &&
cmake -DWITH_QT=ON -DWITH_OPENGL=ON .. &&
make -j4 &&
sudo make install &&
echo "" && echo "~~~ OpenCV installed :) ~~~"
