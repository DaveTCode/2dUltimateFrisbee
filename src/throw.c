/*
 * throw.c
 *
 *  Created on: 23 Oct 2009
 *      Author: David
 */

#include "dt_logger.h"

#include <math.h>
#include "throw.h"
#include "throw_constants.h"
#include "mouse_click_state.h"
#include "disc.h"
#include "data_structures/vector.h"

DISC_THROW *create_throw()
{
  DISC_THROW *throw;

  /*
   * Create the memory required for the the throw object.
   */
  throw = (DISC_THROW *) DT_MALLOC(sizeof(DISC_THROW));

  /*
   * Create the memory required for each of the mouse state_objects related to
   * this throw.
   */
  throw->start_state = create_mouse_state();
  throw->end_state = create_mouse_state();

  /*
   * Throw defaults to NOT being in progress.
   */
  throw->in_progress = false;

  return(throw);
}

/*
 * destroy_throw
 *
 * Free the memory used in a throw object.
 *
 * Parameters: throw - The object to be freed.
 */
void destroy_throw(DISC_THROW *throw)
{
  /*
   * First free the mouse states associated with this object.
   */
  destroy_mouse_state(throw->start_state);
  destroy_mouse_state(throw->end_state);

  /*
   * Free the throw object itself.
   */
  DT_FREE(throw);
}

/*
 * convert_throw_to_velocity
 *
 * Takes a throw object and returns the velocity that the disc should begin
 * with for that throw.
 *
 * This is one of the drop in conversion routines that can easily be replaced
 * in order to change the way user input affects a throw.
 */
VECTOR3 convert_throw_to_velocity(DISC *disc, DISC_THROW *throw)
{
  /*
   * Local Variables.
   */
  float x_diff = ((float) throw->start_state->world_x) - disc->position.x;
  float y_diff = ((float) throw->start_state->world_y) - disc->position.y;
  float time_diff = (float) (throw->end_state->create_time -
                             throw->start_state->create_time);
  float adj_time_diff;
  float total_speed;
  float xy_ratio;
  float alpha;
  VECTOR3 velocity;
  float temp_length;

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
  else
  {
    adj_time_diff = time_diff;
  }

  /*
   * The total speed is calculated using a ratio between the length of time the
   * button is held down for and the actual velocity.
   *
   * This is set to a minimum final speed to give us a known state.
   */
  total_speed = (adj_time_diff * RATIO_BUTTON_TIME_TO_SPEED) + MIN_DISC_SPEED;

  /*
   * This works by utilising the fact that the velocity vector and the x,y
   * coordinates of the mouse make 2 similiar triangles with one point at the
   * origin.
   *
   * We calculate the length of the hypotenuse of the mouse coordinate triangle
   * which has one point at the disc, one at the click location and one on the
   * x axis to make it a right angle triangle.
   *
   * This can then be used to calculate the lengths of the triangle made by
   * placing the velocity along that hypotenuse and calculating the x and y
   * components.
   */
  temp_length = sqrtf(x_diff * x_diff + y_diff * y_diff);
  velocity.x = total_speed * x_diff / temp_length;
  velocity.y = total_speed * y_diff / temp_length;

  /*
   * For now we set the z velocity to 0.
   */
  velocity.z = 0;

  return(velocity);
}
