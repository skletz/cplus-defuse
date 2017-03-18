#!/bin/bash

g++ -shared -Wl,-soname,libtfsignatures.so.1.0 -fPIC -std=c++0x -I/usr/local/include -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_features2d -opencv_imgproc include/*.h src/algo/*.h src/common/*.h src/sbvr/*.h src/ui/*.h -o libtfsignatures.so.1.0

