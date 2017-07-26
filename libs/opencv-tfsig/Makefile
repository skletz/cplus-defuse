# Makefile Version 1.1
# INPUT: path to opencv installation; DEFAULT /usr/local
#

PROJECT=cvtfsig
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
TARGETSTATIC = $(PROJECT).$(VERSION).a

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

#Staic Feature Signatures
PCTSIGNATURES="../opencv-pctsig"
UTIL="../cplusutil/"
LOGGER="../cpluslogger"

#OPENCV_LIBS_STATIC=`pkg-config --static --libs opencv`
#OPENCV_LIBS=-lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core -llibwebp -lippicv -lQtOpenGL -lQtGui -lQtTest -lQtCore -lpng -lz -ltiff -ljasper -ljpeg -lImath -lIlmImf -lIex -lHalf -lIlmThread -lgstbase-0.10 -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lxml2 -lglib-2.0 -lgstvideo-0.10 -lgstapp-0.10 -lgstriff-0.10 -lgstpbutils-0.10 -ldc1394 -lavcodec -lavformat -lavutil -lswscale -lstdc++ -ldl -lm -lpthread -lrt -lGLU -lGL -lSM -lICE -lX11 -lXext -ltbb

LDLIBSOPTIONS += $(PCTSIGNATURES)/build/lib/libcvpctsig.1.0.a
LDLIBSOPTIONS += $(UTIL)/build/lib/libcplusutil.1.0.a
LDLIBSOPTIONS += $(LOGGER)/build/lib/libcpluslogger.1.0.a
LDLIBSOPTIONS += -lboost_filesystem -lboost_system

#INCDIR=-I/usr/local/include/opencv2  -I$(PCTSIGNATURES)/cvpctsig/include -I$(LOGGER)/cpluslogger/include -I$(UTIL)/cplusutil/include
#ADDLIBDIR=-L/usr/local/lib -L/usr/local/share/OpenCV/3rdparty/lib $(CUSTOM_ADDLIBS) -lboost_filesystem -lboost_system
#LDLIBSOPTIONS=$(ADDLIBDIR) $(OPENCV_LIBS)

#Header Files
INCDIR+= -I./$(PROJECT)/include
INCDIR+= -I$(PCTSIGNATURES)/cvpctsig/include
INCDIR+= -I$(LOGGER)/cpluslogger/include
INCDIR+= -I$(UTIL)/cplusutil/include

.PHONY: all

info:
	@echo "OpenCV Installation: " $(OPENCVDIR) "\n"
	@echo "OpenCV Header Include Directory: " $(INCDIR) "\n"
	@echo "OpenCV Linking: " $(LDLIBSOPTIONS) "\n"

all: directories prog shared static

directories:
	mkdir -p $(TARGET)/$(BIN)
	mkdir -p $(TARGET)/$(LIB)
	mkdir -p $(TARGET)/$(EXT)

prog: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)/$(BIN)/prog$(PROJECT).$(VERSION) $(LDLIBSOPTIONS)

shared: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDLIBSOPTIONS) -Wall -shared -Wl,-soname, $(TARGET)/$(EXT)/*.o -o $(TARGET)/$(LIB)/lib$(TARGETSHARED)

$(TARGET)/$(EXT)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@ $(INCDIR)


static: $(OBJECTS)
	ar -rv $(TARGET)/$(LIB)/lib$(TARGETSTATIC) $(OBJECTS)

# shared: $(OBJECTS)
# 	$(CXX) $(CXXFLAGS) $(LDLIBSOPTIONS) -shared -Wl,-soname,$(PROJECT)_$(VERSION).so -o $@ $^

clean:
	rm -rf build
