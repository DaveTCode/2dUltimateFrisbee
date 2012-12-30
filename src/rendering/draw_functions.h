/*
 * draw_functions.h
 *
 *  Created on: 17 Nov 2009
 *      Author: David
 */

#ifndef DRAW_FUNCTIONS_H_
#define DRAW_FUNCTIONS_H_

#include "font_structures.h"

struct disc;
struct player;
struct screen;
struct animation_handler;
struct pitch;
struct font;

/*
 * These are used as indices into color arrays so must be kept as they are.
 *
 * TODO - Is there an opengl defined value for these already?
 */
#define RGBA_RED_INDEX 0
#define RGBA_GREEN_INDEX 1
#define RGBA_BLUE_INDEX 2
#define RGBA_ALPHA_INDEX 3

/*
 * The minimum and maximum values taken by the glColor*f* functions.
 */
#define RGBA_MIN_VAL -1.0
#define RGBA_MAX_VAL 1.0

/*
 * The default alpha value for opaque drawings.
 */
#define RGBA_ALPHA_DEFAULT 1.0

void draw_disc(struct disc *, float);
void draw_player(struct player *, struct animation_handler *);
void draw_pitch_background(struct screen *, struct pitch *);
void draw_pitch_lines(struct screen *, struct pitch *);
void draw_text(char *, int, int, struct font *);

#endif /* DRAW_FUNCTIONS_H_ */
