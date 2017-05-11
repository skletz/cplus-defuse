# Makefile Version 1.1
# INPUT: path to opencv installation; DEFAULT /usr/local
#

PROJECT=xtraction
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
LDLIBSOPTIONS =`pkg-config --libs /opt/local/lib/pkgconfig/opencv.pc`
else
OPENCVDIR := /usr/local/
INCDIR = `pkg-config --cflags opencv`
LDLIBSOPTIONS =`pkg-config --libs opencv`
LDLIBSOPTIONS += -L/usr/local/share/OpenCV/3rdparty/lib
endif


#OpenCV, Boost Cplusutil, Cplulogger Directory, Header Files, Libraries
#Staic Feature Signatures
PCTSIGNATURES="../../libs/opencv-pctsig"
#Track-based Feature Signatures
TFSIGNATURES="../../libs/opencv-tfsig"
CVHISTLIB="../../libs/opencv-histlib"
UTIL="../../libs/cplusutil"
LOGGER="../../libs/cpluslogger"
#Framework
DEFUSE="../../defuse"


LDLIBSOPTIONS += $(LOGGER)/build/lib/cpluslogger.1.0.a
LDLIBSOPTIONS += $(UTIL)/build/lib/cplusutil.1.0.a
LDLIBSOPTIONS += $(DEFUSE)/build/lib/libdefuse.1.0.a
LDLIBSOPTIONS += $(CVHISTLIB)/build/lib/libcvhistlib.1.0.a
LDLIBSOPTIONS += $(TFSIGNATURES)/build/lib/libcvtfsig.1.0.a
LDLIBSOPTIONS += $(PCTSIGNATURES)/build/lib/libcvpctsig.1.0.a

LDLIBSOPTIONS += -lboost_filesystem -lboost_system

INCDIR+= -I$(LOGGER)/cpluslogger/include
INCDIR+= -I$(UTIL)/cplusutil/include
INCDIR+= -I$(DEFUSE)/include
INCDIR+= -I$(PCTSIGNATURES)/cvpctsig/include
INCDIR+= -I$(TFSIGNATURES)/cvtfsig/include
INCDIR+= -I$(CVHISTLIB)/include


.PHONY: all

info:
	@echo "OpenCV Installation: " $(OPENCVDIR) "\n"
	@echo "OpenCV Header Include Directory: " $(INCDIR) "\n"
	@echo "OpenCV Linking: " $(LDLIBSOPTIONS) "\n"

all: directories prog

directories:
	mkdir -p $(TARGET)/$(BIN)
	mkdir -p $(TARGET)/$(LIB)
	mkdir -p $(TARGET)/$(EXT)

prog: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)/$(BIN)/prog$(PROJECT).$(VERSION) $(LDLIBSOPTIONS)

$(TARGET)/$(EXT)/%.o: $(SRC)/%.cpp
	   $(CXX) $(CXXFLAGS) -c $< -o $@ $(INCDIR)

clean:
	rm -rf build
