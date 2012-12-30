/*
 * animation.h
 *
 *  Created on: 18 Feb 2010
 *      Author: David Tyler
 */

#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "animation_handler.h"

struct entity_graphic;
struct player;
struct vector3;

/*
 * If you split the whole of R2 into 8 equal segments with infinite straight
 * lines passing through the origin all of which have equal angles between them
 * then that angle is 360 / 8 = 45.
 *
 * Add the additional requirement that the x,y axis act as angle bisectors and
 * therefore there are two such lines in the first quadrant (x,y >= 0). These
 * two lines are on angles 22.5 and 67.5.
 *
 * The below values are TAN(alpha) where alpha is one of those two angles.
 *
 * They are used to determine which of 8 animation directions to pick for
 * various animations.
 */
#define DIRECTION_OCTANT_1 0.414f
#define DIRECTION_OCTANT_2 2.414f

/*
 * ANIMATION_CREATE_RET_CODES
 *
 * Return codes for the create_animation function
 *
 * ANIMATION_CREATE_OK - The function was successful.
 * ANIMATION_CREATE_FILE_LOAD_FAIL - The file could not be loaded.
 * ANIMATION_CREATE_POST_LOAD_FAIL - The conversion of the loaded image into an
 *                                   opengl texture failed.
 */
#define ANIMATION_CREATE_OK             0
#define ANIMATION_CREATE_FILE_LOAD_FAIL 1
#define ANIMATION_CREATE_POST_LOAD_FAIL 2

/*
 * ANIMATION
 *
 * An object that corresponds to a single animation. Contains a sprite sheet
 * which has all the frames for the animation. Each row in the sheet corresponds
 * to a single direction and they must all have the same number of frames.
 *
 * sprite_sheet - The sprite sheet itself. This will contain in rows all the
 *                frames for a given direction and there will be one row per
 *                direction.
 * num_frames_per_row - The number of frames in each of the rows. This is
 *                      constant for all rows.
 * frame_width - The width of the sprite in each frame.
 * frame_height - The height of the sprite in each frame.
 */
typedef struct animation
{
  struct entity_graphic *sprite_sheet;
  int num_frames_per_row;
  int frame_width;
  int frame_height;
} ANIMATION;

int create_animation(char *, int, int, ANIMATION **);
void destroy_animation(ANIMATION *);
void increment_animation_frame_counter(struct player *,
                                       struct animation_handler *);
ANIMATION_DIRECTION calculate_animation_direction(struct vector3 *,
                                                  struct vector3 *);

#endif /* ANIMATION_H_ */
