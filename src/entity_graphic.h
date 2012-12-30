/*
 * entity_graphic.h
 *
 *  Created on: 17 Oct 2009
 *      Author: David
 */

#ifndef ENTITY_GRAPHIC_H_
#define ENTITY_GRAPHIC_H_

#include "SDL/SDL_opengl.h"

#define LOAD_SPRITE_OK           0
#define LOAD_SPRITE_LOAD_FAIL    1
#define LOAD_SPRITE_CONVERT_FAIL 2

/*
 * ENTITY_GRAPHIC
 *
 * A graphic placeholder for holding simple sprites.
 *
 * sprite - This contains a reference to the texture which is to be applied to
 *          the graphic.
 * width - The width of the graphic in pixels.
 * height - The height of the graphic in pixels.
 * ref_count - The number of references to this graphic object. When it is 0
 * the object is no longer being used so we can remove it.
 */
typedef struct entity_graphic
{
  GLuint sprite;
  int width;
  int height;
  int ref_count;
} ENTITY_GRAPHIC;

int create_entity_graphic(char *, ENTITY_GRAPHIC **, bool);
void destroy_entity_graphic(ENTITY_GRAPHIC *);
void add_reference_to_graphic(ENTITY_GRAPHIC *);
bool remove_reference_to_graphic(ENTITY_GRAPHIC *);

#endif /* ENTITY_GRAPHIC_H_ */
