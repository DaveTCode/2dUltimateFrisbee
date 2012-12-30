/*
 * animation_handler.h
 *
 *  Created on: 24 Feb 2010
 *      Author: David Tyler
 */

#ifndef ANIMATION_HANDLER_H_
#define ANIMATION_HANDLER_H_

struct animation;

/*
 * LOAD_ANIMATION_DATA_RET_CODES
 *
 * LOAD_ANIMATION_DATA_OK - The function was successful
 * LOAD_ANIMATION_DATA_INVALID_ANIMATION - There was a value in the animation
 *                                         choice animation that did not have a
 *                                         corresponding sprite sheet filename.
 * LOAD_ANIMATION_DATA_CREATE_FAILED - The loading and conversion of sprite
 *                                     sheet failed.
 */
#define LOAD_ANIMATION_DATA_OK                0
#define LOAD_ANIMATION_DATA_INVALID_ANIMATION 1
#define LOAD_ANIMATION_DATE_CREATE_FAIL       2

/*
 * ANIMATION_CHOICE
 *
 * An enumeration of all the different animations. Each of these corresponds to
 * a single animation object in the animation handler.
 */
typedef enum animation_choice
{
  running_animation,
  standing_animation,
  jumping_animation,
  num_animations
} ANIMATION_CHOICE;

/*
 * ANIMATION_DIRECTION
 *
 * We enumerate the different directions that an animation can happen in. If
 * the ordering of these is changed in any way then the animations will appear
 * at the wrong times as the enum values are used to tell us which row to look
 * at in the sprite sheet.
 */
typedef enum animation_direction
{
  animate_away,
  animate_away_right,
  animate_right,
  animate_right_towards,
  animate_towards,
  animate_left_towards,
  animate_left,
  animate_away_left,
  num_animation_directions
} ANIMATION_DIRECTION;

/*
 * ANIMATION_HANDLER
 *
 * The animation handler is what is passed around so that the draw functions
 * have access to the animations as determined in the player object.
 *
 * animations - An array of animation objects. One entry for each entry in the
 *              enumeration of animation choices.
 */
typedef struct animation_handler
{
  struct animation *animations[num_animations];
} ANIMATION_HANDLER;

ANIMATION_HANDLER *create_animation_handler();
void destroy_animation_handler(ANIMATION_HANDLER *);
int load_animation_data(ANIMATION_HANDLER *);
struct animation *handler_get_animation(ANIMATION_HANDLER *, ANIMATION_CHOICE);

#endif /* ANIMATION_HANDLER_H_ */
