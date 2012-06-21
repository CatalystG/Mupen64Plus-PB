/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-ui-console - ui-sdl.h                                  *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2011 Metricity                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef TOUCHPAD_UI
#ifndef UI_SDL_H_
#define UI_SDL_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "ui_base.h"

enum VIDEO_PLUGIN
{
	VIDEO_PLUGIN_RICE,
	VIDEO_PLUGIN_GLES2N64
};

char* UIChooseRom(int* disableSound,int* videoPlugin);
static void UILoadBoxArt();
#endif /* UI_SDL_H_ */
#endif
