# Makefile Version 1.1
# INPUT: path to opencv installation; DEFAULT /usr/local
#

PROJECT=MSBXtraction
VERSION=1.0

OS=Linux
CXX=g++
CXXFLAGS=-std=c++11 -m64

TARGET=build
BIN=bin
LIB=lib
EXT=ext

SRC= ./
SOURCES=$(wildcard $(SRC)/*.cpp)
OBJECTS=$(patsubst $(SRC)/%.cpp,$(TARGET)/$(EXT)/%.o,$(SOURCES))

#IF ANY Value of the VARIABLE is set /opt/local/ is used
OPENCVDIR := $(OPENCV)
ifdef OPENCVDIR
OPENCVDIR := $(OPENCV)
INCDIR = `pkg-config --cflags /opt/local/lib/pkgconfig/opencv.pc`
LDLIBSOPTIONS_POST =`pkg-config --libs /opt/local/lib/pkgconfig/opencv.pc`
else
OPENCVDIR := /usr/local/
INCDIR = `pkg-config --cflags opencv`
LDLIBSOPTIONS_POST =`pkg-config --libs opencv`
LDLIBSOPTIONS_POST += -L/usr/local/share/OpenCV/3rdparty/lib
endif

UTIL="../../libs/cplusutil"
LOGGER="../../libs/cpluslogger"

LDLIBSOPTIONS = $(LOGGER)/build/lib/libcpluslogger.1.0.a
LDLIBSOPTIONS += $(UTIL)/build/lib/libcplusutil.1.0.a

LDLIBSOPTIONS += -lboost_filesystem -lboost_system -lboost_program_options
#IMPORTANT: Link sequence! - OpenCV libraries have to added at the end
LDLIBSOPTIONS += $(LDLIBSOPTIONS_POST)

INCDIR+= -I$(LOGGER)/cpluslogger/include
INCDIR+= -I$(UTIL)/cplusutil/include


.PHONY: all

info:
	@echo "OpenCV Installation: " $(OPENCVDIR) "\n"
	@echo "Include Directory: " $(INCDIR) "\n"
	@echo "Linking: " $(LDLIBSOPTIONS) "\n"

all: directories prog

directories:
	mkdir -p $(TARGET)/$(BIN)
	mkdir -p $(TARGET)/$(LIB)
	mkdir -p $(TARGET)/$(EXT)

prog: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)/$(BIN)/prog$(PROJECT).$(VERSION) $(LDLIBSOPTIONS)

$(TARGET)/$(EXT)/%.o: $(SRC)/%.cpp
	   $(CXX) $(CXXFLAGS) -fPIC -c $< -o $@ $(INCDIR)

clean:
	rm -rf build
