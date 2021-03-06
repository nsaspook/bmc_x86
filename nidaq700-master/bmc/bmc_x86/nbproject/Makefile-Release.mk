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
	${OBJECTDIR}/_ext/5c0/bmc.o \
	${OBJECTDIR}/_ext/511e00a9/daq.o \
	${OBJECTDIR}/bmcnet.o


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
LDLIBSOPTIONS=`pkg-config --libs comedilib` `pkg-config --libs xaw7`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bmc_x86

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bmc_x86: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bmc_x86 ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/5c0/bmc.o: ../bmc.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -O3 `pkg-config --cflags comedilib` `pkg-config --cflags xaw7`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/bmc.o ../bmc.c

${OBJECTDIR}/_ext/511e00a9/daq.o: ../bmc/daq.c
	${MKDIR} -p ${OBJECTDIR}/_ext/511e00a9
	${RM} "$@.d"
	$(COMPILE.c) -O3 `pkg-config --cflags comedilib` `pkg-config --cflags xaw7`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e00a9/daq.o ../bmc/daq.c

${OBJECTDIR}/bmcnet.o: bmcnet.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O3 `pkg-config --cflags comedilib` `pkg-config --cflags xaw7`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bmcnet.o bmcnet.c

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
