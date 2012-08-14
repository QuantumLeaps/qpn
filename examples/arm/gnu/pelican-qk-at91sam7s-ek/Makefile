##############################################################################
# Product: QDK-nano_ARM-GNU_AT91SAM7S-EK
# Last Updated for Version: 4.3.00
# Date of the Last Update:  Nov 09, 2011
#
#                    Q u a n t u m     L e a P s
#                    ---------------------------
#                    innovating embedded systems
#
# Copyright (C) 2002-2011 Quantum Leaps, LLC. All rights reserved.
#
# This software may be distributed and modified under the terms of the GNU
# General Public License version 2 (GPL) as published by the Free Software
# Foundation and appearing in the file GPL.TXT included in the packaging of
# this file. Please note that GPL Section 2[b] requires that all works based
# on this software must also be made publicly available under the terms of
# the GPL ("Copyleft").
#
# Alternatively, this software may be distributed and modified under the
# terms of Quantum Leaps commercial licenses, which expressly supersede
# the GPL and are specifically designed for licensees interested in
# retaining the proprietary status of their code.
#
# Contact information:
# Quantum Leaps Web site:  http://www.quantum-leaps.com
# e-mail:                  info@quantum-leaps.com
##############################################################################
# examples of invoking this Makefile:
# building configurations: Debug (default), Release, and Spy
# make
# make CONF=rel
# make CONF=spy
#
# cleaning configurations: Debug (default), Release, and Spy
# make clean
# make CONF=rel clean
# make CONF=spy clean

#-----------------------------------------------------------------------------
# NOTE: the Makefile expects that the QPCPP environment variable is defined
# and points to the QP/C installation directory
#
ifndef QPN
$(error The QPN environment variable must be defined)
endif

#-----------------------------------------------------------------------------
# tools
#
ifeq ($(GNU_ARM),)
GNU_ARM = C:/tools/devkitPro/devkitARM/bin
endif

CC    := $(GNU_ARM)/arm-eabi-gcc
CPP   := $(GNU_ARM)/arm-eabi-g++
ASM   := $(GNU_ARM)/arm-eabi-as
LINK  := $(GNU_ARM)/arm-eabi-gcc
BIN   := $(GNU_ARM)/arm-eabi-objcopy
RM    := rm -rf
MKDIR := mkdir

QP_INCDIR = $(QPN)/include
QP_SRCDIR = $(QPN)/source
QP_PRTDIR = $(QPN)/ports/arm/qk/gnu
BLDDIR    = .

CCINC   = -I$(QP_INCDIR) -I$(BLDDIR)
QP_DEP  = $(QP_INCDIR)\qassert.h \
	$(QP_INCDIR)\qepn.h \
	$(QP_INCDIR)\qfn.h \
	$(QP_INCDIR)\qkn.h \
	$(BLDDIR)\qpn_port.h

APP_DEP = $(BLDDIR)\bsp.h \
	$(BLDDIR)\pelican.h

APP_NAME = pelican-qk
ARM_CORE = arm7tdmi


ifeq (rel, $(CONF))       # Release configuration ............................

BINDIR   = rel

CCFLAGS = -c -mcpu=$(ARM_CORE) -mthumb-interwork -Os \
	-mlong-calls -ffunction-sections -Wall -DNDBEBUG -o$@

ASMFLAGS = -mcpu=$(ARM_CORE) -mthumb-interwork -o$@

LINKFLAGS = -T .\$(APP_NAME).ld -o $(BINDIR)\$(APP_NAME).elf \
	-Wl,-Map,$(BINDIR)\$(APP_NAME).map,--cref,--gc-sections

else                     # default Debug configuration .......................

BINDIR   = dbg

CCFLAGS = -g -c -mcpu=$(ARM_CORE) -mthumb-interwork -O \
	-mlong-calls -ffunction-sections -Wall -o$@

ASMFLAGS = -g -mcpu=$(ARM_CORE) -mthumb-interwork -o$@

LINKFLAGS = -T .\$(APP_NAME).ld -o $(BINDIR)\$(APP_NAME).elf \
	-Wl,-Map,$(BINDIR)\$(APP_NAME).map,--cref,--gc-sections

endif

#.............................................................................

all: $(BINDIR)\$(APP_NAME).elf

$(BINDIR)\$(APP_NAME).elf : \
	.\$(APP_NAME).ld \
	$(BINDIR)\startup.o \
	$(BINDIR)\low_level_init.o \
	$(BINDIR)\main.o \
	$(BINDIR)\bsp.o \
	$(BINDIR)\isr.o \
	$(BINDIR)\pelican.o \
	$(BINDIR)\ped.o \
	$(BINDIR)\qepn.o \
	$(BINDIR)\qfn.o \
	$(BINDIR)\qkn.o \
	$(BINDIR)\qkn_port.o
	$(LINK) \
	$(BINDIR)\startup.o \
	$(BINDIR)\low_level_init.o \
	$(BINDIR)\main.o \
	$(BINDIR)\bsp.o \
	$(BINDIR)\isr.o \
	$(BINDIR)\pelican.o \
	$(BINDIR)\ped.o \
	$(BINDIR)\qepn.o \
	$(BINDIR)\qfn.o \
	$(BINDIR)\qkn.o \
	$(BINDIR)\qkn_port.o \
	$(LINKFLAGS)


$(BINDIR)\qkn_port.o: $(QP_PRTDIR)\qkn_port.s
	$(ASM) $(ASMFLAGS) $<

$(BINDIR)\startup.o: $(BLDDIR)\startup.s
	$(ASM) $(ASMFLAGS) $<

$(BINDIR)\low_level_init.o: $(BLDDIR)\low_level_init.c $(APP_DEP) $(QP_DEP)
	$(CC) -marm   $(CCFLAGS) $(CCINC) $<

$(BINDIR)\main.o: $(BLDDIR)\main.c $(APP_DEP) $(QP_DEP)
	$(CC) -mthumb $(CCFLAGS) $(CCINC) $<

$(BINDIR)\bsp.o: $(BLDDIR)\bsp.c $(APP_DEP) $(QP_DEP)
	$(CC) -marm   $(CCFLAGS) $(CCINC) $<

$(BINDIR)\isr.o: $(BLDDIR)\isr.c $(APP_DEP) $(QP_DEP)
	$(CC) -marm   $(CCFLAGS) $(CCINC) $<

$(BINDIR)\pelican.o: $(BLDDIR)\pelican.c $(APP_DEP) $(QP_DEP)
	$(CC) -mthumb $(CCFLAGS) $(CCINC) $<

$(BINDIR)\ped.o: $(BLDDIR)\ped.c $(APP_DEP) $(QP_DEP)
	$(CC) -mthumb $(CCFLAGS) $(CCINC) $<

$(BINDIR)\qepn.o: $(QP_SRCDIR)\qepn.c $(APP_DEP) $(QP_DEP)
	$(CC) -mthumb $(CCFLAGS) $(CCINC) $<

$(BINDIR)\qfn.o: $(QP_SRCDIR)\qfn.c $(APP_DEP) $(QP_DEP)
	$(CC) -marm   $(CCFLAGS) $(CCINC) $<

$(BINDIR)\qkn.o: $(QP_SRCDIR)\qkn.c $(APP_DEP) $(QP_DEP)
	$(CC) -marm   $(CCFLAGS) $(CCINC) $<


.PHONY : clean
clean:
	-$(RM) $(BINDIR)/*.o \
	$(BINDIR)/*.hex \
	$(BINDIR)/*.elf \
	$(BINDIR)/*.map
