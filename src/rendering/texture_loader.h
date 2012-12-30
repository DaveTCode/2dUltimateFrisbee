/*
 * texture_loader.h
 *
 *  Created on: 2 Apr 2010
 *      Author: David
 */

#ifndef TEXTURE_LOADER_H_
#define TEXTURE_LOADER_H_

#include "SDL/SDL_video.h"
#include "SDL/SDL_opengl.h"

/*
 * LOAD_TEXTURE_RET_CODES
 *
 * LOAD_TEXTURE_OK - Function was successful.
 * LOAD_TEXTURE_NOT_CONVERTED - Texture could not be converted to display
 *                              format.
 * LOAD_TEXTURE_GL_FAIL - Could not create the opengl texture object from the
 *                        sdl surface.
 */
#define LOAD_TEXTURE_OK            0
#define LOAD_TEXTURE_NOT_CONVERTED 1
#define LOAD_TEXTURE_GL_FAIL       2

int SDL_GL_LoadTexture(SDL_Surface *, GLuint *, bool);

#endif /* TEXTURE_LOADER_H_ */
