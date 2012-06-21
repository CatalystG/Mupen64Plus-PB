/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-input-sdl - touch.c                                	   *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2011 Metricity                                   	   *
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

#include "touch.h"
//#include "../mupen64plus-ui-console/ui_base.h"

#include <math.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#define OVERLAY_DIR "app/native"

#include <stdlib.h>

TouchButton touchButtons[14];
TouchStick touchStick;

int showMenu = 1;
int showController = 1;
float save = 0.0f, load = 0.0f;

//UIQuad* overlayQuad = NULL;
//UIQuad* stickQuad = NULL;
//UIQuad* saveButton = NULL;
//UIQuad* loadButton = NULL;
//UIQuad* changeOverlayButton = NULL;
//UIQuad* toggleAccelerometerButton = NULL;
//UIQuad* osd_save = NULL;
//UIQuad* osd_load = NULL;

int useAccelerometer = 0;
int currentOverlay = 0;
int overlayCount = 0;
char** availableOverlays;

void LoadOverlay(char* name)
{
	//if(overlayQuad)
	//{
		//TODO: Free Overlays
		//UIFreeQuad(overlayQuad);
		//UIFreeQuad(stickQuad);
		//SDL_FreeSurface(overlayKey);
	//}

	char* tmp = malloc(sizeof(char) * (strlen(OVERLAY_DIR) + strlen(name) + 5 + 4 + 2)); // Name+Stick+.png+NULL

	//if(overlay && overlayKey && stick)
	//{
		printf("\nLoading Overlay Key...\n");fflush(stdout);

		//OverlayKey is only a pix buffer, not a texture as it's not used to render
		sprintf(tmp,"%s/%sKey.png",OVERLAY_DIR, name);
		overlayKey = create_image(tmp, 0, 0, 1);

		printf("\nLoading Overlay...\n");fflush(stdout);
		sprintf(tmp,"%s/%s.png",OVERLAY_DIR,name);
		overlayQuad = create_image(tmp, 0, 0, 0);

		printf("\nLoading Stick...\n");fflush(stdout);
		sprintf(tmp,"%s/%sStick.png",OVERLAY_DIR,name);
		stickQuad = create_image(tmp, 0, 0, 0);
	//}
}

//static int OverlayFilter(const struct dirent *dir)
//{
//	char* end = (char*)dir->d_name;
//	end += strlen(dir->d_name);
//
//	return (strstr(dir->d_name,"Key.png") == end - 7);
//}
//
//static char** GetOverlays(int* count)
//{
//	struct dirent** dirs;
//
//	*count = scandir(OVERLAY_DIR,&dirs,OverlayFilter,alphasort);
//	printf("Found %i overlays\n",*count);
//	char** t = malloc(sizeof(char*) * (*count));
//
//	int i;
//	for(i = 0;i < *count;i++)
//	{
//		t[i] = malloc((sizeof(char) * strlen(dirs[i]->d_name))+1);
//		strcpy(t[i],dirs[i]->d_name);
//	}
//
//	return t;
//}

void InitTouchInput()
{
	float tWidth, tHeight;

	if(initialized_n64 == 0){
		compile_text_program();
		if(text_rendering_program == 0){
			printf("Error creating text program...");
		}

		font = bbutil_load_font("/usr/fonts/font_repository/monotype/arial.ttf", 12, 170);
		bbutil_measure_text(font, "[Save]", &tWidth, &tHeight);

		saveButton = create_text(font, 0.0f, 600.0f-tHeight, "[Save]");
		loadButton = create_text(font, 1024.0f-tWidth, 600.0f-tHeight, "[Load]");
		osd_save = create_text(font, 400.0f, 0.0f, "Game State Saved!");
		osd_load = create_text(font, 400.0f, 0.0f, "Game State Loaded!");

		LoadOverlay("Default");
		initialized_n64 = 1;
	}
	//font = TTF_OpenFont(PRELUDE_FONT_PATH,30);

	//Search for centre pixel and radius pixel
	float touchCentreX=0,touchCentreY=0,touchRadiusX=100,touchRadiusY=100;
	Uint32* p = (Uint32*)overlayKey->raw_pix;
	int x,y;
	for(y = 0;y < (int)overlayKey->textureH;y++)
	{
		for(x = 0;x < (int)overlayKey->textureW;x++)
		{
			if(*p == SDL_Swap32(0x00FFFFFF))//Centre
			{
				DebugMessage(M64MSG_VERBOSE,"Found centre %i %i",x,y);
				touchCentreX = x;
				touchCentreY = y;
			}
			else if(*p == SDL_Swap32(0xA7FFFCFF))
			{
				DebugMessage(M64MSG_VERBOSE,"Found radius %i %i",x,y);
				touchRadiusX = x;
				touchRadiusY = y;
			}
			p++;
		}
	}

	touchStick.radius = sqrt(pow(touchCentreX-touchRadiusX,2.0)+pow(touchCentreY-touchRadiusY,2.0));;
	touchStick.xCentre = touchCentreX;
	touchStick.yCentre = touchCentreY;
	touchStick.finger = 0;



	//overlayQuad = create_image("app/native/Default.png", 0, 0, 0);
	//stickQuad = create_image("app/native/DefaultStick.png", 0, 0, 0);
	//changeOverlayButton = UICreateText(font,"[Overlay Default]");
	//toggleAccelerometerButton = UICreateText(font,"[Accelerometer Off]");

	//availableOverlays = GetOverlays(&overlayCount);

	//int i;
	//for(i = 0;i < overlayCount;i++)
	//{
	//	if(strstr(availableOverlays[i],"Default") == availableOverlays[i])
	//	{
	//		currentOverlay = i+1;//zero is no overlay
	//		break;
	//	}
	//}

	//TTF_CloseFont(font);

	//changeOverlayButton->x = -(changeOverlayButton->w / 2.0f);
	//toggleAccelerometerButton->x = -(toggleAccelerometerButton->w /2.0f);
	//toggleAccelerometerButton->y = changeOverlayButton->y - changeOverlayButton->h;

	memset(touchButtons, 0, sizeof(TouchButton) * 9);
	//keyColor is RGBA
	touchButtons[0].button = START_BUTTON;
	touchButtons[0].keyColour = SDL_Swap32(0xFF0000FF);

	touchButtons[1].button = Z_TRIG;
	touchButtons[1].keyColour = SDL_Swap32(0xBDD9FFFF);

	touchButtons[2].button = A_BUTTON;
	touchButtons[2].keyColour = SDL_Swap32(0x2A00FFFF);

	touchButtons[3].button = B_BUTTON;
	touchButtons[3].keyColour = SDL_Swap32(0x00FF37FF);

	touchButtons[4].button = L_TRIG;
	touchButtons[4].keyColour = SDL_Swap32(0x009CFFFF);

	touchButtons[5].button = R_TRIG;
	touchButtons[5].keyColour = SDL_Swap32(0xFF00C6FF);

	touchButtons[6].button = U_DPAD;
	touchButtons[6].keyColour = SDL_Swap32(0x2B2B2BFF);

	touchButtons[7].button = D_DPAD;
	touchButtons[7].keyColour = SDL_Swap32(0x7F7F7FFF);

	touchButtons[8].button = L_DPAD;
	touchButtons[8].keyColour = SDL_Swap32(0xFFFFFFFF);

	touchButtons[9].button = R_DPAD;
	touchButtons[9].keyColour = SDL_Swap32(0xC4C3C3FF);

	touchButtons[10].button = U_CBUTTON;
	touchButtons[10].keyColour = SDL_Swap32(0xFF9C00FF);

	touchButtons[11].button = D_CBUTTON;
	touchButtons[11].keyColour = SDL_Swap32(0xFFC600FF);

	touchButtons[12].button = L_CBUTTON;
	touchButtons[12].keyColour = SDL_Swap32(0xF6FF00FF);

	touchButtons[13].button = R_CBUTTON;
	touchButtons[13].keyColour = SDL_Swap32(0xFFEA00FF);

	printf("InitTouchInput Complete...\n");fflush(stdout);
}

//void ProcessMenu(float x,float y)
//{
//	if(UIHitTest(saveButton,x,y))
//	{
//		CoreDoCommand(M64CMD_STATE_SAVE,1,NULL);
//	}
//	else if(UIHitTest(loadButton,x,y))
//	{
//		CoreDoCommand(M64CMD_STATE_LOAD,0,NULL);
//	}
//	else if(UIHitTest(changeOverlayButton,x,y))
//	{
//		currentOverlay = (currentOverlay + 1) % (overlayCount+1);
//
//		float y = changeOverlayButton->y;
//		UIFreeQuad(changeOverlayButton);
//
//		if(currentOverlay == 0)
//		{
//			changeOverlayButton = UICreateText(font,"[Overlay Off]");
//			showController = 0;
//		}
//		else
//		{
//			showController = 1;
//			char tmp[1024];
//			char shortName[1024];
//			strcpy(shortName,availableOverlays[currentOverlay-1]);
//			shortName[strlen(shortName)-7] = 0; //Remove Key.png
//			LoadOverlay(shortName);
//
//			snprintf(tmp,1024,"[Overlay %s]",shortName);
//			changeOverlayButton = UICreateText(font,tmp);
//		}
//
//
//		changeOverlayButton->x = -(changeOverlayButton->w/2);
//		changeOverlayButton->y = y;
//	}
//	else if(UIHitTest(toggleAccelerometerButton,x,y))
//	{
//		float y = toggleAccelerometerButton->y;
//		UIFreeQuad(toggleAccelerometerButton);
//		if(!useAccelerometer)
//		{
//			toggleAccelerometerButton = UICreateText(font,"[Accelerometer On]");
//		}
//		else
//		{
//			toggleAccelerometerButton = UICreateText(font,"[Accelerometer Off]");
//		}
//		useAccelerometer = !useAccelerometer;
//
//		toggleAccelerometerButton->x = -(toggleAccelerometerButton->w/2.0f);
//		toggleAccelerometerButton->y = y;
//	}
//
//}

void ProcessTouchEvent(screen_event_t *event,SController* controller,unsigned short* button_bits)
{
	Uint16 x,y;
	Uint8 finger;
	int type;
	int contactId;
	int pos[2];

	screen_get_event_property_iv(*event, SCREEN_PROPERTY_TYPE, &type);
	screen_get_event_property_iv(*event, SCREEN_PROPERTY_TOUCH_ID, (int*)&contactId);
	screen_get_event_property_iv(*event, SCREEN_PROPERTY_SOURCE_POSITION, pos);

	if(type == SCREEN_EVENT_MTOUCH_MOVE)
	{
		//DebugMessage(M64MSG_VERBOSE,  "Motion");
		x = pos[0];
		y = 600 - pos[1];
		finger = contactId+1;//zero means inactive
	}
	else if(type == SCREEN_EVENT_MTOUCH_TOUCH || type == SCREEN_EVENT_MTOUCH_RELEASE)
	{
		//DebugMessage(M64MSG_VERBOSE,  "Button");
		x = pos[0];
		y = 600 - pos[1];
		finger = contactId + 1;
		//printf("x:%d, y:%d, finger:%d\n", x, y, finger);fflush(stdout);
	}

	if(y > 550 && type == SCREEN_EVENT_MTOUCH_TOUCH)
	{
		if(x > 300 && x < 700){
			showMenu ^= 1;
		}

		if(showMenu) {
			if (x < 100) {
				//save button
				CoreDoCommand(M64CMD_STATE_SAVE,1,NULL);
				save = 1;
			} else if (x > 924) {
				//load
				CoreDoCommand(M64CMD_STATE_LOAD,0,NULL);
				load = 1;
			}
		}
	}

	if(overlayKey && x < overlayKey->textureW && y < overlayKey->textureH)
	{
		uint32_t pixel = ((Uint32*)overlayKey->raw_pix)[(y * (int)overlayKey->textureW) + x];

		int i;
		int buttonHit = 0;
		TouchButton* whichButton = NULL;

		//printf("pixel: %x", pixel);fflush(stdout);
		for(i=0;i < (sizeof(touchButtons)/sizeof(touchButtons[0]));i++)
		{
			if(touchButtons[i].finger == finger)
				whichButton = &touchButtons[i];

			if(touchButtons[i].keyColour == pixel && finger != touchStick.finger)
			{
				//printf("Button Pressed! Colour: %x\n", touchButtons[i].keyColour);fflush(stdout);
				touchButtons[i].finger = finger;
				buttonHit = 1;
				break;
			}
		}

		if((buttonHit == 0 || type == SCREEN_EVENT_MTOUCH_RELEASE) && whichButton) //if no button release button for correct finger (which)
		{
			whichButton->finger = 0;
		}
	}

	double dist = sqrt(pow((double)x-touchStick.xCentre,2.0)+pow((double)y-touchStick.yCentre,2.0));

	//on mouseup turn stick off
	if(type == SCREEN_EVENT_MTOUCH_RELEASE && finger == touchStick.finger)
	{
		//DebugMessage(M64MSG_VERBOSE,  "Turned stick off");
		touchStick.finger = 0;
		touchStick.xPos = touchStick.yPos = 0;
	}
	else if(dist < touchStick.radius && touchStick.finger == 0)
	{
		touchStick.finger = finger;
	}

	//update stick position
	if(finger == touchStick.finger)
	{
		touchStick.xPos = SDL_min(SDL_max(((x - touchStick.xCentre) / touchStick.radius),-1.0f),1.0f) * 80.0;
		touchStick.yPos = SDL_min(SDL_max(((touchStick.yCentre - y) / touchStick.radius),-1.0f),1.0f) * 80.0;
	}


}

int UseAccelerometer()
{
	return useAccelerometer;
}

void ApplyTouchInput(SController* controller,unsigned short* button_bits)
{
	if(touchStick.finger != 0) //Apply touch input
	{
		//printf("Apply stick...\n");
		controller->buttons.X_AXIS = (int)touchStick.xPos;
		controller->buttons.Y_AXIS = -(int)touchStick.yPos;
	}
	else //Read accelerometer input for display
	{
		touchStick.xPos = controller->buttons.X_AXIS;
		touchStick.yPos = controller->buttons.Y_AXIS;
	}

	int i;
	for(i=0;i < (sizeof(touchButtons)/sizeof(touchButtons[0]));i++)
	{
		if(touchButtons[i].finger != 0)
		{
			//printf("Apply Button...\n");
		    controller->buttons.Value |= button_bits[touchButtons[i].button];
		}
	}
	fflush(stdout);
}

void DrawController()
{
	if(overlayQuad == NULL){
		printf("NULL OVERLAY!!!!\n");fflush(stdout);
	}
	render_image(overlayQuad);

	//TIP: Don't mix different coordinate systems.
	stickQuad->x = UIPixelToViewportX(touchStick.xCentre + touchStick.xPos) - (stickQuad->w/2.0f);// + touchStick.xPos + (stickQuad->w/2.0f));// -(stickQuad->w/2.0f));
	stickQuad->y = UIPixelToViewportY(touchStick.yCentre - touchStick.yPos) + (stickQuad->h/2.0f);// -touchStick.yPos + (stickQuad->h/2.0f));// -(stickQuad->h/2.0f));

	render_image(stickQuad);
}

void DrawMenu()
{
	render_text(font, saveButton);
	render_text(font, loadButton);
	//UIDrawQuad(changeOverlayButton);
	//UIDrawQuad(toggleAccelerometerButton);

	if(save > 0){
		osd_save->alpha = save;
		render_text(font, osd_save);
		save -= 0.02f;

	}
	if(load > 0){
		osd_load->alpha = load;
		render_text(font, osd_load);
		load -= 0.02f;
	}
}

void touch_video_plugin_render_callback(void)
{
	//return;
    GLint currentProgram,currentTexture,currentActiveTexture,currentBlendSRC,currentBlendDST,currentVertexAttrib,currentTexcoorAttrib;
    GLboolean currentDepth,currentCull,currentScissor,currentBlend,currentDepthMask;

    GLint currentVertexSize, currentTexSize;
    GLsizei currentVertexStride, currentTexStride;
    void *currentVertexPtr;
    void *currentTexPtr;

    //Save settings

    glGetIntegerv(GL_CURRENT_PROGRAM,&currentProgram);
    glGetIntegerv(GL_TEXTURE_BINDING_2D,&currentTexture);
    glGetIntegerv(GL_BLEND_SRC_ALPHA,&currentBlendSRC);
    glGetIntegerv(GL_BLEND_DST_ALPHA,&currentBlendDST);
    glGetIntegerv(GL_ACTIVE_TEXTURE,&currentActiveTexture);

    glGetVertexAttribiv(0,GL_VERTEX_ATTRIB_ARRAY_ENABLED,&currentVertexAttrib);
    glGetVertexAttribiv(0,GL_VERTEX_ATTRIB_ARRAY_SIZE,&currentVertexSize);
    glGetVertexAttribiv(0,GL_VERTEX_ATTRIB_ARRAY_STRIDE,&currentVertexStride);
    glGetVertexAttribPointerv(0,GL_VERTEX_ATTRIB_ARRAY_POINTER,&currentVertexPtr);


    glGetVertexAttribiv(1,GL_VERTEX_ATTRIB_ARRAY_ENABLED,&currentTexcoorAttrib);
    glGetVertexAttribiv(1,GL_VERTEX_ATTRIB_ARRAY_SIZE,&currentTexSize);
    glGetVertexAttribiv(1,GL_VERTEX_ATTRIB_ARRAY_STRIDE,&currentTexStride);
    glGetVertexAttribPointerv(1,GL_VERTEX_ATTRIB_ARRAY_POINTER,&currentTexPtr);

    currentDepth = glIsEnabled(GL_DEPTH_TEST);
    currentCull = glIsEnabled(GL_CULL_FACE);
    currentScissor= glIsEnabled(GL_SCISSOR_TEST);
    currentBlend = glIsEnabled(GL_BLEND);

    glViewport(0,0,1024,600);

    glGetBooleanv(GL_DEPTH_WRITEMASK,&currentDepthMask);

	glUseProgram(text_rendering_program);
	glEnableVertexAttribArray(positionLoc);
	glEnableVertexAttribArray(texcoordLoc);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_SCISSOR_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	//if(showController)
	//printf("Draw Controller!\n");fflush(stdout);
		DrawController();
    if(showMenu){
    	//printf("Draw Menu!\n");fflush(stdout);
    	DrawMenu();
    }

    //Restore
    if(currentTexcoorAttrib)
    {
    	glVertexAttribPointer(1, currentTexSize, GL_FLOAT, GL_FALSE, currentTexStride, currentTexPtr);
    }
    else
    {
    	glDisableVertexAttribArray( 1 );
    }
    if(currentVertexAttrib)
    {
    	glVertexAttribPointer(0, currentVertexSize, GL_FLOAT, GL_FALSE, currentVertexStride, currentVertexPtr);
    }
    else
    {
    	glDisableVertexAttribArray( 0 );
    }

//    glDepthMask(GL_TRUE);
//    glClearDepthf(1.0f);
//    glClear(GL_DEPTH_BUFFER_BIT);

	glBlendFunc(currentBlendSRC,currentBlendDST);
	if(!currentBlend)
		glDisable(GL_BLEND);
	if(currentScissor)
		glEnable(GL_SCISSOR_TEST);
	if(currentCull)
		glEnable(GL_CULL_FACE);
	if(currentDepth)
		glEnable(GL_DEPTH_TEST);

	glDepthMask(currentDepthMask);

	glActiveTexture(currentActiveTexture);
	glBindTexture(GL_TEXTURE_2D,currentTexture);
    glUseProgram(currentProgram);
}
