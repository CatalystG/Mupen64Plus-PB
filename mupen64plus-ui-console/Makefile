#/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# *   Mupen64plus - Makefile                                                *
# *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
# *   Copyright (C) 2009 Richard42                                          *
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
# Makefile for Mupen64plus-ui-console

CPU = ARM
OS = LINUX

# source and output dirs
SRCDIR ?= .
OBJDIR = _obj

# base CFLAGS, LDLIBS, and LDFLAGS
OPTFLAGS ?= -O3 -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp
CFLAGS += $(OPTFLAGS) -Wall -ffast-math -fno-strict-aliasing -fvisibility=hidden -I$(SRCDIR)
CXXFLAGS += -fvisibility-inlines-hidden
#LDFLAGS += $(SHARED)

ifeq ("$(CPU)","ARM")
  NO_ASM := 1
  #PIC := 1
endif

#CFLAGS += -fPIE
#LDFLAGS += -fPIE -pie

ifeq ($(BIG_ENDIAN), 1)
  CFLAGS += -DM64P_BIG_ENDIAN
endif

# tweak flags for 32-bit build on 64-bit system
ifeq ($(ARCH_DETECTED), 64BITS_32)
  ifeq ($(OS), FREEBSD)
    $(error Do not use the BITS=32 option with FreeBSD, use -m32 and -m elf_i386)
  endif
  CFLAGS += -m32
  LDFLAGS += -m32 -Wl,-m,elf_i386
endif

# set special flags per-system
ifeq ($(OS), LINUX)
  #LDLIBS += -ldl
  ifeq ($(CPU),ARM)
    #headers for palm, sdl
    CFLAGS += -I../blackberry-SDL/include -I../blackberry-SDL_image/ -I../blackberry-SDL_ttf/ 
    #todo add libpng header dir
    #CFLAGS += -DGLES_2 -DPRE -DTOUCHPAD_UI -DNO_ASM -D__arm__
    CFLAGS += -DGLES_2 -DPRE -DNO_ASM -D__arm__ -DTOUCHPAD_UI
    LDLIBS +=  -L../blackberry-SDL/Device-Debug -L../blackberry-SDL_image/Device-Debug -L../blackberry-SDL_ttf/Device-Debug -lGLESv2 -lpng -lSDL12 -lz -lSDL_ttf -lSDL_image -L../../blackberry-TouchControlOverlay-ddadafc/Device-Release -ltco -L../bbutil/arm/so.le-v7/ -lbbutil
    #LDFLAGS += -allow-shlib-undefined
  endif
endif
ifeq ($(OS), OSX)
  ifeq ($(CPU), X86)
    ifeq ($(ARCH_DETECTED), 64BITS)
      CFLAGS += -pipe -arch x86_64 -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk
      LDFLAGS += -bundle -framework OpenGL -arch x86_64
      LDLIBS += -ldl
    else
      CFLAGS += -pipe -mmmx -msse -fomit-frame-pointer -arch i686 -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk
      LDFLAGS += -bundle -framework OpenGL -arch i686
      LDLIBS += -ldl
    endif
  endif
endif

# set mupen64plus core API header path
ifneq ("$(APIDIR)","")
  CFLAGS += "-I$(APIDIR)"
else
  TRYDIR = ../mupen64plus-core/api
  ifneq ("$(wildcard $(TRYDIR)/m64p_types.h)","")
    CFLAGS += -I$(TRYDIR)
  else
    TRYDIR = /usr/local/include/mupen64plus
    ifneq ("$(wildcard $(TRYDIR)/m64p_types.h)","")
      CFLAGS += -I$(TRYDIR)
    else
      TRYDIR = /usr/include/mupen64plus
      ifneq ("$(wildcard $(TRYDIR)/m64p_types.h)","")
        CFLAGS += -I$(TRYDIR)
      else
        $(error Mupen64Plus API header files not found! Use makefile parameter APIDIR to force a location.)
      endif
    endif
  endif
endif

CXX := C:/bbndk-2.1.0-beta1/host/win32/x86/usr/bin/qcc -V4.4.2,gcc_ntoarmv7le_cpp
RM       ?= rm -f
INSTALL  ?= install
MKDIR ?= mkdir -p
COMPILE.c =$(Q_CC)$(CXX) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
COMPILE.cc = $(Q_CXX)$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
LINK.o = $(Q_LD)$(CXX) $(LDFLAGS) $(TARGET_ARCH)

# set special flags for given Makefile parameters
ifeq ($(DEBUG),1)
  CFLAGS += -g
  INSTALL_STRIP_FLAG ?= 
else
  INSTALL_STRIP_FLAG ?= -s
endif
ifeq ($(NO_ASM), 1)
  CFLAGS += -DNO_ASM
endif

# set installation options
ifeq ($(PREFIX),)
  PREFIX := /usr/local
endif
ifeq ($(SHAREDIR),)
  SHAREDIR := $(PREFIX)/share/mupen64plus
endif
ifeq ($(LIBDIR),)
  LIBDIR := $(PREFIX)/lib
endif
ifeq ($(PLUGINDIR),)
  PLUGINDIR := $(LIBDIR)/mupen64plus
endif

# list of source files to compile
SOURCE = \
	$(SRCDIR)/cheat.c \
	$(SRCDIR)/compare_core.c \
	$(SRCDIR)/core_interface.c \
	$(SRCDIR)/main.c \
	$(SRCDIR)/plugin.c \
	$(SRCDIR)/osal_dynamiclib_unix.c \
	$(SRCDIR)/osal_files_unix.c \
	

# generate a list of object files build, make a temporary directory for them
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(filter %.c, $(SOURCE)))
OBJDIRS = $(dir $(OBJECTS))
$(shell $(MKDIR) $(OBJDIRS))

# build targets
TARGET = mupen64plus-ui-console

targets:
	@echo "Mupen64Plus-ui-console makefile."
	@echo "  Targets:"
	@echo "    all            == Build Mupen64Plus console front-end application"
	@echo "    clean          == remove object files and build products"
	@echo "    rebuild        == clean and re-build all"
	@echo "    install        == Install Mupen64Plus console front-end application"
	@echo "    uninstall      == Uninstall Mupen64Plus console front-end application"
	@echo "  Options:"
	@echo "    COREDIR=path   == default path to search for Mupen64Plus Core (must end with slash)"
	@echo "    PLUGINDIR=path == default path to search for plugins"
	@echo "    SHAREDIR=path  == default path to search for shared data files"
	@echo "    APIDIR=path    == path to find Mupen64Plus Core headers"
	@echo "    OPTFLAGS=flags == compiler optimization (default: -O3)"
	@echo "    PIE=(1|0)      == Force enable/disable of position independent executables"
	@echo "  Install Options:"
	@echo "    PREFIX=path    == install/uninstall prefix (default: /usr/local/)"
	@echo "    BINDIR=path    == path to install mupen64plus binary (default: PREFIX/bin/)"
	@echo "    MANDIR=path    == path to install mupen64plus manual page (default: PREFIX/share/man)"
	@echo "    DESTDIR=path   == path to prepend to all installation paths (only for packagers)"
	@echo "  Debugging Options:"
	@echo "    DEBUG=1        == add debugging symbols to application binary"
	@echo "    V=1            == show verbose compiler output"

all: $(TARGET)

clean:
	$(RM) -r ./_obj $(TARGET)

rebuild: clean all

install: $(TARGET)
	$(INSTALL) -d "$(DESTDIR)$(BINDIR)"
	$(INSTALL) -m 0755 $(INSTALL_STRIP_FLAG) $(TARGET) "$(DESTDIR)$(BINDIR)"
	$(INSTALL) -d "$(DESTDIR)$(MANDIR)/man6"
	$(INSTALL) -m 0644 ../../doc/mupen64plus.6.gz "$(DESTDIR)$(MANDIR)/man6"

uninstall:
	$(RM) "$(DESTDIR)$(BINDIR)/$(TARGET)" "$(DESTDIR)$(MANDIR)/man6/mupen64plus.6.gz"

# build dependency files
CFLAGS += -MD
-include $(OBJECTS:.o=.d)

# standard build rules
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILE.c) -o $@ $<

$(TARGET): $(OBJECTS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

.PHONY: all clean install uninstall targets
