/*
 * font_loader.c
 *
 *  Created on: 2 Apr 2010
 *      Author: David
 */
#include "../dt_logger.h"

#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>
#include "font_loader.h"
#include "font_structures.h"
#include "texture_loader.h"

/*
 * create_font
 */
int create_font(char *filename,
                FONT **font,
                int size,
                int style,
                SDL_Color color)
{
  TTF_Font* ttffont;
  SDL_Surface* character_surface;
  int ret_code = BUILD_FONT_OK;
  int rc;
  int ii;

  /*
   * Allocate the memory required for the font object.
   */
  (*font) = (FONT *) DT_MALLOC(sizeof(FONT));

  /*
   * Set the font attributes.
   */
  memcpy(&((*font)->color), &color, sizeof(SDL_Color));
  (*font)->pointsize = size;
  (*font)->style = style;

  /*
   * The font loading may fail if the file does not exist or permissions are
   * incorrect etc.
   */
  ttffont = TTF_OpenFont(filename, (*font)->pointsize);
  if (ttffont == NULL)
  {
    DT_DEBUG_LOG("Error opening font (%s): %s\n", filename, TTF_GetError());
    ret_code = BUILD_FONT_LOAD_FAIL;
    goto EXIT_LABEL;
  }

  /*
   * Set the style of the font to whatever we passed in.
   */
  TTF_SetFontStyle(ttffont, (*font)->style);

  /*
   * Retrieve the font information to store in our font structure.
   */
  (*font)->ascent = TTF_FontAscent(ttffont);
  (*font)->descent = TTF_FontDescent(ttffont);
  (*font)->height = TTF_FontHeight(ttffont);
  (*font)->lineskip = TTF_FontLineSkip(ttffont);

  /*
   * For each possible glyph, attempt to load the font character and create a
   * texture for it. If any of these fail we attempt to close down gracefully
   * and exit the function.
   */
  for (ii = ' '; ii <= '~'; ii++)
  {
    character_surface = TTF_RenderGlyph_Blended(ttffont, ii, (*font)->color);
    if (NULL == character_surface)
    {
      DT_DEBUG_LOG("Error rendering glyph %c whilst creating font (%s): %s\n",
                   (char) ii,
                   filename,
                   TTF_GetError());
      ret_code = BUILD_FONT_RENDER_FAIL;
      goto EXIT_LABEL;
    }

    /*
     * Retrieve the metric info from the font object and store it in our local
     * structure.
     */
    TTF_GlyphMetrics(ttffont,
                     ii,
                     &((*font)->glyphs[ii].minx),
                     &((*font)->glyphs[ii].maxx),
                     &((*font)->glyphs[ii].miny),
                     &((*font)->glyphs[ii].maxy),
                     &((*font)->glyphs[ii].advance));

    /*
     * This function can fail if opengl cannot allocate any more memory for
     * textures.
     */
    rc = SDL_GL_LoadTexture(character_surface, &((*font)->glyphs[ii].texid), false);
    if (LOAD_TEXTURE_OK != rc)
    {
      DT_DEBUG_LOG("Failed creating texture for glyph %c from font %s\n",
                   (char) ii,
                   TTF_GetError());
      SDL_FreeSurface(character_surface);
      ret_code = BUILD_FONT_CREATE_TEXTURE_FAIL;
      goto EXIT_LABEL;
    }

    /*
     * Set the texture coordinates for the glyph (note this is the same for all
     * glyphs).
     */
    (*font)->glyphs[ii].texcoord[0] = 0.0f;
    (*font)->glyphs[ii].texcoord[1] = 0.0f;
    (*font)->glyphs[ii].texcoord[2] = 1.0f;
    (*font)->glyphs[ii].texcoord[3] = 1.0f;

    /*
     * The surface used for that character is no longer required.
     */
    SDL_FreeSurface(character_surface);
  }

EXIT_LABEL:

  if (NULL != ttffont)
  {
    TTF_CloseFont(ttffont);
  }

  return ret_code;
}

/*
 * destroy_font
 *
 * Free the memory allocated for a font object.
 *
 * Parameters: font - The object to be freed.
 */
void destroy_font(FONT *font)
{
  /*
   * Local Variables
   */
  int ii;

  /*
   * Free each of the opengl textures.
   */
  for (ii = ' '; ii <= '~'; ii++)
  {
    glDeleteTextures(1, &(font->glyphs[ii].texid));
  }

  /*
   * Free the font object itself.
   */
  DT_FREE(font);
}
