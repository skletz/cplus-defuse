# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	Makefile for defuse (Dynamic Features)
# @author skletz
# @version 2.0, 08/06/17 change it to a content feature library
# @version 1.0 01/05/17
# -----------------------------------------------------------------------------
# CMD Arguments:	os=win,linux (sets the operating system, default=linux)
#									opencv=usr,opt (usr=/user/local/, opt=/opt/local/; default=usr)
# -----------------------------------------------------------------------------
# @TODO: Make for Windows (currently the option is only considered)
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# Default command line arguments
os = linux
opencv = usr

# Compiler, flags
CXX=g++
CXXFLAGS=-std=c++11 -m64

# Output directory
BUILD=builds
BIN =	linux/bin
LIB =	linux/lib
EXT =	linux/ext

# Project settings
PROJECT=defuse
VERSION=2.0

SRC= $(PROJECT)/src
SOURCES= $(wildcard $(SRC)/*.cpp)
OBJECTS= $(patsubst $(SRC)/%.cpp,$(BUILD)/$(EXT)/%.o,$(SOURCES))

TARGETSHARED = $(PROJECT).so.$(VERSION)
TARGETSTATIC = $(PROJECT).$(VERSION).a

# Default settings for opencv installation directory
OPENCVDIR := /usr/local/
INCDIR = `pkg-config --cflags opencv`
LDLIBSOPTIONS_POST =`pkg-config --libs opencv`
LDLIBSOPTIONS_POST += -L/usr/local/share/OpenCV/3rdparty/lib


# operating system can be changed via command line argument
ifeq ($(os),win)
	BIN := win/bin
	LIB := win/lib
	EXT := win/ext
endif

# opencv installation dir can be changed via command line argument
ifeq ($(opencv),opt)
	OPENCVDIR := /opt/local/
	INCDIR = `pkg-config --cflags /opt/local/lib/pkgconfig/opencv.pc`
	LDLIBSOPTIONS_POST =`pkg-config --libs /opt/local/lib/pkgconfig/opencv.pc`
endif

# OpenCV, Boost Cplusutil, Cplulogger Directory, Header Files, Libraries
# Staic Feature Signatures
PCTSIGNATURES="../opencv-pctsig"
# Track-based Feature Signatures
TFSIGNATURES="../opencv-tfsig"
# Visualization of motion histogram
CVHISTLIB="../opencv-histlib"
UTIL="../cplusutil"
LOGGER="../cpluslogger"

LIBS="../../$(BUILD)/$(LIB)"

LDLIBSOPTIONS += $(LIBS)/libcpluslogger.1.0.a
LDLIBSOPTIONS += $(LIBS)/libcplusutil.1.0.a
LDLIBSOPTIONS += $(LIBS)/libcvpctsig.1.0.a
LDLIBSOPTIONS += $(LIBS)/libcvtfsig.1.0.a
LDLIBSOPTIONS += $(LIBS)/libcvpctsig.1.0.a
LDLIBSOPTIONS += $(LIBS)/libcvhistlib.1.0.a

LDLIBSOPTIONS += -lboost_filesystem -lboost_system -lboost_serialization
#IMPORTANT: Link sequence! - OpenCV libraries have to be added at the end
LDLIBSOPTIONS += $(LDLIBSOPTIONS_POST)

INCDIR+= -I./include
INCDIR+= -I$(LOGGER)/cpluslogger/include
INCDIR+= -I$(UTIL)/cplusutil/include
INCDIR+= -I$(PCTSIGNATURES)/cvpctsig/include
INCDIR+= -I$(TFSIGNATURES)/cvtfsig/include
INCDIR+= -I$(CVHISTLIB)/include

.PHONY: all

all: directories prog static shared

directories:
	mkdir -p $(BUILD)/$(BIN)
	mkdir -p $(BUILD)/$(LIB)
	mkdir -p $(BUILD)/$(EXT)

prog: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BUILD)/$(BIN)/prog$(PROJECT).$(VERSION) $(LDLIBSOPTIONS)

shared: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDLIBSOPTIONS) -Wall -shared -Wl,-soname, $(BUILD)/$(EXT)/*.o -o $(BUILD)/$(LIB)/lib$(TARGETSHARED)

$(BUILD)/$(EXT)/%.o: $(SRC)/%.cpp
	   $(CXX) $(CXXFLAGS) -fPIC -c $< -o $@ $(INCDIR)

static: $(OBJECTS)
	ar -rv $(BUILD)/$(LIB)/lib$(TARGETSTATIC) $(OBJECTS)

clean:
	rm -rf $(BUILD)

info:
	@echo "OpenCV Installation: " $(OPENCVDIR) "\n"
	@echo "OpenCV Header Include Directory: " $(INCDIR) "\n"
	@echo "OpenCV Linking: " $(LDLIBSOPTIONS) "\n"
