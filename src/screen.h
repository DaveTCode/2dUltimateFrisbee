/*
 * screen.h
 *
 *  Created on: 19 Oct 2009
 *      Author: David
 */
#ifndef SCREEN_H_
#define SCREEN_H_

#include "SDL/SDL_video.h"

/*
 * Screen constants. TODO - to be moved out to config file at some point.
 */
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BPP 32

/*
 * SCREEN:
 *
 * This object holds information on the screen which will be passed to SDL
 * routines. It is dynamic so that the user can change resolution etc.
 *
 * viewport - The surface onto which we draw the next frame.
 * width - The width of the screen.
 * height - The height of the screen.
 * bpp - The number of bits per pixel. Should be set to 32 under normal op.
 */
typedef struct screen
{
  SDL_Surface *viewport;
  int width;
  int height;
  int bpp;
  float centre_x;
  float centre_y;
} SCREEN;

struct screen *create_screen();
void destroy_screen(struct screen *);

#endif /* SCREEN_H_ */
