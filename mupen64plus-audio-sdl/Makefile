#/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# *   Mupen64plus-audio-sdl - Makefile                                      *
# *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
# *   Copyright (C) 2007-2009 Richard Goedeken                              *
# *   Copyright (C) 2007-2008 Tillin9                                       *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
# Makefile for SDL Audio plugin in Mupen64plus

# detect operation system
OS = LINUX
SHARED = -shared
SO_EXTENSION = so


# detect system architecture
HOST_CPU ?= $(shell uname -m)
NO_ASM ?= 1
CPU := NONE
PIC=1

# base CFLAGS, LDLIBS, and LDFLAGS
OPTFLAGS ?= -O3
CFLAGS += $(OPTFLAGS) -ffast-math -fno-strict-aliasing -fvisibility=hidden -I../../src
LDFLAGS += $(SHARED)

CFLAGS += -fPIC
LDFLAGS += -fPIC

CFLAGS  += -I../blackberry-SDL/include
LDLIBS += -L../blackberry-SDL/Device-Debug -lSDL12

CFLAGS  += -I..//mupen64plus-core/api

# set base program pointers and flags
CC_OVERRIDE = C:/bbndk-2.1.0-beta1/host/win32/x86/usr/bin/qcc -V4.4.2,gcc_ntoarmv7le_cpp -w1 -shared
CC = $(CC_OVERRIDE)
RM       ?= rm -f
INSTALL  ?= install
MKDIR ?= mkdir -p
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS)  -c
LINK.o = $(CC) $(LDFLAGS)

# list of source files to compile
SOURCE = \
	main.c \
	volume.c \
	osal_dynamiclib_unix.c

# generate a list of object files build, make a temporary directory for them
#OBJDIRS = _obj
OBJDIRS = .
$(shell $(MKDIR) $(OBJDIRS))
OBJECTS := $(OBJDIRS)/main.o $(OBJDIRS)/volume.o $(OBJDIRS)/osal_dynamiclib_unix.o

# build dependency files
CFLAGS += -MD
-include $(OBJECTS:.o=.d)

# build targets
TARGET = ../libs/mupen64plus-audio-sdl.$(SO_EXTENSION)

targets:
	@echo "Mupen64Plus-audio-sdl makefile. "
	@echo "  Targets:"
	@echo "    all           == Build Mupen64Plus SDL audio plugin"
	@echo "    clean         == remove object files"
	@echo "    rebuild       == clean and re-build all"
	@echo "    install       == Install Mupen64Plus SDL audio plugin"
	@echo "    uninstall     == Uninstall Mupen64Plus SDL audio plugin"
	@echo "  Options:"
	@echo "    BITS=32       == build 32-bit binaries on 64-bit machine"
	@echo "    APIDIR=path   == path to find Mupen64Plus Core headers"
	@echo "    OPTFLAGS=flag == compiler optimization (default: -O3)"
	@echo "    PIC=(1|0)     == Force enable/disable of position independent code"
	@echo "    NO_RESAMP=1   == build without libsamplerate; disables high-quality audio resampling"
	@echo "  Install Options:"
	@echo "    PREFIX=path   == install/uninstall prefix (default: /usr/local)"
	@echo "    LIBDIR=path   == library prefix (default: PREFIX/lib)"
	@echo "    PLUGINDIR=path == path to install plugin libraries (default: LIBDIR/mupen64plus)"
	@echo "    DESTDIR=path  == path to prepend to all installation paths (only for packagers)"
	@echo "  Debugging Options:"
	@echo "    DEBUG=1       == add debugging symbols"
	@echo "    V=1           == show verbose compiler output"


all: $(TARGET)

install: $(TARGET)
	$(INSTALL) -d "$(DESTDIR)$(PLUGINDIR)"
	$(INSTALL) -m 0644 $(INSTALL_STRIP_FLAG) $(TARGET) "$(DESTDIR)$(PLUGINDIR)"

uninstall:
	$(RM) "$(DESTDIR)$(PLUGINDIR)/$(TARGET)"

clean:
	$(RM) *.o $(TARGET)

rebuild: clean all

# standard build rules
$(OBJDIRS)/%.o: $(SRCDIR)/%.c
	$(COMPILE.c) -o $@ $<

$(TARGET): $(OBJECTS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

.PHONY: all clean install uninstall targets
