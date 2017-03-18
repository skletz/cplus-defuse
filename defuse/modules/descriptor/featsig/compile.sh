#!/bin/bash

g++ -shared -Wl,-soname,libpctsignatures.so.1.0 -fPIC -std=c++0x -I/usr/local/include -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_features2d -opencv_imgproc include/*.h src/*.cpp src/*.hpp -o libpctsignatures.so.1.0

