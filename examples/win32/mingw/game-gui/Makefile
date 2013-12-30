##############################################################################
# Product: Generic Makefile for QP-nano GUI application, Win32-GUI, MinGW
# Last Updated for Version: 5.1.2
# Date of the Last Update:  Nov 04, 2013
#
#                    Q u a n t u m     L e a P s
#                    ---------------------------
#                    innovating embedded systems
#
# Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
#
# This program is open source software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Alternatively, this program may be distributed and modified under the
# terms of Quantum Leaps commercial licenses, which expressly supersede
# the GNU General Public License and are specifically designed for
# licensees interested in retaining the proprietary status of their code.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
# Contact information:
# Quantum Leaps Web sites: http://www.quantum-leaps.com
#                          http://www.state-machine.com
# e-mail:                  info@quantum-leaps.com
##############################################################################
# examples of invoking this Makefile:
# building configurations: Debug (default) and Release
# make
# make CONF=rel
#
# cleaning configurations: Debug (default) and Release
# make clean
# make CONF=rel clean
##############################################################################
#
# NOTE: Typically, you should have no need to change anything in this Makefile
#
##############################################################################

#-----------------------------------------------------------------------------
# NOTE: the Makefile expects that the QPN environment variable is defined
# and points to the QP-nano installation directory
#
ifndef QPN
$(error The QPN environment variable must be defined)
endif

#-----------------------------------------------------------------------------
# NOTE: this Makefile assumes that the Qtools\bin directory is added
# to the PATH variable.
#
CC    := gcc
CPP   := g++
LINK  := gcc                                                # for C programs
#LINK  := g++                                               # for C++ programs
RC    := windres
RM    := rm

#-----------------------------------------------------------------------------
# directories
#
# Project name is derived from the directory name
PROJECT := $(notdir $(CURDIR))

QP_PORT_DIR := $(QPN)/ports/win32
APP_DIR     := .

VPATH = $(APP_DIR) \
	$(QPN)/source \
	$(QP_PORT_DIR)

# include directories
INCLUDES  = -I. \
	-I$(QPN)/include	\
	-I$(QP_PORT_DIR)

# resource directory
RCINCLUDES = -IRes

# defines
DEFINES =

LINKFLAGS = -Wl,-Map,$(BIN_DIR)/$(PROJECT).map -s -mwindows

#-----------------------------------------------------------------------------
# files
#

# assembler source files
ASM_SRCS :=

# C source files
C_SRCS := $(wildcard *.c) \
    qepn.c \
    qfn_win32.c \
    win32_gui.c \

# C++ source files
CPP_SRCS := $(wildcard *.cpp)

# Resource files
RC_SRCS := $(wildcard *.rc)

#-----------------------------------------------------------------------------
# build options for various configurations
#

ifeq (rel, $(CONF))       # Release configuration ............................

BIN_DIR := rel

CFLAGS = -c -Wall -O1 $(INCLUDES) $(DEFINES) -DNDEBUG

CPPFLAGS = -c -Wall -O1 $(INCLUDES) $(DEFINES) -DNDEBUG

else                     # default Debug configuration .......................

BIN_DIR := dbg

CFLAGS = -c -Wall -g $(INCLUDES) $(DEFINES)

CPPFLAGS = -c -Wall -g $(INCLUDES) $(DEFINES)

endif

C_OBJS       := $(patsubst %.c,%.o,$(C_SRCS))
CPP_OBJS     := $(patsubst %.cpp,%.o,$(CPP_SRCS))
RC_OBJS      := $(patsubst %.rc,%.o,$(RC_SRCS))

TARGET_BIN   := $(BIN_DIR)/$(PROJECT).bin
TARGET_EXE   := $(BIN_DIR)/$(PROJECT).exe
C_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT   := $(patsubst %.o,%.d, $(C_OBJS_EXT))
CPP_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT := $(patsubst %.o,%.d, $(CPP_OBJS_EXT))
RC_OBJS_EXT  := $(addprefix $(BIN_DIR)/, $(RC_OBJS))

#-----------------------------------------------------------------------------
# rules
#

all: $(BIN_DIR) $(TARGET_EXE)
#all: $(BIN_DIR) $(TARGET_BIN)

$(BIN_DIR):
	@echo
	mkdir $@

$(TARGET_BIN): $(TARGET_EXE)
	$(BIN) -O binary $< $@

$(TARGET_EXE) : $(C_OBJS_EXT) $(CPP_OBJS_EXT) $(RC_OBJS_EXT)
	$(LINK) $(LINKFLAGS) -o $@ $^

$(BIN_DIR)/%.d : %.c
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.o : %.s
	$(AS) $(ASFLAGS) $< -o $@

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.rc
	$(RC) $(RCINCLUDES) -i $< -o $@

# include dependency files only if our goal depends on their existence
ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),show)
-include $(C_DEPS_EXT) $(CPP_DEPS_EXT)
  endif
endif

.PHONY : clean
clean:
	-$(RM) $(BIN_DIR)/*.o \
	$(BIN_DIR)/*.d \
	$(BIN_DIR)/*.exe \
	$(BIN_DIR)/*.map
	
show:
	@echo PROJECT  = $(PROJECT)
	@echo CONF     = $(CONF)
	@echo VPATH    = $(VPATH)
	@echo C_SRCS   = $(C_SRCS)
	@echo CPP_SRCS = $(CPP_SRCS)
	@echo ASM_OBJS_EXT = $(ASM_OBJS_EXT)
	@echo C_OBJS_EXT   = $(C_OBJS_EXT)
	@echo C_DEPS_EXT   = $(C_DEPS_EXT)
	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)
