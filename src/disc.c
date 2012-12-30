/*
 * disc.c
 *
 *  Created on: 21 Oct 2009
 *      Author: David
 */

#include "dt_logger.h"

#include <stddef.h>

#include "data_structures/vector.h"
#include "disc.h"
#include "entity_graphic.h"
#include "mouse_click_state.h"
#include "flight_mechanics/disc_dimensions.h"
#include "rendering/texture_loader.h"
#include "throw.h"
#include "throw_constants.h"

/*
 * create_disc
 *
 * Allocate the memory required for a disc object and its underlying physics
 * object then return the result.
 *
 * Returns: A pointer to the new object.
 */
DISC *create_disc(char *graphic_filename)
{
  DISC *disc;
  int rc;

  /*
   * Allocate the required memory for the disc object.
   */
  disc = (DISC *) DT_MALLOC(sizeof(DISC));

  /*
   * This would be moved to a config file implementation.
   */
  disc->diameter_m = DISC_DIAMETER_M;
  disc->thickness_m = DISC_THICKNESS_M;

  /*
   * Create the position, velocity and orientation vectors. d3 starts out
   * pointing directly upwards.
   */
  vector_set_values(&(disc->d3), 0.0f, 0.0f, 1.0f);
  vector_set_values(&(disc->position), 0.0f, 0.0f, 0.0f);
  vector_set_values(&(disc->new_position), 0.0f, 0.0f, 0.0f);
  vector_set_values(&(disc->velocity), 0.0f, 0.0f, 0.0f);
  vector_set_values(&(disc->angular_velocity), 0.0f, 0.0f, 0.0f);

  /*
   * The disc starts off with no previous thrower or current holder.
   */
  disc->thrower = NULL;
  disc->holder = NULL;

  /*
   * The disc graphic is created in this function so that it can be released
   * when the disc is destroyed.
   *
   * Don't try and recover if the disc graphic could not be found.
   *
   * Special case code exists if the filename is NULL so that we can create a
   * disc with no graphic.
   */
  if (NULL != graphic_filename)
  {
    rc = create_entity_graphic(graphic_filename, &(disc->graphic), false);
    add_reference_to_graphic(disc->graphic);
    if (rc != LOAD_TEXTURE_OK)
    {
      game_exit("Could not load disc image.");
    }
  }
  else
  {
    disc->graphic = NULL;
  }

  return(disc);
}

/*
 * destroy_disc
 *
 * Free the memory used for a disc object.
 *
 * Parameters: disc - The object to be freed.
 */
void destroy_disc(DISC *disc)
{
  /*
   * Reduce the reference count on the graphic used for the disc. This will
   * free up the resource used in creating the texture provided that the disc
   * is the only object using the sprite.
   */
  if (NULL != disc->graphic)
  {
    remove_reference_to_graphic(disc->graphic);
  }

  /*
   * Then free the memory used for the disc object itself.
   */
  DT_FREE(disc);
}

/*
 * copy_disc_values
 *
 * Makes an exact replica of a disc object.
 *
 * replica - The disc object into which values will be copied.
 * old_disc - The disc which will be copied.
 */
void create_disc_replica(DISC *replica, DISC *old_disc)
{
  vector_copy_values(&(replica->angular_velocity), &(old_disc->angular_velocity));
  vector_copy_values(&(replica->new_position), &(old_disc->new_position));
  vector_copy_values(&(replica->d3), &(old_disc->d3));
  vector_copy_values(&(replica->position), &(old_disc->position));
  vector_copy_values(&(replica->velocity), &(old_disc->velocity));
  replica->diameter_m = old_disc->diameter_m;
  replica->disc_state = old_disc->disc_state;
  replica->rotation_angle_rad = old_disc->rotation_angle_rad;
  replica->thickness_m = old_disc->thickness_m;
}

/*
 * set_init_disc_conditions
 *
 * When we complete a throw action this function is called to set the initial
 * physical properties of the disc. It also sets the disc to be in the air so
 * that the physics processing routine knows to attempt to calculate its
 * position as time increases.
 *
 * Parameters: disc - The disc object.
 *             throw - The completed throw object that caused this throw.
 */
void set_init_disc_conditions(DISC *disc, DISC_THROW *throw)
{
  /*
   * Local Variables.
   */
  float x_diff = ((float) throw->start_state->world_x) - disc->velocity.x;
  float y_diff = ((float) throw->start_state->world_y) - disc->velocity.y;
  float time_diff = (float) (throw->end_state->create_time -
                             throw->start_state->create_time);
  float adj_time_diff;

  /*
   * The time difference must be between the maximum and minimum time values.
   * If it isn't then force it to be so that the user can't over power a throw.
   */
  if (time_diff > MAX_TIME_POWER_BUTTON_HELD)
  {
    adj_time_diff = MAX_TIME_POWER_BUTTON_HELD;
  }
  else if (time_diff < MIN_TIME_POWER_BUTTON_HELD)
  {
    adj_time_diff = MIN_TIME_POWER_BUTTON_HELD;
  }

  /*
   * Get the disc velocity from the x, y and time differences.
   */
  disc->velocity = convert_throw_to_velocity(disc, throw);

  /*
   * Calculate the initial speed of the disc.
   * @@@DAT - Dummy function.
   */
  disc->angular_velocity.x = -13.94f;
  disc->angular_velocity.y = -1.48f;
  disc->angular_velocity.z = -54.25f;
  disc->position.z = 1.5f;
  disc->d3.x = 0.0f;
  disc->d3.y = 0.0f;
  disc->d3.z = 1.0f;
  vector_copy_values(&(disc->new_position), &(disc->position));

  disc->disc_state = disc_in_air;
}
