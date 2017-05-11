#!/bin/bash

g++ -shared -Wl,-soname,libpctsignatures.so.1.0 -fPIC -std=c++0x -I/opt/local/include -L/opt/local/lib -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc cvpctsig/include/*.h cvpctsig/src/*.cpp cvpctsig/src/*.hpp -o libpctsignatures.so.1.0

