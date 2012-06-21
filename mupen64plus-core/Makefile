#/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# *   Mupen64plus - Makefile                                                *
# *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
# *   Copyright (C) 2008-2009 Richard Goedeken                              *
# *   Copyright (C) 2007-2008 DarkJeztr Tillin9                             *
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
# Makefile for Mupen64Plus Core

CPU = ARM
OS = LINUX
SHARED = -shared
SO_EXTENSION = so

# source and output dirs
SRCDIR ?= .
OBJDIR = _obj

# base CFLAGS, LDLIBS, and LDFLAGS
OPTFLAGS ?= -O3 -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp
CFLAGS += $(OPTFLAGS) -Wall -ffast-math -fno-strict-aliasing -fvisibility=hidden -I$(SRCDIR)
CXXFLAGS += -fvisibility-inlines-hidden
LDFLAGS += $(SHARED)

ifeq ("$(CPU)","ARM")
  PIC := 1
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
ifeq ($(OS), FREEBSD)
  TARGET = libmupen64plus.so.2.0.0
  SONAME = libmupen64plus.so.2
  LDFLAGS += -Wl,-Bsymbolic -shared -Wl,-export-dynamic -Wl,-soname,$(SONAME)
  LDLIBS += -L${LOCALBASE}/lib -lc
endif
ifeq ($(OS), LINUX)
  TARGET = ../libs/libmupen64plus.so.2
  SONAME = libmupen64plus.so.2
  LDFLAGS += -shared
  #LDLIBS += -ldl
  ifeq ($(CPU),ARM)
    #headers for palm, sdl, libpng
    #CFLAGS += -I/opt/PalmPDK/include
    CFLAGS += -I../blackberry-SDL/include
    CFLAGS += -DNEW_DYNAREC -D_GNU_SOURCE -DDYNAREC -D__arm__ -D_PROFILE
    LDLIBS += -L../libs -lbbutil
    #LDFLAGS += -allow-shlib-undefined
  endif
endif
ifeq ($(OS), OSX)
  CFLAGS += -DUSE_FILE32API
  TARGET = libmupen64plus.dylib
  LDFLAGS += -bundle -read_only_relocs suppress
  LDLIBS += -ldl -framework OpenGL
  ifeq ($(CPU), X86)
    ifeq ($(ARCH_DETECTED), 64BITS)
      CFLAGS += -pipe -arch x86_64 -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk
      LDFLAGS += -arch x86_64
    else
      CFLAGS += -pipe -mmmx -msse -arch i686 -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk
      LDFLAGS += -arch i686
      ifneq ($(PROFILE), 1)
        CFLAGS += -fomit-frame-pointer
      endif
    endif
  endif
endif
ifeq ($(CPU_ENDIANNESS), BIG)
  CFLAGS += -DM64P_BIG_ENDIAN
endif

# reduced compile output when running make without V=1
ifneq ($(findstring $(MAKEFLAGS),s),s)
#ifndef V
	#Q_CC  = @echo '    CC  '$@;
	#Q_CXX = @echo '    CXX '$@;
	#Q_LD  = @echo '    LD  '$@;
#endif
endif

# set base program pointers and flags
#CDIR := /opt/PalmPDK/arm-gcc/bin/
CPREFIX :=

# cross compiler for arm
ifeq ($(CPU),ARM)
	#CPREFIX := arm-none-linux-gnueabi-
endif

#CC := $(CDIR)$(CPREFIX)gcc
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

# compiler/linker flags for various compile-time options.
# 1. macro for no assembly language
ifeq ($(NO_ASM), 1)
  CFLAGS += -DNO_ASM
endif
# 2. variables for profiling and adding debugging symbols
ifeq ($(PROFILE), 1)
  CFLAGS += -pg -g
  LDFLAGS += -pg
  INSTALL_STRIP_FLAG ?= 
else
  ifeq ($(DEBUG), 1)
    CFLAGS += -g
    INSTALL_STRIP_FLAG ?= 
  else
    INSTALL_STRIP_FLAG ?= -s
  endif
endif
# 3. other options given to the makefile on the command line
ifeq ($(LIRC), 1)
  CFLAGS += -DWITH_LIRC
endif
ifeq ($(DEBUGGER), 1)
  CFLAGS += -DDBG
endif
ifeq ($(DBG_COMPARE), 1)
  CFLAGS += -DCOMPARE_CORE
endif
ifeq ($(DBG_CORE), 1)
  CFLAGS += -DCORE_DBG
endif
ifeq ($(DBG_COUNT), 1)
  CFLAGS += -DCOUNT_INSTR
endif
ifeq ($(DBG_PROFILE), 1)
  CFLAGS += -DPROFILE_R4300
endif
# 4. compile-time directory paths for building into the library
ifneq ($(SHAREDIR),)
  CFLAGS += -DSHAREDIR="$(SHAREDIR)"
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
ifeq ($(INCDIR),)
  INCDIR := $(PREFIX)/include/mupen64plus
endif

# list of required source files for compilation
SOURCE = \
	$(SRCDIR)/api/callbacks.c \
	$(SRCDIR)/api/common.c \
	$(SRCDIR)/api/config.c \
	$(SRCDIR)/api/debugger.c \
	$(SRCDIR)/api/frontend.c \
	$(SRCDIR)/api/vidext.c \
	$(SRCDIR)/main/main.c \
	$(SRCDIR)/main/util.c \
	$(SRCDIR)/main/cheat.c \
	$(SRCDIR)/main/eventloop.c \
	$(SRCDIR)/main/md5.c \
	$(SRCDIR)/main/rom.c \
	$(SRCDIR)/main/ini_reader.c \
	$(SRCDIR)/main/savestates.c \
	$(SRCDIR)/main/adler32.c \
	$(SRCDIR)/main/ticks.c \
	$(SRCDIR)/memory/dma.c \
	$(SRCDIR)/memory/flashram.c \
	$(SRCDIR)/memory/memory.c \
	$(SRCDIR)/memory/n64_cic_nus_6105.c \
	$(SRCDIR)/memory/pif.c \
	$(SRCDIR)/memory/tlb.c \
	$(SRCDIR)/osal/dynamiclib_unix.c \
	$(SRCDIR)/osal/files_unix.c \
	$(SRCDIR)/plugin/plugin.c \
	$(SRCDIR)/plugin/dummy_video.c \
	$(SRCDIR)/plugin/dummy_audio.c \
	$(SRCDIR)/plugin/dummy_input.c \
	$(SRCDIR)/plugin/dummy_rsp.c \
	$(SRCDIR)/r4300/r4300.c \
	$(SRCDIR)/r4300/bc.c \
	$(SRCDIR)/r4300/cop0.c \
	$(SRCDIR)/r4300/cop1.c \
	$(SRCDIR)/r4300/cop1_d.c \
	$(SRCDIR)/r4300/cop1_l.c \
	$(SRCDIR)/r4300/cop1_s.c \
	$(SRCDIR)/r4300/cop1_w.c \
	$(SRCDIR)/r4300/exception.c \
	$(SRCDIR)/r4300/interupt.c \
	$(SRCDIR)/r4300/profile.c \
	$(SRCDIR)/r4300/pure_interp.c \
	$(SRCDIR)/r4300/recomp.c \
	$(SRCDIR)/r4300/special.c \
	$(SRCDIR)/r4300/regimm.c \
	$(SRCDIR)/r4300/tlb.c \
	$(SRCDIR)/r4300/empty_dynarec.c \
	$(SRCDIR)/r4300/new_dynarec/fpu.c \
	$(SRCDIR)/r4300/new_dynarec/linkage_arm.S \
	$(SRCDIR)/r4300/new_dynarec/new_dynarec.c \
	$(SRCDIR)/main/zip/ioapi.c \
	$(SRCDIR)/main/zip/zip.c \
	$(SRCDIR)/main/zip/unzip.c


ifeq ($(DEBUGGER), 1)
  SOURCE += \
	$(SRCDIR)/debugger/debugger.c \
	$(SRCDIR)/debugger/dbg_decoder.c \
	$(SRCDIR)/debugger/dbg_memory.c \
	$(SRCDIR)/debugger/dbg_breakpoints.c
  LDLIBS += -lopcodes -lbfd
endif

# generate a list of object files to build, make a temporary directory for them
OBJECTS := $(patsubst $(SRCDIR)/%.c,   $(OBJDIR)/%.o, $(filter %.c,   $(SOURCE)))
OBJECTS += $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(filter %.cpp, $(SOURCE)))
OBJECTS += $(patsubst $(SRCDIR)/%.S, $(OBJDIR)/%.o, $(filter %.S, $(SOURCE)))
OBJDIRS = $(dir $(OBJECTS))
$(shell $(MKDIR) $(OBJDIRS))

# build targets
targets:
	@echo "Mupen64Plus-core makefile. "
	@echo "  Targets:"
	@echo "    all           == Build Mupen64Plus core library"
	@echo "    clean         == remove object files"
	@echo "    install       == Install Mupen64Plus core library"
	@echo "    uninstall     == Uninstall Mupen64Plus core library"
	@echo "  Build Options:"
	@echo "    BITS=32       == build 32-bit binaries on 64-bit machine"
	@echo "    LIRC=1        == enable LIRC support"
	@echo "    NO_ASM=1      == build without assembly (no dynamic recompiler or MMX/SSE code)"
	@echo "    SHAREDIR=path == extra path to search for shared data files"
	@echo "    OPTFLAGS=flag == compiler optimization (default: -O3)"
	@echo "    PIC=(1|0)     == Force enable/disable of position independent code"
	@echo "  Install Options:"
	@echo "    PREFIX=path   == install/uninstall prefix (default: /usr/local/)"
	@echo "    SHAREDIR=path == path to install shared data files (default: PREFIX/share/mupen64plus)"
	@echo "    LIBDIR=path   == path to install core library (default: PREFIX/lib)"
	@echo "    INCDIR=path   == path to install core header files (default: PREFIX/include/mupen64plus)"
	@echo "    DESTDIR=path  == path to prepend to all installation paths (only for packagers)"
	@echo "  Debugging Options:"
	@echo "    PROFILE=1     == build gprof instrumentation into binaries for profiling"
	@echo "    DEBUG=1       == add debugging symbols to binaries"
	@echo "    DEBUGGER=1    == build graphical debugger"
	@echo "    DBG_CORE=1    == print debugging info in r4300 core"
	@echo "    DBG_COUNT=1   == print R4300 instruction count totals (64-bit dynarec only)"
	@echo "    DBG_COMPARE=1 == enable core-synchronized r4300 debugging"
	@echo "    DBG_PROFILE=1 == dump profiling data for r4300 dynarec to data file"
	@echo "    V=1           == show verbose compiler output"

all: $(TARGET)

install: $(TARGET)
	$(INSTALL) -d "$(DESTDIR)$(LIBDIR)"
	$(INSTALL) -m 0644 $(INSTALL_STRIP_FLAG) $(TARGET) "$(DESTDIR)$(LIBDIR)"
	$(INSTALL) -d "$(DESTDIR)$(SHAREDIR)"
	$(INSTALL) -m 0644 ../../data/* "$(DESTDIR)$(SHAREDIR)"
	$(INSTALL) -d "$(DESTDIR)$(INCDIR)"
	$(INSTALL) -m 0644 ../../src/api/m64p_*.h "$(DESTDIR)$(INCDIR)"
	-$(LDCONFIG) "$(DESTDIR)$(LIBDIR)"
	if [ ! -e "$(DESTDIR)$(LIBDIR)/$(SONAME)" ]; then ln -sf "$(TARGET)" "$(DESTDIR)$(LIBDIR)/$(SONAME)"; fi

uninstall:
	$(RM) "$(DESTDIR)$(LIBDIR)/$(TARGET)"
	if [ "$(SONAME)" != "" ]; then $(RM) "$(DESTDIR)$(LIBDIR)/$(SONAME)"; fi
	$(RM) $(DESTDIR)$(INCDIR)/m64p_*.h
	$(RM) "$(DESTDIR)$(SHAREDIR)/mupen64plus.cht"
	$(RM) "$(DESTDIR)$(SHAREDIR)/mupen64plus.ini"
	$(RM) "$(DESTDIR)$(SHAREDIR)/font.ttf"
	$(RM) "$(DESTDIR)$(SHAREDIR)/mupencheat.txt"

clean:
	$(RM) -r $(TARGET) $(SONAME) ./_obj

# build dependency files
CFLAGS += -MD
-include $(OBJECTS:.o=.d)

CXXFLAGS += $(CFLAGS)

# standard build rules
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILE.c) -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(COMPILE.cc) -o $@ $<
	
$(OBJDIR)/%.o: $(SRCDIR)/%.S
	$(COMPILE.c) -o $@ $<

$(TARGET): $(OBJECTS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
#	if [ "$(SONAME)" != "" ]; then ln -sf $@ $(SONAME); fi

.PHONY: all clean install uninstall targets
