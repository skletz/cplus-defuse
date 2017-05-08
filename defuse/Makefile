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

SRC= src
SOURCES=$(wildcard $(SRC)/*.cpp)
OBJECTS=$(patsubst $(SRC)/%.cpp,$(TARGET)/$(EXT)/%.o,$(SOURCES))

#OpenCV, Boost Cplusutil, Cplulogger Directory, Header Files, Libraries
UTIL="../libs/cplusutil"
LOGGER="../libs/cpluslogger"

INCDIR=-I/usr/local/include/opencv2 -I../../cpluslogger/cpluslogger/include -I../../cplusutil/cplusutil/include
ADDLIBDIR=-L/usr/local/lib $(LOGGER)/build/staticlib/cpluslogger_1_0.a $(UTIL)/build/staticlib/cplusutil_1_0.a
LDLIBSOPTIONS=$(ADDLIBDIR) `pkg-config --static --libs opencv` -lboost_filesystem -lboost_system

all: clean directories subsystem info staticlib

directories:
	mkdir -p $(TARGET)/$(BIN)
	mkdir -p $(TARGET)/$(STATICLIB)
	mkdir -p $(TARGET)/$(EXT)

info: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)/$(BIN)/defuse_info $(LDLIBSOPTIONS)

$(TARGET)/$(EXT)/%.o: $(SRC)/%.cpp
	   $(CXX) $(CXXFLAGS) -c $< -o $@ $(INCDIR)

subsystem:
	$(MAKE) -C $(LOGGER)
	$(MAKE) -C $(UTIL)

staticlib: $(OBJECTS)
	ar -rv $(TARGET)/$(STATICLIB)/defuse_$(VERSION).a $(OBJECTS)

clean:
	rm -rf build
	$(MAKE) -C $(LOGGER) clean
	$(MAKE) -C $(UTIL) clean

#-g -I../../../cpluslogger/cpluslogger/include -I../../../cplusutil/cplusutil/include -I/usr/local/include/opencv2 `pkg-config --cflags opencv` -std=c++11
