# Makefile Version 1.1
# INPUT: path to opencv installation; DEFAULT /usr/local
#

PROJECT=cvpctsig
VERSION=1.0

OS=Linux
CXX=g++
CXXFLAGS=-std=c++11 -m64

TARGET=build
BIN=bin
LIB=lib
EXT=ext

SRC= $(PROJECT)/src
SOURCES= $(wildcard $(SRC)/*.cpp)
OBJECTS= $(patsubst $(SRC)/%.cpp,$(TARGET)/$(EXT)/%.o,$(SOURCES))
TARGETSHARED = $(PROJECT).so.1.0
TARGETSTATIC = $(PROJECT)_$(VERSION).a

OPENCVDIR := $(OPENCV)
ifdef OPENCVDIR
OPENCVDIR := $(OPENCV)
INCDIR = `pkg-config --cflags /opt/local/lib/pkgconfig/opencv.pc`
LDLIBSOPTIONS = -L$(OPENCV)/lib -lopencv_core -lopencv_highgui -lopencv_features2d -opencv_imgproc
else
OPENCVDIR := /usr/local/
INCDIR = `pkg-config --cflags opencv`
LDLIBSOPTIONS =`pkg-config --libs opencv`
LDLIBSOPTIONS += -L/usr/local/share/OpenCV/3rdparty/lib
endif

#Header Files
INCDIR+= -I./include

.PHONY: all

info:
	@echo "OpenCV Installation: " $(OPENCVDIR) "\n"
	@echo "OpenCV Header Include Directory: " $(INCDIR) "\n"
	@echo "OpenCV Linking: " $(LDLIBSOPTIONS) "\n"

all: directories shared prog static

directories:
	mkdir -p $(TARGET)/$(BIN)
	mkdir -p $(TARGET)/$(LIB)
	mkdir -p $(TARGET)/$(EXT)

prog: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)/$(BIN)/prog$(PROJECT)_$(VERSION) $(LDLIBSOPTIONS)

shared: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDLIBSOPTIONS) -Wall -shared -Wl,-soname, $(TARGET)/$(EXT)/*.o -o $(TARGET)/$(LIB)/lib$(PROJECT)_$(VERSION).so.1.0

$(TARGET)/$(EXT)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@ $(INCDIR)

static: $(OBJECTS)
	ar -rv $(TARGET)/$(LIB)/lib$(PROJECT)_$(VERSION).a $(OBJECTS)

# shared: $(OBJECTS)
# 	$(CXX) $(CXXFLAGS) $(LDLIBSOPTIONS) -shared -Wl,-soname,$(PROJECT)_$(VERSION).so -o $@ $^

clean:
	rm -rf build
