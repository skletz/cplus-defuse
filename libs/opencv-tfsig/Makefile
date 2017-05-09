PROJECT=cvtfsig
VERSION=1_0
OS=Linux
CXX=g++
CXXFLAGS=-std=c++11 -m64
TARGET=build
BIN=bin
STATICLIB=lib
EXT=ext

SRC= $(PROJECT)/src
SOURCES=$(wildcard $(SRC)/*.cpp)
OBJECTS=$(patsubst $(SRC)/%.cpp,$(TARGET)/$(EXT)/%.o,$(SOURCES))

#Staic Feature Signatures
PCTSIGNATURES="../opencv-pctsig"
UTIL="../cplusutil/"
LOGGER="../cpluslogger"

#OPENCV_LIBS_STATIC=`pkg-config --static --libs opencv`
OPENCV_LIBS=-lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core -llibwebp -lippicv -lQtOpenGL -lQtGui -lQtTest -lQtCore -lpng -lz -ltiff -ljasper -ljpeg -lImath -lIlmImf -lIex -lHalf -lIlmThread -lgstbase-0.10 -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lxml2 -lglib-2.0 -lgstvideo-0.10 -lgstapp-0.10 -lgstriff-0.10 -lgstpbutils-0.10 -ldc1394 -lavcodec -lavformat -lavutil -lswscale -lstdc++ -ldl -lm -lpthread -lrt -lGLU -lGL -lSM -lICE -lX11 -lXext -ltbb

CUSTOM_ADDLIBS=-L$(PCTSIGNATURES)/build/lib/cvpctsig_1_0.a $(LOGGER)/build/lib/cpluslogger_1_0.a $(UTIL)/build/lib/cplusutil_1_0.a

INCDIR=-I/usr/local/include/opencv2  -I$(PCTSIGNATURES)/cvpctsig/include -I$(LOGGER)/cpluslogger/include -I$(UTIL)/cplusutil/include
ADDLIBDIR=-L/usr/local/lib -L/usr/local/share/OpenCV/3rdparty/lib $(CUSTOM_ADDLIBS) -lboost_filesystem -lboost_system
LDLIBSOPTIONS=$(ADDLIBDIR) $(OPENCV_LIBS)

all: directories prog static

directories:
	mkdir -p $(TARGET)/$(BIN)
	mkdir -p $(TARGET)/$(STATICLIB)
	mkdir -p $(TARGET)/$(EXT)

prog: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)/$(BIN)/$(PROJECT)_$(VERSION) $(LDLIBSOPTIONS)

$(TARGET)/$(EXT)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCDIR)


static: $(OBJECTS)
	ar -rv $(TARGET)/$(STATICLIB)/$(PROJECT)_$(VERSION).a $(OBJECTS)

# shared: $(OBJECTS)
# 	$(CXX) $(CXXFLAGS) $(LDLIBSOPTIONS) -shared -Wl,-soname,$(PROJECT)_$(VERSION).so -o $@ $^

clean:
	rm -rf build
