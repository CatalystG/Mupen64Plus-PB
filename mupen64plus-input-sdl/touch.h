/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-input-sdl - touch.h                                  *
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

#ifndef TOUCH_H_
#define TOUCH_H_

#define Lerp(min,max,value) \
		min + (value * (max-min))

#include "plugin.h"
#include <SDL.h>
#include "../bbutil/bbutil.h"
#include <GLES2/gl2.h>
//#include <SDL_image.h>

enum TouchOverlayMode
{
	TouchOverlayHidden,
	TouchOverlayVisible,
	TouchOverlayMenu
};

typedef struct
{
	int button; //EButton
	uint32_t keyColour;
	uint8_t finger;//which finger is pressing the button, 0=inactive
}TouchButton;

typedef struct
{
	float xCentre,yCentre; //Centre of virtual stick
	float radius;
	float xPos,yPos; //Current stick values
	uint8_t finger; //which finger is controlling the stick,  0=inactive
}TouchStick;

extern TouchButton touchButtons[14];
extern TouchStick touchStick;

void InitTouchInput();
inline void ProcessTouchEvent(screen_event_t *screen_event,SController* controller,unsigned short* button_bits);
inline void ApplyTouchInput();
int UseAccelerometer();
void touch_video_plugin_render_callback(void);
UIQuad *create_image(const char* filename, float x, float y,  int raw_pix);
#endif /* TOUCH_H_ */
