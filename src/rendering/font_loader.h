/*
 * font_loader.h
 *
 *  Created on: 2 Apr 2010
 *      Author: David
 */
#ifndef FONT_LOADER_H_
#define FONT_LOADER_H_

/*
 * BUILD_FONT_RET_CODES
 *
 * BUILD_FONT_OK - Function was successful.
 * BUILD_FONT_LOAD_FAIL - Could not load file.
 * BUILD_FONT_RENDER_FAIL - Could not render one of the glyphs from the font.
 * BUILD_FONT_CREATE_TEXTURE_FAIL - Could not create opengl texture from the
 *                                  glyph.
 */
#define BUILD_FONT_OK                  0
#define BUILD_FONT_LOAD_FAIL           1
#define BUILD_FONT_RENDER_FAIL         2
#define BUILD_FONT_CREATE_TEXTURE_FAIL 3

struct font;

int create_font(char *, struct font **, int, int, SDL_Color);
void destroy_font(struct font *);

#endif /* FONT_LOADER_H_ */
