/*
 * animation.c
 *
 *  Created on: 18 Feb 2010
 *      Author: David Tyler
 */
#include "../dt_logger.h"

#include <math.h>

#include "animation.h"
#include "../player.h"
#include "../rendering/texture_loader.h"
#include "../entity_graphic.h"

/*
 * increment_animation_frame_counter
 *
 * Moves a player to the next frame in an animation.
 *
 * Parameters: player - The player whose animation we are incrementing.
 *             handler - The animation handler containing details on all
 *                       loaded animations.
 */
void increment_animation_frame_counter(PLAYER *player,
                                       ANIMATION_HANDLER *handler)
{
  /*
   * Each animation has a fixed number of frames per row. The player knows
   * what frame they are currently on and that cycles around using modulo
   * whatever the number of frames per row is.
   */
  player->curr_frame++;
  player->curr_frame = player->curr_frame %
              handler->animations[player->animation_choice]->num_frames_per_row;
}

/*
 * calculate_animation_direction
 *
 * Complex looking function that takes the current position of an object and
 * it's desired position. Returns the animation direction for something
 * travelling from position to desired position.
 *
 * Uses ratios rather than angles but this is essentially equivalent to
 * checking what angle the object will travel at and splitting that over the 8
 * different directions equally.
 *
 * So if the object is travelling from 0,0 to 1,5 then it will be travelling
 * away (0 is up on the screen) and this is calculated here by doing 0-1/0-5=0.2
 * and comparing that to pre calculated values for tangent(22.5 deg) and
 * tangent(47.5 deg).
 *
 * Parameters: position - The current position.
 *             desired_position - The position that the object is travelling
 *                                towards.
 *
 * Returns: The direction that the animation should be played for.
 */
ANIMATION_DIRECTION calculate_animation_direction(VECTOR3 *position,
                                                  VECTOR3 *desired_position)
{
  /*
   * Local Variables.
   */
  float x_diff = position->x - desired_position->x;
  float y_diff = position->y - desired_position->y;
  float ratio = (float) fabs(y_diff / x_diff);
  float inverse_ratio = (float) fabs(x_diff / y_diff);

  /*
   * Find which quadrant t
   */
  if (x_diff < 0 && y_diff < 0)
  {
    /*
     * First quadrant.
     */
    if (ratio < DIRECTION_OCTANT_1)
    {
      return(animate_away);
    }
    else
    {
      if (ratio < DIRECTION_OCTANT_2)
      {
        return(animate_away_right);
      }
      else
      {
        return(animate_right);
      }
    }
  }
  else if (x_diff < 0 && y_diff >= 0)
  {
    /*
     * Second Quadrant.
     */
    if (inverse_ratio < DIRECTION_OCTANT_1)
    {
      return(animate_right);
    }
    else
    {
      if (inverse_ratio < DIRECTION_OCTANT_2)
      {
        return(animate_right_towards);
      }
      else
      {
        return(animate_towards);
      }
    }
  }
  else if (x_diff >= 0 && y_diff >= 0)
  {
    /*
     * Third Quadrant.
     */
    if (ratio < DIRECTION_OCTANT_1)
    {
      return(animate_towards);
    }
    else
    {
      if (ratio < DIRECTION_OCTANT_2)
      {
        return(animate_left_towards);
      }
      else
      {
        return(animate_left);
      }
    }
  }
  else
  {
    /*
     * Fourth Quadrant.
     */
    if (ratio < DIRECTION_OCTANT_1)
    {
      return(animate_left);
    }
    else
    {
      if (ratio < DIRECTION_OCTANT_2)
      {
        return(animate_away_left);
      }
      else
      {
        return(animate_away);
      }
    }
  }
}

/*
 * create_animation
 *
 * Allocates the memory required for a animation and fills the animation object
 * based on the designated spritesheet.
 *
 * Parameters: filename - The location of the spritesheet.
 *             frame_width - The frame width and height is determined outside of
 *                           the spritesheet and is constant for all frames.
 *             frame_height
 *             animation - Will be returned as the new animation object.
 *
 * Returns: One of ANIMATION_CREATE_RET_CODES
 */
int create_animation(char *filename,
                     int frame_width,
                     int frame_height,
                     ANIMATION **animation)
{
  /*
   * Local Variables
   */
  int ret_code = ANIMATION_CREATE_OK;
  int rc;

  /*
   * Allocate the required memory
   */
  (*animation) = (ANIMATION *) DT_MALLOC(sizeof(ANIMATION));

  /*
   * Set up the resources.
   */
  rc = create_entity_graphic(filename, &((*animation)->sprite_sheet), false);
  if (LOAD_SPRITE_OK != rc)
  {
    if (LOAD_SPRITE_LOAD_FAIL == rc)
    {
      ret_code = ANIMATION_CREATE_FILE_LOAD_FAIL;
    }
    else
    {
      ret_code = ANIMATION_CREATE_POST_LOAD_FAIL;
    }

    goto EXIT_LABEL;
  }

  /*
   * The frame height and width are passed into this function and are constant
   * for all frames in the animation.
   */
  (*animation)->frame_height = frame_height;
  (*animation)->frame_width = frame_width;

  /*
   * The number of frames per row is determined by the total width of the sheet
   * and the width of each of the frames as passed in.
   */
  (*animation)->num_frames_per_row = (*animation)->sprite_sheet->width /
                                     frame_width;

EXIT_LABEL:

  return(ret_code);
}

/*
 * destroy_animation
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: animation - The object to be freed.
 */
void destroy_animation(ANIMATION *animation)
{
  /*
   * An animation has a single sprite sheet attached to it. If this is a
   * shared sprite (no guarantee it isn't) then we only want to remove a ref
   * to it. Otherwise we can free it.
   */
  if (NULL != animation->sprite_sheet)
  {
    remove_reference_to_graphic(animation->sprite_sheet);
  }

  /*
   * Free the object.
   */
  DT_FREE(animation);
}
