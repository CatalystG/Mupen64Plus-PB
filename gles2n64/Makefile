#/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# *   Mupen64plus - Makefile                                                *
# *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
# *   Copyright (C) 2007-2008 DarkJeztr Tillin9 Richard42                   *
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
# Makefile for glN64 in Mupen64plus.

ARCH = ARM
OS = LINUX
#ARCH = X86
#OS = WIN32

CFLAGS  = -Wall -O3
LDFLAGS =

SO_EXTENSION = so
CC_OVERRIDE = C:/bbndk-2.1.0-beta1/host/win32/x86/usr/bin/qcc -V4.4.2,gcc_ntoarmv7le_cpp -w1 -shared
CXX = $(CC_OVERRIDE)
INCLUDE = C:\Development\Mupen64Plus-PB
CFLAGS  += -I$(INCLUDE)/blackberry-SDL/include
CFLAGS  += -I$(INCLUDE)
CFLAGS  += -I$(INCLUDE)/mupen64plus-core/api
CFLAGS  += -march=armv7-a -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -ffast-math \
                  -fsingle-precision-constant  -ftree-vectorize -fexpensive-optimizations -fomit-frame-pointer

CFLAGS  += -findirect-inlining
CFLAGS  += -ftree-switch-conversion

CFLAGS  +=-DARM_ASM -D__NEON_OPT -D__VEC4_OPT -D__HASHMAP_OPT -D__CRC_OPT
#CFLAGS += -D__PACKVERTEX_OPT
CFLAGS += -D__TRIBUFFER_OPT

CFLAGS += -Wall -fPIC
LDFLAGS += -lGLESv2 -L../blackberry-SDL/Device-Debug -lSDL12 -lc  -lang-c++ -L../libs -lbbutil


#CFLAGS += -DPROFILE_GBI
#CFLAGS += -DSHADER_TEST

OBJECTS =

# list of object files to generate
OBJECTS = Config.o \
    gles2n64.o \
        OpenGL.o \
        N64.o \
        RSP.o \
        VI.o \
        Textures.o \
        ShaderCombiner.o \
        gDP.o \
        gSP.o \
        gSPNeon.o \
        GBI.o \
        DepthBuffer.o \
        CRC.o \
        2xSAI.o \
        RDP.o \
        F3D.o \
        F3DEX.o \
        F3DEX2.o \
        L3D.o \
        L3DEX.o \
        L3DEX2.o \
        S2DEX.o \
        S2DEX2.o \
        F3DPD.o \
        F3DDKR.o \
        F3DWRUS.o \
        F3DCBFD.o

# build targets
all: ../libs/gles2n64.$(SO_EXTENSION)

clean:
	rm -f *.o *.s *.ii *.$(SO_EXTENSION) ui_gln64config.*

# build rules
.cpp.o:
	$(CXX) -o $@ $(CFLAGS) -c $<

.c.o:
	$(CXX) -o $@ $(CFLAGS) -c $<

ui_gln64config.h: gln64config.ui
	$(UIC) $< -o $@

../libs/gles2n64.$(SO_EXTENSION): $(OBJECTS)
	$(CXX) $^ $(LDFLAGS) $(SDL_LIBS) $(LIBGL_LIBS) -o $@

gles2n64.o: gles2N64.cpp
	$(CXX) $(CFLAGS) $(SDL_FLAGS) -DMAINDEF -c -o $@ $<

Config_qt4.o: Config_qt4.cpp ui_gln64config.h
	$(CXX) $(CFLAGS) $(SDL_FLAGS) -c -o $@ $<

Config_gtk2.o: Config_gtk2.cpp
	$(CXX) $(CFLAGS) $(SDL_FLAGS) -c -o $@ $<

GBI.o: GBI.cpp
	$(CXX) $(CFLAGS) $(SDL_FLAGS) -c -o $@ $<
