#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/511e4115/features.o \
	${OBJECTDIR}/_ext/511e4115/parameter.o \
	${OBJECTDIR}/_ext/511e4115/videobase.o \
	${OBJECTDIR}/_ext/511e4115/xtractor.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-fPIC
CXXFLAGS=-fPIC

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/lib ../../../cplusutil/nb/dist/Debug/GNU-Linux/libnb.a ../../../cplusutil/nb/dist/Release/GNU-Linux/libnb.a ../../../cpluslogger/nb/dist/Debug/GNU-Linux/libnb.a ../../../cpluslogger/nb/dist/Release/GNU-Linux/libnb.a `pkg-config --static --libs opencv` -lboost_filesystem -lboost_system  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnb.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnb.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnb.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnb.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnb.a

${OBJECTDIR}/_ext/511e4115/features.o: ../src/features.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include/opencv2 -I../../../cpluslogger/cpluslogger/include -I../../../cplusutil/cplusutil/include `pkg-config --static --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/features.o ../src/features.cpp

${OBJECTDIR}/_ext/511e4115/parameter.o: ../src/parameter.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include/opencv2 -I../../../cpluslogger/cpluslogger/include -I../../../cplusutil/cplusutil/include `pkg-config --static --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/parameter.o ../src/parameter.cpp

${OBJECTDIR}/_ext/511e4115/videobase.o: ../src/videobase.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include/opencv2 -I../../../cpluslogger/cpluslogger/include -I../../../cplusutil/cplusutil/include `pkg-config --static --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/videobase.o ../src/videobase.cpp

${OBJECTDIR}/_ext/511e4115/xtractor.o: ../src/xtractor.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include/opencv2 -I../../../cpluslogger/cpluslogger/include -I../../../cplusutil/cplusutil/include `pkg-config --static --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/xtractor.o ../src/xtractor.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
