/*
 * pitch.c
 *
 *  Created on: 18 Oct 2009
 *      Author: David
 */
#include "dt_logger.h"

#include <time.h>
#include <SDL/SDL_opengl.h>
#include <stdlib.h>
#include "pitch.h"
#include "entity_graphic.h"
#include "rendering/draw_functions.h"

/*
 * calculate_pitch_positions
 *
 * Takes a pitch object with all the dimensions initialised and calculates where
 * all the lines should be relative to the top/back of one endzone.
 *
 * Parameters: pitch - The pitch object. Wil be updated with new line info.
 */
void calculate_pitch_positions(PITCH *pitch)
{
  /*
   * Sides of the pitch. Assume the first one is at 0.
   */
  pitch->side_1 = 0;
  pitch->side_2 = pitch->side_1 + pitch->width_m;

  /*
   * Left hand side endzone. Arbitrary choice as the camera can rotate the
   * pitch.
   */
  pitch->back_endzone_1 = 0;
  pitch->front_endzone_1 = pitch->back_endzone_1 + pitch->endzone_depth_m;

  /*
   * Right hand side endzone.
   */
  pitch->back_endzone_2 = pitch->back_endzone_1 + pitch->length_m;
  pitch->front_endzone_2 = pitch->back_endzone_2 - pitch->endzone_depth_m;
}

/*
 * create_pitch
 *
 * Creates a new pitch object allocating the required memory. Does not init any
 * of the variables. This must be done seperately before it is read.
 *
 * Parameters: tile_filename - The location of the grass tile object that will
 *                             be used as the background image for the pitch.
 *
 * Returns: A pointer to the new pitch object.
 */
PITCH *create_pitch(char *tile_filename)
{
  /*
   * Local Variables.
   */
  PITCH *new_pitch;
  int rc;

  /*
   * Allocate memory for the pitch object.
   */
  new_pitch = (PITCH *) DT_MALLOC(sizeof(PITCH));

  /*
   * Attempt to create a graphic object to represent the grass image.
   */
  rc = create_entity_graphic(tile_filename, &(new_pitch->grass_tile), true);
  add_reference_to_graphic(new_pitch->grass_tile);
  if (LOAD_SPRITE_OK != rc)
  {
    DT_DEBUG_LOG("Failed to create pitch object\n");
    new_pitch->grass_tile = NULL;
    destroy_pitch(new_pitch);
    new_pitch = NULL;
  }

  return(new_pitch);
}

/*
 * destroy_pitch
 *
 * Free the memory allocated for a pitch object. Use this rather than the free
 * command.
 *
 * Parameters: pitch - The pitch object to be destroyed.
 */
void destroy_pitch(PITCH *pitch)
{
  if (NULL != pitch->grass_tile)
  {
    remove_reference_to_graphic(pitch->grass_tile);
  }

  DT_FREE(pitch);
}

/*
 * set_pitch_line_color
 *
 * Sets the line color of the pitch by specifying all the values in the color
 * array.
 *
 * Parameters: pitch - The pitch whose line color we are setting.
 *             red, green, blue - The colors.
 *             alpha - The alpha value for the lines.
 */
void set_pitch_line_color(PITCH *pitch,
                          GLfloat red,
                          GLfloat green,
                          GLfloat blue,
                          GLfloat alpha)
{
  pitch->line_color[RGBA_RED_INDEX] = red;
  pitch->line_color[RGBA_GREEN_INDEX] = green;
  pitch->line_color[RGBA_BLUE_INDEX] = blue;
  pitch->line_color[RGBA_ALPHA_INDEX] = RGBA_ALPHA_DEFAULT;
}

/*
 * set_pitch_width
 *
 * Sets the width of the pitch. Fails the program if the width is not between
 * the defined max and min.
 *
 * Parameters: pitch
 *             width - The width in (m).
 */
void set_pitch_width(PITCH *pitch, int width)
{
  /*
   * Check the width is between the max and min.
   */
  assert((width <= PITCH_WIDTH_MAX) && (width >= PITCH_WIDTH_MIN));

  /*
   * Set the pitch width.
   */
  pitch->width_m = width;
}

/*
 * set_pitch_length
 *
 * Sets the length of the pitch. Fails the program if the length is not between
 * the defined max and min.
 *
 * Parameters: pitch
 *             length - The width in (m).
 */
void set_pitch_length(PITCH *pitch, int length)
{
  /*
   * Check the length is between the max and min.
   */
  assert((length <= PITCH_LENGTH_MAX) && (length >= PITCH_LENGTH_MIN));

  /*
   * Set the pitch length.
   */
  pitch->length_m = length;
}

/*
 * set_endzone_depth
 *
 * Sets the depth of the endzones. Fails the program if the depth is not between
 * the defined max and min.
 *
 * Parameters: pitch
 *             endzone_depth - The width in (m).
 */
void set_endzone_depth(PITCH *pitch, int endzone_depth)
{
  /*
   * Check the depth is between the max and min.
   */
  assert((endzone_depth <= ENDZONE_DEPTH_MAX) &&
      (endzone_depth >= ENDZONE_DEPTH_MIN));

  /*
   * Set the endzone depth.
   */
  pitch->endzone_depth_m = endzone_depth;
}

/*
 * set_line_width
 *
 * Sets the width of the pitch lines. Fails the program if the width is not
 * between the defined max and min.
 *
 * Parameters: pitch
 *             line_width - The width in (mm).
 */
void set_line_width(PITCH *pitch, float line_width)
{
  /*
   * Check the width is between the max and min.
   */
  assert((line_width <= LINE_WIDTH_MAX) && (line_width >= LINE_WIDTH_MIN));

  /*
   * Set the line width.
   */
  pitch->line_width_m = line_width;
}

/*
 * set_brick_mark_dist
 *
 * Sets the distance from the front of the endzones of the brick mark.
 *
 * Parameters: pitch - The pitch object to update.
 *             brick_mark_dist - The new brick mark distance.
 */
void set_brick_mark_dist(PITCH *pitch, int brick_mark_dist)
{
  pitch->brick_mark_dist_m = brick_mark_dist;
}

/*
 * init_pitch
 *
 * Sets the pitch size to random numbers inside the constraints detailed in the
 * pitch header file. Use either this OR init_pitch.
 *
 * Parameters - pitch
 */
void init_pitch(PITCH *pitch)
{
  /*
   * Reseed the random number generator so that we get different values each
   * time the function is run.
   */
  srand((unsigned int) time(0));

  /*
   * Get random integers between the maximum and minimum lengths for the pitch
   * dimensions.
   */
  pitch->length_m = PITCH_LENGTH_MIN;
  pitch->width_m = PITCH_WIDTH_MIN;
  pitch->endzone_depth_m = ENDZONE_DEPTH_MIN;

  /*
   * At the moment the line width is set to 1 so that we actually get a pitch.
   */
  pitch->line_width_m = 1.0;

  /*
   * Set the distance to the brick mark. This is a fixed distance so we do not
   * need to randomize it.
   */
  set_brick_mark_dist(pitch, BRICK_MARK_DIST);

  /*
   * Set the line color to be white.
   */
  pitch->line_color[RGBA_RED_INDEX] = RGBA_MAX_VAL;
  pitch->line_color[RGBA_GREEN_INDEX] = RGBA_MAX_VAL;
  pitch->line_color[RGBA_BLUE_INDEX] = RGBA_MAX_VAL;
  pitch->line_color[RGBA_ALPHA_INDEX] = RGBA_ALPHA_DEFAULT;

  /*
   * Use the new pitch dimensions to calculate the positions of the lines.
   */
  calculate_pitch_positions(pitch);

  /*
   * Log the values created.
   */
  DT_DEBUG_LOG("Pitch created: length %i, width %i, endzone_depth %i, line_width %f\n",
               pitch->length_m,
               pitch->width_m,
               pitch->endzone_depth_m,
               pitch->line_width_m);

  return;
}
