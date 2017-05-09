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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/511e4115/defuse_info.o \
	${OBJECTDIR}/_ext/511e4115/dfs1parameter.o \
	${OBJECTDIR}/_ext/511e4115/dfs1xtractor.o \
	${OBJECTDIR}/_ext/511e4115/dfs2parameter.o \
	${OBJECTDIR}/_ext/511e4115/dfs2xtractor.o \
	${OBJECTDIR}/_ext/511e4115/evaluatedquery.o \
	${OBJECTDIR}/_ext/511e4115/features.o \
	${OBJECTDIR}/_ext/511e4115/featuresignatures.o \
	${OBJECTDIR}/_ext/511e4115/gd1parameter.o \
	${OBJECTDIR}/_ext/511e4115/mahalanobis1.o \
	${OBJECTDIR}/_ext/511e4115/minkowski1.o \
	${OBJECTDIR}/_ext/511e4115/mohist1parameter.o \
	${OBJECTDIR}/_ext/511e4115/mohist1xtractor.o \
	${OBJECTDIR}/_ext/511e4115/motionhistogram.o \
	${OBJECTDIR}/_ext/511e4115/parameter.o \
	${OBJECTDIR}/_ext/511e4115/resultbase.o \
	${OBJECTDIR}/_ext/511e4115/samplepoints.o \
	${OBJECTDIR}/_ext/511e4115/sfs1parameter.o \
	${OBJECTDIR}/_ext/511e4115/sfs1xtractor.o \
	${OBJECTDIR}/_ext/511e4115/smd1.o \
	${OBJECTDIR}/_ext/511e4115/sqfd1.o \
	${OBJECTDIR}/_ext/511e4115/videobase.o \
	${OBJECTDIR}/_ext/511e4115/xtractor.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnb.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnb.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnb.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnb.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnb.a

${OBJECTDIR}/_ext/511e4115/defuse_info.o: ../src/defuse_info.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/defuse_info.o ../src/defuse_info.cpp

${OBJECTDIR}/_ext/511e4115/dfs1parameter.o: ../src/dfs1parameter.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/dfs1parameter.o ../src/dfs1parameter.cpp

${OBJECTDIR}/_ext/511e4115/dfs1xtractor.o: ../src/dfs1xtractor.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/dfs1xtractor.o ../src/dfs1xtractor.cpp

${OBJECTDIR}/_ext/511e4115/dfs2parameter.o: ../src/dfs2parameter.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/dfs2parameter.o ../src/dfs2parameter.cpp

${OBJECTDIR}/_ext/511e4115/dfs2xtractor.o: ../src/dfs2xtractor.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/dfs2xtractor.o ../src/dfs2xtractor.cpp

${OBJECTDIR}/_ext/511e4115/evaluatedquery.o: ../src/evaluatedquery.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/evaluatedquery.o ../src/evaluatedquery.cpp

${OBJECTDIR}/_ext/511e4115/features.o: ../src/features.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/features.o ../src/features.cpp

${OBJECTDIR}/_ext/511e4115/featuresignatures.o: ../src/featuresignatures.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/featuresignatures.o ../src/featuresignatures.cpp

${OBJECTDIR}/_ext/511e4115/gd1parameter.o: ../src/gd1parameter.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/gd1parameter.o ../src/gd1parameter.cpp

${OBJECTDIR}/_ext/511e4115/mahalanobis1.o: ../src/mahalanobis1.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/mahalanobis1.o ../src/mahalanobis1.cpp

${OBJECTDIR}/_ext/511e4115/minkowski1.o: ../src/minkowski1.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/minkowski1.o ../src/minkowski1.cpp

${OBJECTDIR}/_ext/511e4115/mohist1parameter.o: ../src/mohist1parameter.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/mohist1parameter.o ../src/mohist1parameter.cpp

${OBJECTDIR}/_ext/511e4115/mohist1xtractor.o: ../src/mohist1xtractor.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/mohist1xtractor.o ../src/mohist1xtractor.cpp

${OBJECTDIR}/_ext/511e4115/motionhistogram.o: ../src/motionhistogram.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/motionhistogram.o ../src/motionhistogram.cpp

${OBJECTDIR}/_ext/511e4115/parameter.o: ../src/parameter.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/parameter.o ../src/parameter.cpp

${OBJECTDIR}/_ext/511e4115/resultbase.o: ../src/resultbase.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/resultbase.o ../src/resultbase.cpp

${OBJECTDIR}/_ext/511e4115/samplepoints.o: ../src/samplepoints.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/samplepoints.o ../src/samplepoints.cpp

${OBJECTDIR}/_ext/511e4115/sfs1parameter.o: ../src/sfs1parameter.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/sfs1parameter.o ../src/sfs1parameter.cpp

${OBJECTDIR}/_ext/511e4115/sfs1xtractor.o: ../src/sfs1xtractor.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/sfs1xtractor.o ../src/sfs1xtractor.cpp

${OBJECTDIR}/_ext/511e4115/smd1.o: ../src/smd1.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/smd1.o ../src/smd1.cpp

${OBJECTDIR}/_ext/511e4115/sqfd1.o: ../src/sqfd1.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/sqfd1.o ../src/sqfd1.cpp

${OBJECTDIR}/_ext/511e4115/videobase.o: ../src/videobase.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/videobase.o ../src/videobase.cpp

${OBJECTDIR}/_ext/511e4115/xtractor.o: ../src/xtractor.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/xtractor.o ../src/xtractor.cpp

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
