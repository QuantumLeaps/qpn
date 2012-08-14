##############################################################################
# Product: QDK-nano AVR-GNU (WinAVR), Arduino-UNO
# Last Updated for Version: 4.1.05
# Date of the Last Update:  Dec 08, 2010
#
#                    Q u a n t u m     L e a P s
#                    ---------------------------
#                    innovating embedded systems
#
# Copyright (C) 2002-2010 Quantum Leaps, LLC. All rights reserved.
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
# general utilities
RM = erase

# GNU-AVR tools directory
GNU_AVR = C:\tools\arduino-0021\hardware\tools\avr

# Adjust the Target MCU to your project
TARGET_MCU = atmega328p

# Adjust the Application Name to your project
APP_NAME = pelican-arduino-uno

# The gcc compiler and linker
CC      = $(GNU_AVR)\bin\avr-gcc
LINK    = $(GNU_AVR)\bin\avr-gcc
OBJCOPY = $(GNU_AVR)\bin\avr-objcopy
OBJDUMP = $(GNU_AVR)\bin\avr-objdump

QP_INCDIR = ..\..\..\..\include
QP_SRCDIR = ..\..\..\..\source
BLDDIR    = .

CCINC     := -I$(QP_INCDIR) -I$(BLDDIR)
QP_DEP    := $(QP_INCDIR)\qassert.h \
	$(QP_INCDIR)\qepn.h \
	$(QP_INCDIR)\qfn.h \
	$(BLDDIR)\qpn_port.h

APP_DEP   := $(BLDDIR)\bsp.h \
	$(BLDDIR)\pelican.h


# dbg (default target) .......................................................

BINDIR   = dbg

# gcc options
# -c    : compile only
# -g    : source level debugging
CCFLAGS  = -c -gdwarf-2 -std=gnu99 -Os -fsigned-char -fshort-enums \
 -mmcu=$(TARGET_MCU) -Wall -o$@

LINKFLAGS = -gdwarf-2 -Os -mmcu=$(TARGET_MCU)


# release ....................................................................
ifeq ($(MAKECMDGOALS), rel)

BINDIR   = rel

# gcc options
# -c    : compile only
# -O    : optimize
CCFLAGS  = -c -DNDEBUG -std=gnu99 -Os -fsigned-char -fshort-enums \
 -mmcu=$(TARGET_MCU) -Wall -o$@

LINKFLAGS =	-O2 -mmcu=$(TARGET_MCU)

endif
ifeq ($(MAKECMDGOALS), rel_clean)

BINDIR   = rel

endif

#.............................................................................

all: $(BINDIR)\$(APP_NAME).hex

$(BINDIR)\$(APP_NAME).hex : $(BINDIR)\$(APP_NAME).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(BINDIR)\$(APP_NAME).elf : \
	$(BINDIR)\bsp.o \
	$(BINDIR)\main.o \
	$(BINDIR)\ped.o \
	$(BINDIR)\pelican.o \
	$(BINDIR)\qepn.o \
	$(BINDIR)\qfn.o
	$(LINK) $(LINKFLAGS) \
	-o $(BINDIR)\$(APP_NAME).elf \
	-Wl,-Map,$(BINDIR)\$(APP_NAME).map,--cref \
	$(BINDIR)\bsp.o \
	$(BINDIR)\main.o \
	$(BINDIR)\ped.o \
	$(BINDIR)\pelican.o \
	$(BINDIR)\qepn.o \
	$(BINDIR)\qfn.o

$(BINDIR)\main.o: $(BLDDIR)\main.c $(APP_DEP) $(QP_DEP)
	$(CC) $(CCFLAGS) $(CCINC) $<

$(BINDIR)\bsp.o: $(BLDDIR)\bsp.c $(APP_DEP) $(QP_DEP)
	$(CC) $(CCFLAGS) $(CCINC) $<

$(BINDIR)\pelican.o: $(BLDDIR)\pelican.c $(APP_DEP) $(QP_DEP)
	$(CC) $(CCFLAGS) $(CCINC) $<

$(BINDIR)\ped.o: $(BLDDIR)\ped.c $(APP_DEP) $(QP_DEP)
	$(CC) $(CCFLAGS) $(CCINC) $<

$(BINDIR)\qepn.o: $(QP_SRCDIR)\qepn.c $(QP_DEP)
	$(CC) $(CCFLAGS) $(CCINC) $<

$(BINDIR)\qfn.o: $(QP_SRCDIR)\qfn.c $(QP_DEP)
	$(CC) $(CCFLAGS) $(CCINC) $<



dbg: all

rel: all


# clean targets...

.PHONY: clean rel_clean spy_clean

rel_clean: clean

dbg_clean: clean

clean:
	-$(RM) $(BINDIR)\*.o
	-$(RM) $(BINDIR)\*.elf
	-$(RM) $(BINDIR)\*.hex
	-$(RM) $(BINDIR)\*.map
