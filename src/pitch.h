/*
 * pitch.h
 *
 * Contains the pitch dimension constants.
 *
 *  Created on: 18 Oct 2009
 *      Author: David
 */

#ifndef PITCH_H_
#define PITCH_H_

#include <SDL/SDL_opengl.h>

struct entity_graphic;

/*
 * The following constants determine the dimensions of the pitch in (m). These
 * are used in the code and converted to pixels based on the size of the screen
 * that is being used.
 *
 * They are correct as of WFDF rules 2009.
 */
#define PITCH_WIDTH_MIN 37
#define PITCH_WIDTH_MAX 37
#define PITCH_LENGTH_MIN 100
#define PITCH_LENGTH_MAX 100
#define ENDZONE_DEPTH_MIN 18
#define ENDZONE_DEPTH_MAX 18
#define LINE_WIDTH_MIN 0.075f
#define LINE_WIDTH_MAX 0.120f
#define BRICK_MARK_DIST 20

/*
 * PITCH
 *
 * This structure contains the pitch dimensions and orientation in terms of
 * real world distances.
 *
 * width_m - The width of the pitch in (m).
 * length_m - The length of the pitch (including endzones) in (m).
 * endzone_depth_m - The depth of the endzones in (m) (forced to be the same for
 *                   both endzones).
 * brick_mark_dist_m - The distance from the front of the endzones that the
 *                     brick marks is placed.
 * line_width_m  - The width of the lines marking the pitch in (m).
 * line_color - A pointer to an array of colors. Must include alpha values and
 *              have color in ranges 0 - 255.
 * grass_tile - The tile with which to draw the background of the pitch.
 */
typedef struct pitch
{
  int width_m;
  int length_m;
  int endzone_depth_m;
  int brick_mark_dist_m;
  float line_width_m;
  GLfloat line_color[4];
  int side_1;
  int side_2;
  int back_endzone_1;
  int front_endzone_1;
  int front_endzone_2;
  int back_endzone_2;
  struct entity_graphic *grass_tile;
} PITCH;

PITCH *create_pitch(char *);
void destroy_pitch(PITCH *);
void init_pitch(PITCH *);

#endif /* PITCH_H_ */
