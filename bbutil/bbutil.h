/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _UTILITY_H_INCLUDED
#define _UTILITY_H_INCLUDED

#include <EGL/egl.h>
#include <screen/screen.h>
#include <sys/platform.h>

#include <bps/bps.h>
#include <bps/dialog.h>
#include <bps/screen.h>
#include <bps/event.h>
#include <bps/orientation.h>
#include <bps/navigator.h>
#include <GLES2/gl2.h>

#include <png.h>

typedef struct{
    unsigned int font_texture;
    float pt;
    float advance[128];
    float width[128];
    float height[128];
    float tex_x1[128];
    float tex_x2[128];
    float tex_y1[128];
    float tex_y2[128];
    float offset_x[128];
    float offset_y[128];
    int initialized;
}font_t;

extern EGLDisplay egl_disp;
extern EGLSurface egl_surf;
extern screen_context_t screen_cxt;
extern font_t* font;
extern GLuint text_rendering_program;
extern GLint positionLoc;
extern GLint texcoordLoc;
extern GLint textureLoc;
extern GLint colorLoc;

extern int disableSound,videoPlugin;
extern char romName[256];
extern int initialized_n64;

typedef struct
{
	char* text;
	GLuint texture,program;
	float x,y,w,h,textureW,textureH,scale,alpha;
	float colour[4];
	GLfloat *vertices;
	GLfloat *texture_coords;
	GLshort* indices;
	png_byte *raw_pix;
}UIQuad;

#define BBUTIL_DEFAULT_FONT "/usr/fonts/font_repository/monotype/arial.ttf"

extern UIQuad* overlayKey;
extern UIQuad* overlayQuad;
extern UIQuad* stickQuad;
extern UIQuad* saveButton;
extern UIQuad* loadButton;
//UIQuad* changeOverlayButton = NULL;
//UIQuad* toggleAccelerometerButton = NULL;
extern UIQuad* osd_save;
extern UIQuad* osd_load;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes EGL
 *
 * @param libscreen context that will be used for EGL setup
 * @return EXIT_SUCCESS if initialization succeeded otherwise EXIT_FAILURE
 */
int bbutil_init_egl(screen_context_t ctx);

/**
 * Terminates EGL
 */
void bbutil_terminate();

/**
 * Swaps default bbutil window surface to the screen
 */
void bbutil_swap();

/**
 * Loads the font from the specified font file.
 * NOTE: should be called after a successful return from bbutil_init() or bbutil_init_egl() call
 * @param font_file string indicating the absolute path of the font file
 * @param point_size used for glyph generation
 * @param dpi used for glyph generation
 * @return pointer to font_t structure on success or NULL on failure
 */
font_t* bbutil_load_font(const char* font_file, int point_size, int dpi);

/**
 * Destroys the passed font
 * @param font to be destroyed
 */
void bbutil_destroy_font(font_t* font);

/**
 * Renders the specified message using current font starting from the specified
 * bottom left coordinates.
 * NOTE: must be called after a successful return from bbutil_init() or bbutil_init_egl() call

 *
 * @param font to use for rendering
 * @param msg the message to display
 * @param x, y position of the bottom-left corner of text string in world coordinate space
 * @param rgba color for the text to render with
 */
void bbutil_render_text(font_t* font, const char* msg, float x, float y, float r, float g, float b, float a);

/**
 * Returns the non-scaled width and height of a string
 * NOTE: must be called after a successful return from bbutil_init() or bbutil_init_egl() call

 *
 * @param font to use for measurement of a string size
 * @param msg the message to get the size of
 * @param return pointer for width of a string
 * @param return pointer for height of a string
 */
void bbutil_measure_text(font_t* font, const  char* msg, float* width, float* height);

/**
 * Creates and loads a texture from a png file
 * NOTE: must be called after a successful return from bbutil_init() or bbutil_init_egl() call

 *
 * @param filename path to texture png
 * @param return width of texture
 * @param return height of texture
 * @param return gl texture handle
 * @return EXIT_SUCCESS if texture loading succeeded otherwise EXIT_FAILURE
 */

int bbutil_load_texture(const char* filename, int* width, int* height, float* tex_x, float* tex_y, unsigned int* tex, png_byte **raw_pix);

/**
 * Returns dpi for a given screen

 *
 * @param ctx path libscreen context that corresponds to display of interest
 * @return dpi for a given screen
 */

int bbutil_calculate_dpi(screen_context_t ctx);

/**
 * Rotates the screen to a given angle

 *
 * @param angle to rotate screen surface to, must by 0, 90, 180, or 270
 * @return EXIT_SUCCESS if texture loading succeeded otherwise EXIT_FAILURE
 */

int bbutil_rotate_screen_surface(int angle);
void PB_HandleEvents(void (*keyHandleFunction)(screen_event_t*));
void PB_eglSwapBuffers();

float UIPixelToViewportX(float x);
float UIPixelToViewportY(float y);
float UIPixelToViewportW(float w);
float UIPixelToViewportH(float h);
void compile_text_program();
void render_image(UIQuad *quad);
void render_text(font_t *font, UIQuad*quad);
UIQuad *create_image(const char* filename, float x, float y,  int raw_pix);
UIQuad * create_text(font_t *font, float x, float y, char * msg);
int dialog_select_game(char * isofilename, char *isoDir, int *videoPlugin, int *disableSound);

#ifdef __cplusplus
}
#endif

#endif
