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
	${OBJECTDIR}/_ext/5c0/de_xtraction.o


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
LDLIBSOPTIONS=-L/usr/local/lib ../../../libs/cpluslogger/build/staticlib/cpluslogger_1_0.a ../../../libs/cplusutil/build/staticlib/cplusutil_1_0.a ../../../defuse/build/staticlib/defuse_2_0.a `pkg-config --static --libs opencv` -lboost_filesystem -lboost_system  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nb

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nb: ../../../libs/cpluslogger/build/staticlib/cpluslogger_1_0.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nb: ../../../libs/cplusutil/build/staticlib/cplusutil_1_0.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nb: ../../../defuse/build/staticlib/defuse_2_0.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nb: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/nb ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/5c0/de_xtraction.o: ../de_xtraction.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include/opencv2 -I/usr/local/include/opencv -I../../../defuse/include -I../../../../cpluslogger/cpluslogger/include -I../../../../cplusutil/cplusutil/include `pkg-config --static --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/de_xtraction.o ../de_xtraction.cpp

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