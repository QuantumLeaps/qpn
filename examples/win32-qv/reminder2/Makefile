##############################################################################
# Product: Makefile for QP-nano, Reminder2 pattern console, Win32-QV, MinGW
# Last updated for version 5.6.4
# Last updated on  2016-05-04
#
#                    Q u a n t u m     L e a P s
#                    ---------------------------
#                    innovating embedded systems
#
# Copyright (C) Quantum Leaps, LLC. All rights reserved.
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
# http://www.state-machine.com
# mailto:info@state-machine.com
##############################################################################
# examples of invoking this Makefile:
# building configurations: Debug (default), Release, and Spy
# make
# make CONF=rel
#
# cleaning configurations: Debug (default), Release, and Spy
# make clean
# make CONF=rel clean
#
# NOTE:
# To use this Makefile on Windows, you will need the GNU make utility, which
# is included in the Qtools collection for Windows, see:
#    http://sourceforge.net/projects/qpc/files/Qtools/
#

#-----------------------------------------------------------------------------
# project name
#
PROJECT     := reminder2

#-----------------------------------------------------------------------------
# project directories
#

# location of the QP-nano framework (if not provided in an environemnt var.)
ifeq ($(QPN),)
QPN := ../../..
endif

# QP port used in this project
QP_PORT_DIR := $(QPN)/ports/win32-qv

# list of all source directories used by this project
VPATH = \
	.

# list of all include directories needed by this project
INCLUDES  = \
	-I. \
	-I$(QPN)/include

# list of resource include directories needed by this project
RCINCLUDES = \
	-IRes


#-----------------------------------------------------------------------------
# files
#

# C source files
C_SRCS := \
	bsp.c \
	main.c \
	reminder2.c

# C++ source files
CPP_SRCS :=	

# Resource files
RC_SRCS :=

QP_SRCS := \
	qepn.c \
	qfn_win32.c

LIB_DIRS  :=
LIBS      :=

# defines...
# QP_API_VERSION controls the QP API compatibility; 9999 means the latest API
DEFINES   := -DQP_API_VERSION=9999

#-----------------------------------------------------------------------------
# MinGW toolset (NOTE: assumed to be on your PATH)
#
# NOTE:
# MinGW toolset is included in the Qtools collection for Windows, see:
#    http://sourceforge.net/projects/qpc/files/Qtools/
#
# NOTE:
# This Makefile assumes that the windres utility is available on the
# PATH (NOTE: windres is available in the Qtools collection for Windows)
#
CC    := gcc
CPP   := g++
LINK  := gcc    # for C programs
#LINK  := g++   # for C++ programs
RC    := windres

# basic utilities (included in Qtools for Windows), see:
#    http://sourceforge.net/projects/qpc/files/Qtools

MKDIR := mkdir
RM    := rm

#-----------------------------------------------------------------------------
# build options for various configurations
#

ifeq (rel, $(CONF)) # Release configuration ..................................

BIN_DIR := rel

CFLAGS = -ffunction-sections -fdata-sections \
	-Os -fno-strict-aliasing -Wall -W $(INCLUDES) $(DEFINES) -DNDEBUG

CPPFLAGS = -ffunction-sections -fdata-sections \
	-Os -fno-strict-aliasing -Wall -W $(INCLUDES) $(DEFINES) -DNDEBUG

else  # default Debug configuration ..........................................

BIN_DIR := dbg

CFLAGS = -g -ffunction-sections -fdata-sections \
	-O -Wall -W $(INCLUDES) $(DEFINES)

CPPFLAGS = -g -ffunction-sections -fdata-sections \
	-O -Wall -W $(INCLUDES) $(DEFINES)

endif  # .....................................................................

LINKFLAGS := -Wl,-Map,$(BIN_DIR)/$(PROJECT).map,--cref,--gc-sections

# is it a GUI application? (any GUI resources provided?) ...
ifneq (,$(RC_SRCS))
LINKFLAGS += -mwindows
C_SRCS    += qwin_gui.c
DEFINES   += -DQWIN_GUI
endif

#-----------------------------------------------------------------------------

# combine all the soruces...
INCLUDES += -I$(QPN)/source -I$(QP_PORT_DIR)
VPATH    += $(QPN)/source $(QP_PORT_DIR)
C_SRCS   += $(QP_SRCS)

C_OBJS       := $(patsubst %.c,   %.o, $(C_SRCS))
CPP_OBJS     := $(patsubst %.cpp, %.o, $(CPP_SRCS))
RC_OBJS      := $(patsubst %.rc,  %.o, $(RC_SRCS))

TARGET_BIN   := $(BIN_DIR)/$(PROJECT).bin
TARGET_EXE   := $(BIN_DIR)/$(PROJECT).exe
C_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT   := $(patsubst %.o, %.d, $(C_OBJS_EXT))
CPP_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT := $(patsubst %.o, %.d, $(CPP_OBJS_EXT))
RC_OBJS_EXT  := $(addprefix $(BIN_DIR)/, $(RC_OBJS))

# create $(BIN_DIR) if it does not exist
ifeq ("$(wildcard $(BIN_DIR))","")
$(shell $(MKDIR) $(BIN_DIR))
endif

#-----------------------------------------------------------------------------
# rules
#

all: $(TARGET_EXE)
#all: $(TARGET_BIN)

$(TARGET_BIN): $(TARGET_EXE)
	$(BIN) -O binary $< $@

$(TARGET_EXE) : $(C_OBJS_EXT) $(CPP_OBJS_EXT) $(RC_OBJS_EXT)
	$(LINK) $(LINKFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.d : %.c
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

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
	@echo C_OBJS_EXT   = $(C_OBJS_EXT)
	@echo C_DEPS_EXT   = $(C_DEPS_EXT)
	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)
	@echo CPP_OBJS_EXT = $(CPP_OBJS_EXT)
	@echo RC_OBJS_EXT  = $(RC_OBJS_EXT)
	@echo LIB_DIRS = $(LIB_DIRS)
	@echo LIBS     = $(LIBS)
