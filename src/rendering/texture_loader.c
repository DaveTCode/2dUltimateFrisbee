/*
 * texture_loader.c
 *
 *  Created on: 2 Apr 2010
 *      Author: David
 */
#include "../dt_logger.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <stdbool.h>
#include "texture_loader.h"

/*
 * SDL_GL_LoadTexture
 *
 * Takes an sdl surface and turns it into an opengl texture.
 *
 * Parameters: surface - The sdl surface to convert.
 *             texture - The returned texture.
 *             isTextureWrapping - Set to true if the texture should get tiled
 *                                 and set to false if it should get stretched.
 *
 * Returns - One of LOAD_TEXTURE_RET_CODES.
 */
int SDL_GL_LoadTexture(SDL_Surface *surface,
                       GLuint *texture,
                       bool isTextureWrapping)
{
  /*
   * Local Variables.
   */
  SDL_Surface *optimised_surface;
  int ret_code = LOAD_TEXTURE_OK;
  GLenum rc;

  /*
   * Set the sprite to be in the same format as the display. This also forces
   * the optimised surface to have an alpha channel and allows us to use pngs
   * with transparency.
   */
  optimised_surface = SDL_DisplayFormatAlpha(surface);
  if (NULL == optimised_surface)
  {
    DT_DEBUG_LOG("Failed to optimise texture: %s\n", SDL_GetError());
    ret_code = LOAD_TEXTURE_NOT_CONVERTED;
    goto EXIT_LABEL;
  }

  /*
   * Generate a new openGL texture object.
   */
  glGenTextures(1, texture);

  /*
   * Bind the new texture object so that opengl knows what texture we are
   * loading and then load the texture using the pixel data from the optimised
   * surface.
   *
   * We set the following:
   * Linear approximation for both magnification and minification of the image.
   * This means that if the sprite that is being textured is smaller/larger
   * than the image passed in then the texture will be applied smoothly to the
   * whole image.
   *
   * BGRA -> RGBA conversion. The image comes in as an SDL surface in BGRA
   * format but we intend to store it as an RGBA image.
   */
  glBindTexture(GL_TEXTURE_2D, *texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               optimised_surface->w,
               optimised_surface->h,
               0,
               GL_BGRA,
               GL_UNSIGNED_BYTE,
               optimised_surface->pixels);

  /*
   * Check that the texture has loaded correctly.
   */
  rc = glGetError();
  if (GL_NO_ERROR != rc)
  {
    DT_DEBUG_LOG("Opengl error whilst loading a texture: %x\n", rc);
    ret_code = LOAD_TEXTURE_GL_FAIL;
  }

EXIT_LABEL:

  /*
   * Free the memory used for the optimised SDL surface.
   */
  if (NULL != optimised_surface)
  {
    SDL_FreeSurface(optimised_surface);
  }

  return ret_code;
}
