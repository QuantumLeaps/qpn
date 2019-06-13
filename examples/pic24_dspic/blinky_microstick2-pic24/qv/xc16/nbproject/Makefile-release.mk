#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-release.mk)" "nbproject/Makefile-local-release.mk"
include nbproject/Makefile-local-release.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=release
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/xc16.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/xc16.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../../../../src/qfn/qepn.c ../../../../../src/qfn/qfn.c ../../../../../src/qvn/qvn.c bsp.c ../../../../../include/qstamp.c ../../main.c ../../blinky.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/329161665/qepn.o ${OBJECTDIR}/_ext/329161665/qfn.o ${OBJECTDIR}/_ext/329161169/qvn.o ${OBJECTDIR}/bsp.o ${OBJECTDIR}/_ext/726959463/qstamp.o ${OBJECTDIR}/_ext/43898991/main.o ${OBJECTDIR}/_ext/43898991/blinky.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/329161665/qepn.o.d ${OBJECTDIR}/_ext/329161665/qfn.o.d ${OBJECTDIR}/_ext/329161169/qvn.o.d ${OBJECTDIR}/bsp.o.d ${OBJECTDIR}/_ext/726959463/qstamp.o.d ${OBJECTDIR}/_ext/43898991/main.o.d ${OBJECTDIR}/_ext/43898991/blinky.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/329161665/qepn.o ${OBJECTDIR}/_ext/329161665/qfn.o ${OBJECTDIR}/_ext/329161169/qvn.o ${OBJECTDIR}/bsp.o ${OBJECTDIR}/_ext/726959463/qstamp.o ${OBJECTDIR}/_ext/43898991/main.o ${OBJECTDIR}/_ext/43898991/blinky.o

# Source Files
SOURCEFILES=../../../../../src/qfn/qepn.c ../../../../../src/qfn/qfn.c ../../../../../src/qvn/qvn.c bsp.c ../../../../../include/qstamp.c ../../main.c ../../blinky.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-release.mk dist/${CND_CONF}/${IMAGE_TYPE}/xc16.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ64GB002
MP_LINKER_FILE_OPTION=,--script=p24FJ64GB002.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/329161665/qepn.o: ../../../../../src/qfn/qepn.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/329161665" 
	@${RM} ${OBJECTDIR}/_ext/329161665/qepn.o.d 
	@${RM} ${OBJECTDIR}/_ext/329161665/qepn.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../src/qfn/qepn.c  -o ${OBJECTDIR}/_ext/329161665/qepn.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/329161665/qepn.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/329161665/qepn.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/329161665/qfn.o: ../../../../../src/qfn/qfn.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/329161665" 
	@${RM} ${OBJECTDIR}/_ext/329161665/qfn.o.d 
	@${RM} ${OBJECTDIR}/_ext/329161665/qfn.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../src/qfn/qfn.c  -o ${OBJECTDIR}/_ext/329161665/qfn.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/329161665/qfn.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/329161665/qfn.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/329161169/qvn.o: ../../../../../src/qvn/qvn.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/329161169" 
	@${RM} ${OBJECTDIR}/_ext/329161169/qvn.o.d 
	@${RM} ${OBJECTDIR}/_ext/329161169/qvn.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../src/qvn/qvn.c  -o ${OBJECTDIR}/_ext/329161169/qvn.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/329161169/qvn.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/329161169/qvn.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/bsp.o: bsp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/bsp.o.d 
	@${RM} ${OBJECTDIR}/bsp.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  bsp.c  -o ${OBJECTDIR}/bsp.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/bsp.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/bsp.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/726959463/qstamp.o: ../../../../../include/qstamp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/726959463" 
	@${RM} ${OBJECTDIR}/_ext/726959463/qstamp.o.d 
	@${RM} ${OBJECTDIR}/_ext/726959463/qstamp.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../include/qstamp.c  -o ${OBJECTDIR}/_ext/726959463/qstamp.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/726959463/qstamp.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/726959463/qstamp.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/43898991/main.o: ../../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/43898991" 
	@${RM} ${OBJECTDIR}/_ext/43898991/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/43898991/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../main.c  -o ${OBJECTDIR}/_ext/43898991/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/43898991/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/43898991/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/43898991/blinky.o: ../../blinky.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/43898991" 
	@${RM} ${OBJECTDIR}/_ext/43898991/blinky.o.d 
	@${RM} ${OBJECTDIR}/_ext/43898991/blinky.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../blinky.c  -o ${OBJECTDIR}/_ext/43898991/blinky.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/43898991/blinky.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/43898991/blinky.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/329161665/qepn.o: ../../../../../src/qfn/qepn.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/329161665" 
	@${RM} ${OBJECTDIR}/_ext/329161665/qepn.o.d 
	@${RM} ${OBJECTDIR}/_ext/329161665/qepn.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../src/qfn/qepn.c  -o ${OBJECTDIR}/_ext/329161665/qepn.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/329161665/qepn.o.d"        -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/329161665/qepn.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/329161665/qfn.o: ../../../../../src/qfn/qfn.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/329161665" 
	@${RM} ${OBJECTDIR}/_ext/329161665/qfn.o.d 
	@${RM} ${OBJECTDIR}/_ext/329161665/qfn.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../src/qfn/qfn.c  -o ${OBJECTDIR}/_ext/329161665/qfn.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/329161665/qfn.o.d"        -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/329161665/qfn.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/329161169/qvn.o: ../../../../../src/qvn/qvn.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/329161169" 
	@${RM} ${OBJECTDIR}/_ext/329161169/qvn.o.d 
	@${RM} ${OBJECTDIR}/_ext/329161169/qvn.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../src/qvn/qvn.c  -o ${OBJECTDIR}/_ext/329161169/qvn.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/329161169/qvn.o.d"        -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/329161169/qvn.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/bsp.o: bsp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/bsp.o.d 
	@${RM} ${OBJECTDIR}/bsp.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  bsp.c  -o ${OBJECTDIR}/bsp.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/bsp.o.d"        -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/bsp.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/726959463/qstamp.o: ../../../../../include/qstamp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/726959463" 
	@${RM} ${OBJECTDIR}/_ext/726959463/qstamp.o.d 
	@${RM} ${OBJECTDIR}/_ext/726959463/qstamp.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../include/qstamp.c  -o ${OBJECTDIR}/_ext/726959463/qstamp.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/726959463/qstamp.o.d"        -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/726959463/qstamp.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/43898991/main.o: ../../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/43898991" 
	@${RM} ${OBJECTDIR}/_ext/43898991/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/43898991/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../main.c  -o ${OBJECTDIR}/_ext/43898991/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/43898991/main.o.d"        -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/43898991/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/43898991/blinky.o: ../../blinky.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/43898991" 
	@${RM} ${OBJECTDIR}/_ext/43898991/blinky.o.d 
	@${RM} ${OBJECTDIR}/_ext/43898991/blinky.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../blinky.c  -o ${OBJECTDIR}/_ext/43898991/blinky.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/43898991/blinky.o.d"        -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Os -I"../.." -I"../../../../../include" -I"../../../../../src" -I"../../../../../ports/pic24_dspic/qv/xc16" -DNDEBUG -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/43898991/blinky.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/xc16.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/xc16.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x800:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x825 -mreserve=data@0x826:0x84F   -Wl,,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/xc16.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/xc16.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_release=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Wl,,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/xc16.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/release
	${RM} -r dist/release

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
