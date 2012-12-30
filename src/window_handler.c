/*
 * window_handler.c
 *
 * Does the SDL screen initialisation routines and creates the screen object.
 *
 *  Created on: 17 Oct 2009
 *      Author: David
 */

#include "dt_logger.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>
#include "window_handler.h"
#include "screen.h"
#include "data_structures/vector.h"

/*
 * init_window_system
 *
 * Set up the SDL screen object.
 *
 * Parameters: screen - The screen object containing resolution info.
 *             sdl_screen - This will contain a pointer to the new SDL_Surface
 *                          created for the screen.
 *
 * Returns: One of INIT_WINDOW_RET_CODES.
 */
int init_window_system(SCREEN *screen, int width, int height, int bpp)
{
  /*
   * Local Variables.
   */
  int ret_code = INIT_OK;
  int result;

  /*
   * Initialise SDL video subsystem.
   */
  result = SDL_Init(SDL_INIT_VIDEO);
  if (-1 == result)
  {
    DT_DEBUG_LOG("Failed to initialise SDL video subsystem: %s\n",
                 SDL_GetError());
    ret_code = INIT_SDL_FAIL;
    goto EXIT_LABEL;
  }

  /*
   * Set the screen parameters.
   */
  screen->bpp = bpp;
  screen->height = height;
  screen->width = width;
  screen->centre_x = ((float) screen->width) / 2.0f;
  screen->centre_y = ((float) screen->height) / 2.0f;

  /*
   * Set up the opengl attributes.
   */
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

  /*
   * Set up the screen to use hardware rendering and a double frame buffer.
   */
  screen->viewport = (SDL_Surface *) SDL_SetVideoMode(width,
                                                      height,
                                                      bpp,
                                                      SDL_OPENGL);
  if (NULL == screen->viewport)
  {
    SDL_Quit();
    ret_code = INIT_SCREEN_FAIL;
    DT_DEBUG_LOG("SDL video mode init failed: %s\n", SDL_GetError());
    goto EXIT_LABEL;
  }

  /*
   * Set up the TTF font sub system.
   */
  if (TTF_Init() < 0)
  {
    SDL_Quit();
    ret_code = INIT_TTF_FAIL;
    DT_DEBUG_LOG("TTF initialisation failed: %s\n", TTF_GetError());
    goto EXIT_LABEL;
  }

EXIT_LABEL:

  return(ret_code);
}
