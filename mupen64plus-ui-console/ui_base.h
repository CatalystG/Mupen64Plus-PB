/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-ui-console - ui-base.h                                  *
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
#ifndef UI_BASE_H_
#define UI_BASE_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include <GLES2/gl2.h>

#define PRELUDE_FONT_PATH "app/native/font.ttf"

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif





GLuint       tiledProgram;
TTF_Font *largeFont,*smallFont;

GLuint CreateShader(GLenum type,const char* source);
void CheckShader(GLuint shader);
void CheckProgram(GLuint program);


float UIPixelToViewportX(float x);
float UIPixelToViewportY(float y);
float UIPixelToViewportW(float w);
float UIPixelToViewportH(float h);

void UIInitBase();
void UIBeginDraw();
void UIOffset(float x,float y);
//UIQuad* UICreateImage(SDL_Surface* surface);
//UIQuad* UICreateText(TTF_Font* font, char* text);
//int UIHitTest(UIQuad* quad,float x,float y);
//UIQuad* UIHitTests(UIQuad** quads,int count,float x,float y);
//void UILayoutVertically(UIQuad** quads,int count);
//void UILayoutWrap(UIQuad** quads,int count,float spacing);
//void UIDrawQuad(UIQuad* quad);
//void UIFreeQuad(UIQuad* quad);
#endif
#endif
