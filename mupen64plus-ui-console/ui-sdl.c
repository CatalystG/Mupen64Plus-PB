/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-core - ui-sdl.c                                  *
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
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <ctype.h>

#include <EGL\egl.h>
#include <SDL_image.h>
#include <screen/screen.h>

#include "ui-sdl.h"
#include "bbutil.h"

#define ROM_PATH "shared/misc/n64"

SDL_Surface* screen;

EGLDisplay egl_disp;
EGLSurface egl_surf;

EGLConfig egl_conf;
EGLContext egl_ctx;

screen_context_t screen_ctx;
screen_window_t screen_win;
screen_display_t screen_disp;

int nbuffers = 2;

static char** boxArtImages;
static int boxArtCount;

void UIInit()
{
	//screen_create_context(&screen_ctx, 0);
	//bbutil_init_egl(screen_ctx);
	fflush(stdout);

	UIInitBase();
	UILoadBoxArt();
}

static int UIRomFilter (const struct dirent *dir)
{
	char* end = (char*)dir->d_name;
	end += strlen(dir->d_name);

	return (strstr(dir->d_name,".n64") == end - 4) || (strstr(dir->d_name,".v64") == end - 4) || (strstr(dir->d_name,".z64") == end - 4);
}

static int UIPngFilter (const struct dirent *dir)
{
	char* end = (char*)dir->d_name;
	end += strlen(dir->d_name);

	return (strstr(dir->d_name,".png") == end - 4);
}

static int UISimilarStrings(char* haystack,char* needle,int count)
{
	int end = min(count,min(strlen(haystack),strlen(needle)));
	if(end <= 0)
		return 0;

	int h=0,n=0;

	while(h < end && n < end)
	{
		if(!isalnum(haystack[h]))
			h++;

		if(!isalnum(needle[n]))
			n++;

		if(tolower(haystack[h]) != tolower(needle[n]))
			return 0;
		else
		{
			h++;
			n++;
		}
	}
	return 1;
}

static UIQuad** UIGetFiles(int* count)
{
	struct dirent** dirs;

	*count = scandir(ROM_PATH,&dirs,UIRomFilter,alphasort);

	UIQuad** t = malloc(sizeof(UIQuad*) * (*count));

	int i,colour=0;
	int artIndex=0;

	char fullPath[1024];


	for(i = 0;i < *count;i++)
	{
		t[i] = NULL;
		//TODO avoid scanning every boxart for each rom as they are both sorted
		for(artIndex = 0;artIndex < boxArtCount; artIndex++)
		{
//			if(t[i] != NULL) //If rom has multiple matching box arts free previous
//				UIFreeQuad(t[i]);
			if(UISimilarStrings(dirs[i]->d_name,boxArtImages[artIndex],strlen(boxArtImages[artIndex])-4))
			{
				snprintf(fullPath,SDL_arraysize(fullPath),"%s/boxart/%s",ROM_PATH,boxArtImages[artIndex]);
				SDL_Surface* boxArt = IMG_Load(fullPath);
				if(boxArt != NULL)
				{
					t[i] = UICreateImage(boxArt);
					SDL_FreeSurface(boxArt);
				}

			}
		}

		if(t[i] == NULL)
		{
			SDL_Color white;
			white.r = white.g = white.b = 255;
			SDL_Surface* bg = SDL_CreateRGBSurface(SDL_SWSURFACE,256,177,24,0xFF0000,0x00FF00,0x0000FF,0x000000);
			SDL_Surface* text = TTF_RenderText_Blended(smallFont,dirs[i]->d_name,white);
			SDL_Rect textPos;
			textPos.w = text->w;
			textPos.h = text->h;
			textPos.y = bg->h - text->h - 2;
			textPos.x = (Sint16)((bg->w / 2.0f) - (text->w / 2.0f));

			if(colour)
			{
				SDL_FillRect(bg,NULL,SDL_MapRGB(bg->format,60,60,60));
			}

			SDL_BlitSurface(text,NULL,bg,&textPos);

			t[i] = UICreateImage(bg);

			SDL_FreeSurface(bg);
			SDL_FreeSurface(text);

		}

		t[i]->text = malloc(strlen(dirs[i]->d_name)+1);
		strcpy(t[i]->text,dirs[i]->d_name);

		if((i % 4) != 3)
			colour = !colour;

		t[i]->scale = -(drand48()/2.0);

		free(dirs[i]);
	}
	free(dirs);

	return t;
}

static void UILoadBoxArt()
{
	struct dirent** dirs;
	boxArtCount = scandir(ROM_PATH "/boxart",&dirs,UIPngFilter,alphasort);

	if(boxArtCount > 0)
	{
		boxArtImages = malloc(boxArtCount * sizeof(char*));

		int i;
		for(i = 0;i < boxArtCount;i++)
		{
			boxArtImages[i] = malloc(strlen(dirs[i]->d_name)+1);
			strcpy(boxArtImages[i],dirs[i]->d_name);
			free(dirs[i]);
		}
		free(dirs);
	}
}

void UIInstructions()
{
	//UIQuad* instructions[] = {
	//	UICreateText(largeFont,"No Roms Found!"),
	//	UICreateText(largeFont,"Place unzipped .n64, v.64, z.64 files into folder /media/internal/n64"),
	//	UICreateText(largeFont,"When mounted as a USB drive this is just /n64")};

	//UILayoutVertically(instructions,SDL_arraysize(instructions));
	SDL_Event event;

	SDL_PollEvent(&event);
	int i;

	font_t *test = bbutil_load_font(BBUTIL_DEFAULT_FONT, 16, 170);

	while(event.type != SDL_QUIT)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		//for(i = 0;i < SDL_arraysize(instructions);i++)
			//UIDrawQuad(instructions[i]); //,0.0f);
		bbutil_render_text(test, "No Roms!", 0.0f, 550.0f, 1.0f, 1.0f, 1.0f, 1.0f);
		bbutil_render_text(test, "Put them in /shared/misc/n64!", 0.0f, 500.0f, 1.0f, 1.0f, 1.0f, 1.0f);

		//SDL_GL_SwapBuffers();
		PB_eglSwapBuffers();

		SDL_PollEvent(&event);
	}

	//for(i = 0;i < SDL_arraysize(instructions);i++)
	//{
	//	UIFreeQuad(instructions[i]);
	//}

}

char* UIChooseRom(int* disableSound,int* videoPlugin)
{
	UIInit();

	UIBeginDraw();

	int count = 0;
	UIQuad** texts = UIGetFiles(&count);
	UIQuad* soundOn = UICreateText(largeFont,"[Sound On]");
	UIQuad* soundOff = UICreateText(largeFont,"[Sound Off]");
	UIQuad* sound = soundOn;

	UIQuad* videoRice = UICreateText(largeFont,"[Video Rice]");
	UIQuad* videoGl = UICreateText(largeFont,"[Video gles2n64]");
	UIQuad* video = *videoPlugin == VIDEO_PLUGIN_GLES2N64 ? videoGl : videoRice;

	if(count <= 0)
	{
		printf("No Files Found...\n");fflush(stdout);
		UIInstructions();
		return NULL;
	}
	else
	{
		texts[0]->y = sound->y - sound->h;

		videoRice->x = 1 - videoRice->w;
		videoGl->x = 1 - videoGl->w;

		UILayoutWrap(texts,count,0);

		SDL_Event event;

		int motionCount = 0;
		const int tapThreshold = 7;
		float scrollOffset = 0,startY=0,scrollOriginY=0,scrollVelocity=0;
		Uint32 motionStart;

		float scrollEnd = -(texts[count-1]->y-texts[count-1]->h);

		if(fabs(scrollEnd) < 1)
			scrollEnd = 0;
		else
			scrollEnd -= 1;


		char* chosen = 0;
		int i;
		while(!chosen)
		{
			if(motionCount == 0 && fabs(scrollVelocity) > 0.01)
			{
				scrollOffset += scrollVelocity;
				scrollVelocity *= 0.85;

				if(scrollOffset <= 0)
					scrollOffset = 0;
				if(scrollOffset >= scrollEnd)
					scrollOffset = scrollEnd;
			}

			UIOffset(0,scrollOffset);
			UIDrawQuad(sound);
			UIDrawQuad(video);

			for(i = 0;i < count;i++)
			{
				UIDrawQuad(texts[i]);
				if(texts[i]->scale < 1.0)
					texts[i]->scale += 0.03;
			}

			PB_eglSwapBuffers();
			glClear(GL_COLOR_BUFFER_BIT);

			while(!chosen && SDL_PollEvent(&event))
			{
				if(event.type == SDL_MOUSEMOTION)
				{
					float x = event.motion.x * 2 - 1;
					float y = -(event.motion.y * 2 - 1);

					if(motionCount == 0)
					{
						startY = y;
						scrollOriginY = y - scrollOffset;
						motionStart = SDL_GetTicks();
						//printf("Down %f\n",y);
					}
					else
					{
						scrollOffset = y - scrollOriginY;
						if(scrollOffset <= 0)
							scrollOffset = 0;
						if(scrollOffset >= scrollEnd)
							scrollOffset = scrollEnd;

						//printf("Move %f\n",y);
					}
					motionCount++;
				}
				else if(event.type == SDL_MOUSEBUTTONUP)
				{
					float x = event.button.x * 2 - 1;
					float y = -(event.button.y * 2 - 1) - scrollOffset;

					if(motionCount <= tapThreshold) //Tap
					{
						if(fabs(scrollVelocity) > 0.01)
							scrollVelocity = 0;
						else
						{
							if(UIHitTest(sound,x,y))
							{
								sound = sound == soundOn ? soundOff : soundOn;
							}
							else if(UIHitTest(video,x,y))
							{
								video = video == videoRice ? videoGl : videoRice;
							}
							else
							{
								UIQuad* selected = UIHitTests(texts,count,x,y);
								if(selected)
								{
									chosen = malloc(strlen(selected->text)+strlen(ROM_PATH)+2);
									sprintf(chosen,"%s/%s",ROM_PATH,selected->text);
								}
							}
						}
					}
					const float maxTime = 400;

					motionCount = 0;
					scrollVelocity = y + scrollOffset - startY;
					float time = min(SDL_GetTicks() - motionStart,maxTime);
					float factor = 1.0f - (time / maxTime);
					scrollVelocity *= factor;
					//printf("Up velocity %f %f time:%f timef %f\n",scrollVelocity,factor,time,time / maxTime);
				}
				else if(event.type == SDL_QUIT)
				{
					break;
				}
			}
		}

		*disableSound = sound == soundOff;
		*videoPlugin = video == videoRice ? VIDEO_PLUGIN_RICE : VIDEO_PLUGIN_GLES2N64;

		UIFreeQuad(soundOn);
		UIFreeQuad(soundOff);

		UIFreeQuad(videoRice);
		UIFreeQuad(videoGl);

		for(i = 0;i < count;i++)
		{
			UIFreeQuad(texts[i]);
		}
		free(texts);

		return chosen;
	}
}
#endif
