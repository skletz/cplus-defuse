#
#  Version 1.0
#
OS=Linux
VERSION=2_0
CXX=g++
CXXFLAGS=-std=c++11 -m64
TARGET=build
BIN=bin
STATICLIB=staticlib
EXT=ext

SRC= ./
SOURCES=$(wildcard $(SRC)/*.cpp)
OBJECTS=$(patsubst $(SRC)/%.cpp,$(TARGET)/$(EXT)/%.o,$(SOURCES))

#OpenCV, Boost Cplusutil, Cplulogger Directory, Header Files, Libraries
UTIL="../../libs/cplusutil"
LOGGER="../../libs/cpluslogger"
DEFUSE="../../defuse"

#OPENCV_LIBS=-lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab
OPENCV_LIBS=`pkg-config --static --libs opencv`

INCDIR=-I/usr/local/include/opencv2 -I/usr/local/include/opencv -I../../../cpluslogger/cpluslogger/include -I../../../cplusutil/cplusutil/include -I../../defuse/include
ADDLIBDIR=-L/usr/local/lib $(LOGGER)/build/staticlib/cpluslogger_1_0.a $(UTIL)/build/staticlib/cplusutil_1_0.a $(DEFUSE)/build/staticlib/defuse_2_0.a
LDLIBSOPTIONS=$(ADDLIBDIR) $(OPENCV_LIBS) -lboost_filesystem -lboost_system

all: clean directories libs prog

directories:
	mkdir -p $(TARGET)/$(BIN)
	mkdir -p $(TARGET)/$(STATICLIB)
	mkdir -p $(TARGET)/$(EXT)

prog: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)/$(BIN)/de_xtraction $(LDLIBSOPTIONS)

$(TARGET)/$(EXT)/%.o: $(SRC)/%.cpp
	   $(CXX) $(CXXFLAGS) -c $< -o $@ $(INCDIR)

libs:
	$(MAKE) -C $(DEFUSE)

clean:
	rm -rf build
	$(MAKE) -C $(DEFUSE) clean

#-g -I../../../cpluslogger/cpluslogger/include -I../../../cplusutil/cplusutil/include -I/usr/local/include/opencv2 `pkg-config --cflags opencv` -std=c++11
