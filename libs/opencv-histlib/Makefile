
#
#  Version 1.0
#
PROJECT=cvhistlib
VERSION=1_0
OS=Linux
CXX=g++
CXXFLAGS=-std=c++11 -m64
TARGET=build
BIN=bin
STATICLIB=lib
EXT=ext

SRC= src
SOURCES=$(wildcard $(SRC)/*.cpp)
OBJECTS=$(patsubst $(SRC)/%.cpp,$(TARGET)/$(EXT)/%.o,$(SOURCES))

#OPENCV_LIBS=`pkg-config --libs opencv`
#OPENCV_LIBS=`pkg-config --libs /opt/local/lib/pkgconfig/opencv.pc`
#OPENCV_INC=`pkg-config --cflags /opt/local/lib/pkgconfig/opencv.pc`
#OPENCV_LIBS=-lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core -llibwebp -lippicv -lQtOpenGL -lQtGui -lQtTest -lQtCore -lpng -lz -ltiff -ljasper -ljpeg -lImath -lIlmImf -lIex -lHalf -lIlmThread -lgstbase-0.10 -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lxml2 -lglib-2.0 -lgstvideo-0.10 -lgstapp-0.10 -lgstriff-0.10 -lgstpbutils-0.10 -ldc1394 -lavcodec -lavformat -lavutil -lswscale -lstdc++ -ldl -lm -lpthread -lrt -lGLU -lGL -lSM -lICE -lX11 -lXext -ltbb
#OPENCV_LIBS=-L/opt/local/lib -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_viz -lopencv_core
OPENCV_LIBS= -lopencv_core -lopencv_highgui -lopencv_features2d -opencv_imgproc

#INCDIR = `pkg-config --cflags opencv`
INCDIR = `pkg-config --cflags /opt/local/lib/pkgconfig/opencv.pc`
INCDIR+= -I./include
#ADDLIBDIR = -L/usr/local/share/OpenCV/3rdparty/lib
#LDLIBSOPTIONS= $(ADDLIBDIR)
LDLIBSOPTIONS= $(OPENCV_LIBS)

# all:
# 	# @echo $(SOURCES) "\n"
# 	# @echo "Object files:"
# 	# @echo $(OBJECTS)
#
# all: clean directories demo staticlib
all: clean directories demo staticlib

directories:
	mkdir -p $(TARGET)/$(BIN)
	mkdir -p $(TARGET)/$(STATICLIB)
	mkdir -p $(TARGET)/$(EXT)

demo: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)/$(BIN)/$(PROJECT)_$(VERSION) $(LDLIBSOPTIONS) -Wnarrowing

$(TARGET)/$(EXT)/%.o: $(SRC)/%.cpp
	   $(CXX) $(CXXFLAGS) -c $< -o $@ $(INCDIR)

staticlib: $(OBJECTS)
	ar -rv $(TARGET)/$(STATICLIB)/$(PROJECT)_$(VERSION).a $(OBJECTS)

clean:
	rm -rf build
