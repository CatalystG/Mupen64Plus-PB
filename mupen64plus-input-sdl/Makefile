#/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# *   Mupen64plus-input-sdl - Makefile                                      *
# *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
# *   Copyright (C) 2007-2009 Richard Goedeken                              *
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
# Makefile for SDL Input plugin in Mupen64plus

CPU = ARM
OS = LINUX
SHARED = -shared
SO_EXTENSION = so

# source and output dirs
SRCDIR ?= .
OBJDIR = _obj

# base CFLAGS, LDLIBS, and LDFLAGS
OPTFLAGS ?= -O3 -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp
CFLAGS += $(OPTFLAGS) -Wall -ffast-math -fno-strict-aliasing -fvisibility=hidden -I$(SRCDIR) -g
CXXFLAGS += -fvisibility-inlines-hidden
LDFLAGS += $(SHARED)

ifeq ("$(CPU)","ARM")
  NO_ASM := 1
  PIC := 1
endif

# tweak flags for 32-bit build on 64-bit system
ifeq ($(ARCH_DETECTED), 64BITS_32)
  CFLAGS += -m32
  LDFLAGS += -m32 -Wl,-m,elf_i386
endif

# Since we are building a shared library, we must compile with -fPIC on some architectures
# On 32-bit x86 systems we do not want to use -fPIC because we don't have to and it has a big performance penalty on this arch
ifeq ($(PIC), 1)
  CFLAGS += -fPIC
  LDFLAGS += -fPIC
else
  CFLAGS += -fno-PIC
  LDFLAGS += -fno-PIC
endif

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
     #CFLAGS += -I/opt/PalmPDK/include
    CFLAGS += -I../blackberry-SDL/include -I../blackberry-SDL_image/ -I../blackberry-SDL_ttf/ -IC:\bbndk-2.1.0-beta1\target\qnx6/usr/include/freetype2
    LDLIBS += -L../blackberry-SDL/Device-Debug -lSDL12 -lGLESv2 -L../libs/ -lbbutil
    #CFLAGS += -D__linux__ -DTOUCHPAD_UI -DPRE
    CFLAGS += -DPRE -DTOUCHPAD_UI
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


# list of source files to compile
SOURCE = \
	$(SRCDIR)/plugin.c \
	$(SRCDIR)/autoconfig.c \
	$(SRCDIR)/config.c \
	$(SRCDIR)/osal_dynamiclib_unix.c \
	$(SRCDIR)/touch.c \
	#$(SRCDIR)/bbutil.c

# generate a list of object files build, make a temporary directory for them
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(filter %.c, $(SOURCE)))
OBJDIRS = $(dir $(OBJECTS))
$(shell $(MKDIR) $(OBJDIRS))

# build dependency files
CFLAGS += -MD
-include $(OBJECTS:.o=.d)

# build targets
TARGET = ../libs/libmupen64plus-input-sdl.$(SO_EXTENSION)

targets:
	@echo "Mupen64Plus-input-sdl makefile. "
	@echo "  Targets:"
	@echo "    all           == Build Mupen64Plus SDL input plugin"
	@echo "    clean         == remove object files"
	@echo "    rebuild       == clean and re-build all"
	@echo "    install       == Install Mupen64Plus SDL input plugin"
	@echo "    uninstall     == Uninstall Mupen64Plus SDL input plugin"
	@echo "  Options:"
	@echo "    BITS=32       == build 32-bit binaries on 64-bit machine"
	@echo "    APIDIR=path   == path to find Mupen64Plus Core headers"
	@echo "    OPTFLAGS=flag == compiler optimization (default: -O3)"
	@echo "    PIC=(1|0)     == Force enable/disable of position independent code"
	@echo "  Install Options:"
	@echo "    PREFIX=path   == install/uninstall prefix (default: /usr/local)"
	@echo "    SHAREDIR=path == path to install shared data files (default: PREFIX/share/mupen64plus)"
	@echo "    LIBDIR=path   == library prefix (default: PREFIX/lib)"
	@echo "    PLUGINDIR=path == path to install plugin libraries (default: LIBDIR/mupen64plus)"
	@echo "    DESTDIR=path  == path to prepend to all installation paths (only for packagers)"
	@echo "  Debugging Options:"
	@echo "    DEBUG=1       == add debugging symbols"
	@echo "    PLUGINDBG=1   == print extra debugging information while running"
	@echo "    V=1           == show verbose compiler output"

all: $(TARGET)

install: $(TARGET)
	$(INSTALL) -d "$(DESTDIR)$(PLUGINDIR)"
	$(INSTALL) -m 0644 $(INSTALL_STRIP_FLAG) $(TARGET) "$(DESTDIR)$(PLUGINDIR)"
	$(INSTALL) -d "$(DESTDIR)$(SHAREDIR)"
	$(INSTALL) -m 0644 "../../data/InputAutoCfg.ini" "$(DESTDIR)$(SHAREDIR)"

uninstall:
	$(RM) "$(DESTDIR)$(PLUGINDIR)/$(TARGET)"
	$(RM) "$(DESTDIR)$(SHAREDIR)/InputAutoCfg.ini"

clean:
	$(RM) -r ./_obj $(TARGET)

rebuild: clean all

# standard build rules
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILE.c) -o $@ $<

$(TARGET): $(OBJECTS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

.PHONY: all clean install uninstall targets
