/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-core - ui-base.c                                  *
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
#include <stdio.h>
#include <dirent.h>
#include <math.h>
#include <string.h>

#include "ui_base.h"
//#define PRELUDE_FONT_PATH "/usr/share/fonts/Prelude-Medium.ttf"
//#define ROM_PATH "/media/internal/n64"

SDL_Surface* screen;
TTF_Font *largeFont,*smallFont;

GLuint       defaultProgram;
GLuint       tiledProgram;
GLuint       g_VertexLoc      = 0;
GLuint       g_TexcoordLoc    = 1;
GLuint		 offsetLoc,tiledOffsetLoc,textureScaleLoc,backgroundLoc ;

const char* vertexShader =
		"uniform vec2 uOffset;                                \n"
		"attribute highp vec2 aPosition;                        \n"
		"attribute highp vec2 aTexCoord;                        \n"
		"varying mediump vec2 vTexCoord;                        \n"
		"void main(){                                           \n"
		"gl_Position = vec4(aPosition.x+uOffset.x, aPosition.y+uOffset.y, 0, 1.0);\n\t"
		"vTexCoord = aTexCoord;                                 \n"
		"}                                                      \n";

const char* fragmentShader =
		"uniform sampler2D uTex;                                \n"
		"varying mediump vec2 vTexCoord;                        \n"
		"void main(){                                           \n"
		"gl_FragColor = texture2D(uTex, vTexCoord);             \n"
		"}";

const char* tiledFragmentShader =
		"uniform sampler2D uTex;                                                                            \n"
		"uniform mediump vec2 uTextureScale;                                                                \n"
		"varying mediump vec2 vTexCoord;                                                                    \n"
		"uniform lowp vec4 uBackground;                                                                     \n"
		"const lowp vec4 foreground = vec4(1.0,1.0,1.0,0.8);                                                \n"
		"mediump float angle = -0.30;                                                                       \n"
		"mediump mat2 rotate = mat2(cos(angle),-sin(angle),                                                 \n"
		"						sin(angle),cos(angle));                                                     \n"
		"void main(){                                                                                       \n"
		"mediump vec2 uv = rotate * vTexCoord * uTextureScale;                                              \n"
		"uv.x += floor(uv.y) / 3.0;                                                                        \n"
		"gl_FragColor = mix(uBackground,foreground,texture2D(uTex, vec2(mod(uv.x,1.0),mod(uv.y,1.0))).a);    \n"
		"}                                                                                                  \n";



GLuint CreateShader(GLenum type,const char* source)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL );
	glCompileShader(shader);

	CheckShader(shader);

	return shader;
}

void CheckShader(GLuint shader)
{
	GLint result = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		char Log[1024];
		GLint nLength;
		glGetShaderInfoLog(shader, 1024, &nLength, Log);
		printf(Log);
	}
}

void CheckProgram(GLuint program)
{
	GLint result = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (!result)
	{
		char Log[1024];
		GLint nLength;
		glGetProgramInfoLog(program, 1024, &nLength, Log);
		printf(Log);
	}
}

void UIInitBase()
{
	GLuint vs = CreateShader(GL_VERTEX_SHADER,vertexShader);
	GLuint frag = CreateShader(GL_FRAGMENT_SHADER,fragmentShader);
	GLuint tiled = CreateShader(GL_FRAGMENT_SHADER,tiledFragmentShader);

	defaultProgram = glCreateProgram();
	glAttachShader( defaultProgram, vs );
	glAttachShader( defaultProgram, frag );

	glBindAttribLocation(defaultProgram, g_VertexLoc,   "aPosition");
	glBindAttribLocation(defaultProgram, g_TexcoordLoc, "aTexCoord");

	glLinkProgram(defaultProgram);
	CheckProgram(defaultProgram);

	offsetLoc = glGetUniformLocation(defaultProgram,"uOffset");

	tiledProgram = glCreateProgram();
	glAttachShader( tiledProgram, vs );
	glAttachShader( tiledProgram, tiled );

	glBindAttribLocation(tiledProgram, g_VertexLoc,   "aPosition");
	glBindAttribLocation(tiledProgram, g_TexcoordLoc, "aTexCoord");

	glLinkProgram(tiledProgram);
	CheckProgram(tiledProgram);

	tiledOffsetLoc = glGetUniformLocation(tiledProgram,"uOffset");
	textureScaleLoc = glGetUniformLocation(tiledProgram,"uTextureScale");
	backgroundLoc = glGetUniformLocation(tiledProgram,"uBackground");



	if( TTF_Init() != 0){
		printf("Error TTF init!\n");fflush(stdout);
	}

	largeFont = TTF_OpenFont("/usr/fonts/font_repository/monotype/arial.ttf", 20);
	smallFont = TTF_OpenFont("/usr/fonts/font_repository/monotype/arial.ttf", 14);

	if(!largeFont || !smallFont){
		printf("Error opening font!\n");fflush(stdout);
	}
}

void UIClose()
{
	glDeleteProgram(defaultProgram);
	glDeleteProgram(tiledProgram);

	TTF_CloseFont(largeFont);
	TTF_CloseFont(smallFont);

	TTF_Quit();
}

float UIPixelToViewportX(float x)
{
	return ((x/1024.0f)*2.0f)-1.0f;
}

float UIPixelToViewportY(float y)
{
	return -(((y/768.0f)*2.0f)-1.0f);
}

float UIPixelToViewportW(float w)
{
	return (w/1024.0f)*2.0f;
}

float UIPixelToViewportH(float h)
{
	return (h/768.0f)*2.0f;
}

void UIOffset(float x,float y)
{
	glUseProgram(defaultProgram);
	glUniform2f(offsetLoc,x,y);
	glUseProgram(tiledProgram);
	glUniform2f(tiledOffsetLoc,x,y);
}

UIQuad* UICreateImage(SDL_Surface* surface)
{
	UIQuad* quad = malloc(sizeof(UIQuad));
	quad->text = NULL;

	quad->x = UIPixelToViewportX(0);
	quad->y = UIPixelToViewportY(0);
	quad->w = quad->textureW = UIPixelToViewportW(surface->w);
	quad->h = quad->textureH = UIPixelToViewportH(surface->h);
	quad->program = defaultProgram;

	quad->scale = 1.0;
	quad->alpha = 1.0;


	GLint format = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;

	glGenTextures(1,&quad->texture);
	glBindTexture( GL_TEXTURE_2D, quad->texture);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0,format, surface->w, surface->h,
				  0, format, GL_UNSIGNED_BYTE, surface->pixels);

	return quad;
}

UIQuad* UICreateText(TTF_Font* font,char* text)
{
	SDL_Colour color = {255,255,255};

	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, color);
	UIQuad* quad = UICreateImage(textSurface);
	quad->text = malloc(strlen(text)+1);
	strcpy(quad->text,text);

	SDL_FreeSurface(textSurface);

	return quad;
}

int UIHitTest(UIQuad* quad,float x,float y)
{
	return x >= quad->x && x <= quad->x + quad->w && y <= quad->y && y >= quad->y - quad->h;
}

UIQuad* UIHitTests(UIQuad** quads,int count,float x,float y)
{
	int i;
	for(i=0;i < count;i++)
	{
		if(UIHitTest(quads[i],x,y))
			return quads[i];
	}
	return NULL;
}

void UIBeginDraw()
{
	glClearColor(0,0,0,1);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_DST_ALPHA);
	glUseProgram(defaultProgram);
	glEnableVertexAttribArray(g_VertexLoc);
    glEnableVertexAttribArray(g_TexcoordLoc);
}

void UIDrawQuad(UIQuad* quad)
{
	float s = max(quad->scale,0.0f) * 0.5f;
	float midX = quad->x + (quad->w / 2.0f);
	float midY = quad->y - (quad->h / 2.0f);
	float scaledW = quad->w * s;
	float scaledH = quad->h * s;
	GLfloat vert[] =
    {
        //x					y        					u        v
        midX-scaledW,	midY-scaledH, 					+0.0,	 1.0,
        midX+scaledW, 	midY-scaledH, 					1.0,	 1.0,
        midX-scaledW,	midY+scaledH, 					0.0,	+0.0,
        midX+scaledW,	midY+scaledH, 					1.0,	 +0.0
    };

    glBindTexture(GL_TEXTURE_2D,quad->texture);
    glUseProgram(quad->program);

    if(quad->program == tiledProgram)
    {
      	glUniform2f(textureScaleLoc,quad->w / quad->textureW,quad->h / quad->textureH);
      	glUniform4fv(backgroundLoc,1,quad->colour);
    }

    glVertexAttribPointer(g_VertexLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (float*)vert);
    glVertexAttribPointer(g_TexcoordLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (float*)vert + 2);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

void UILayoutVertically(UIQuad** quads,int count)
{
	int i;
	for(i = 1;i < count;i++)
	{
		quads[i]->y = quads[i-1]->y - quads[i-1]->h;
	}
}

void UILayoutWrap(UIQuad** quads,int count,float spacing)
{
	int i;

	UIQuad *cur,*prev;
	float left = quads[0]->x;
	float maxHeight = 0;

	for(i = 1;i < count;i++)
	{
		cur = quads[i];
		prev = quads[i-1];
		maxHeight = max(maxHeight,cur->h);
		if(prev->x + cur->w >= 1.0) //new line
		{
			cur->y = prev->y - maxHeight;
			cur->x = left;
		}
		else
		{
			cur->y = prev->y;
			cur->x = prev->x + prev->w;
		}
	}
}

void UIFreeQuad(UIQuad* quad)
{
	glDeleteTextures(1,&quad->texture);
	free(quad->text);
	free(quad);
}
#endif

