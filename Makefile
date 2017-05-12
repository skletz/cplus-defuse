# Makefile

TARGET=build
BIN=bin
LIB=lib
#OpenCV, Boost Cplusutil, Cplulogger Directory, Header Files, Libraries
LOGGER="./libs/cpluslogger"
UTIL="./libs/cplusutil"
#Staic Feature Signatures
PCTSIGNATURES="./libs/opencv-pctsig"
#Track-based Feature Signatures
TFSIGNATURES="./libs/opencv-tfsig"
CVHISTLIB="./libs/opencv-histlib"
#Framework
DEFUSEDIR="./defuse"
XTRACTION="./tools/xtraction"
VALUATION="./tools/valuation"
DATAINFO="./tools/datainfo"
DATACONST="./tools/dataconst"
#.PHONY: all

ARG1=OPENCV
TEST := $(OPENCV)
ifdef TEST
TEST := $(OPENCV)
VALUE1 = 1
else
TEST := /usr/local/
VALUE1 = ``
endif

info:
	@echo "OpenCV Installation: " $(ARG1)=$(VALUE1) "\n"


all: directories logger util pctsignatures tfsignatuers histlib defusefw xtraction valuation datainfo dataconst

directories:
	mkdir -p $(BUILD)/$(BIN)
	mkdir -p $(BUILD)/$(LIB)
	mkdir -p $(BUILD)/$(EXT)

logger:
	echo "CPLUSLogger: " $(LOGGER)
	$(MAKE) -C $(LOGGER)/ all

util:
	echo "CPLUSUtil: " $(UTIL)
	$(MAKE) -C $(UTIL)/ all

pctsignatures:
	echo "Position-Color-Texture Signatures: " $(PCTSIGNATURES)
	$(MAKE) -C $(PCTSIGNATURES)/ $(ARG1)=$(VALUE1) all

tfsignatuers:
	echo "Track-based Signatures: " $(TFSIGNATURES)
	$(MAKE) -C $(TFSIGNATURES)/ $(ARG1)=$(VALUE1) all

histlib:
	echo "OpenCV-HistLib: " $(CVHISTLIB)
	$(MAKE) -C $(CVHISTLIB)/ $(ARG1)=$(VALUE1) all

defusefw:
	echo "DeFUSE: " $(DEFUSE)
	$(MAKE) -C $(DEFUSEDIR)/ $(ARG1)=$(VALUE1) all

xtraction:
	echo "DeXtraction: " $(XTRACTION)
	$(MAKE) -C $(XTRACTION)/ $(ARG1)=$(VALUE1) all

valuation:
	echo "DeValuation: " $(VALUATION)
	$(MAKE) -C $(VALUATION)/ $(ARG1)=$(VALUE1) all

datainfo:
	echo "DATAInfo: " $(DATAINFO)
	$(MAKE) -C $(DATAINFO)/ $(ARG1)=$(VALUE1) all

dataconst:
	echo "DATAConstruction: " $(DATACONST)
	$(MAKE) -C $(DATACONST)/ $(ARG1)=$(VALUE1) all

clean:
	$(MAKE) -C $(LOGGER)/ clean
	$(MAKE) -C $(UTIL)/ clean
	$(MAKE) -C $(PCTSIGNATURES)/ clean
	$(MAKE) -C $(TFSIGNATURES)/ clean
	$(MAKE) -C $(CVHISTLIB)/ clean
	$(MAKE) -C $(DEFUSEDIR)/ clean
	$(MAKE) -C $(XTRACTION)/ clean
	$(MAKE) -C $(VALUATION)/ clean
	$(MAKE) -C $(DATAINFO)/ clean
	$(MAKE) -C $(DATACONST)/ clean
