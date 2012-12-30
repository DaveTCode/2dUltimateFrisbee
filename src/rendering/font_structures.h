/*
 * font_structures.h
 *
 *  Created on: 2 Apr 2010
 *      Author: David
 */

#ifndef FONT_STRUCTURES_H_
#define FONT_STRUCTURES_H_

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

typedef struct glyph
{
  int minx;
  int maxx;
  int miny;
  int maxy;
  int advance;
  GLfloat texcoord[4];
  GLuint texid;
} GLYPH;

typedef struct font
{
  int style;
  int pointsize;
  int height;
  int ascent;
  int descent;
  int lineskip;
  SDL_Color color;
  GLYPH glyphs[128];
} FONT;

#endif /* FONT_STRUCTURES_H_ */
