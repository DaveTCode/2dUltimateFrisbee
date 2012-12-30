/*
 * disc_path.c
 *
 *  Created on: 14 Dec 2009
 *      Author: David Tyler
 */

#include "dt_logger.h"

#include <stdbool.h>
#include <stddef.h>
#include "disc_path.h"
#include "disc.h"
#include "data_structures/vector.h"

/*
 * create_disc_path
 *
 * Allocates the memory required for a disc_path
 *
 * Returns: A pointer to the newly created memory
 */
DISC_PATH *create_disc_path()
{
  /*
   * Local Variables
   */
  DISC_PATH *disc_path;

  /*
   * Allocate the required memory
   */
  disc_path = (DISC_PATH *) DT_MALLOC(sizeof(DISC_PATH));

  return(disc_path);
}

/*
 * destroy_disc_path
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: disc_path - The object to be freed.
 */
void destroy_disc_path(DISC_PATH *disc_path)
{
  /*
   * Free the object.
   */
  DT_FREE(disc_path);
}

/*
 * init_disc_path
 *
 * Takes a disc object (which knows where the disc is), a new disc path object
 * and an interval in seconds and fills in the path of the discs flight using
 * a time stepped path.
 *
 * This can then be used to guess where the disc will be at each given moment
 * along the path.
 *
 * It is expensive as it creates a new disc position for each step along the
 * path (calling malloc once per position). This must also be freed.
 *
 * Parameters: disc - The game disc.
 *             disc_path - If this already contains a disc path then it will
 *                         leak memory as this function will not free it.
 *             interval - Interval between path positions in seconds.
 */
void init_disc_path(DISC *disc, DISC_PATH *disc_path, float interval)
{
  /*
   * Local Variables.
   */
  DISC temp_disc;
  DISC_POSITION *curr_position = NULL;
  DISC_POSITION *prev_position = NULL;
  int num_positions = 0;

  /*
   * Copy the disc object so that we can modify it using the standard disc
   * physics functions.
   *
   * We can't just memcpy the whole structure here or we would end up using
   * the same vector positions (copying pointers).
   */
  create_disc_replica(&temp_disc, disc);

  /*
   * Create new disc positions for each interval until the disc hits the floor.
   *
   * There is fail safe code in here which stops once a maximum number of path
   * steps have been allocated.
   *
   * !!!NOTE THAT MEMORY IS ALLOCATED IN THIS LOOP!!!
   */
  while (temp_disc.position.z > 0.0f &&
         num_positions <= MAX_DISC_PATH_ELEMENTS)
  {
    /*
     * Create a new disc position (allocates memory) and set the values to be
     * equal to the temporary disc objects current position.
     */
    curr_position = create_disc_position();
    curr_position->t = ((float) num_positions) * interval;
    vector_copy_values(&(curr_position->position), &(temp_disc.position));

    /*
     * If this isn't the first time through then set the previous position to
     * point to this new one.
     */
    if (prev_position != NULL)
    {
      prev_position->next = curr_position;
    }
    else
    {
      /*
       * If the previous position is NULL then this is the first node so set
       * the start position.
       */
      disc_path->start_position = curr_position;
    }

    /*
     * Update the temporary disc objects position (and other vectors associated
     * with the flight path calculation).
     */
    calculate_disc_position(&temp_disc, interval, false);

    prev_position = curr_position;
    num_positions++;
  }

  /*
   * Fill in the end of the disc path so that we can quickly find where it
   * ends. This will be the last position BEFORE the disc hits the floor.
   */
  disc_path->end_position = curr_position;
}

/*
 * create_disc_position
 *
 * Allocates the memory required for a disc_position
 *
 * Returns: A pointer to the newly created memory
 */
DISC_POSITION *create_disc_position()
{
  /*
   * Local Variables
   */
  DISC_POSITION *disc_position;

  /*
   * Allocate the required memory
   */
  disc_position = (DISC_POSITION *) DT_MALLOC(sizeof(DISC_POSITION));
  disc_position->next = NULL;

  return(disc_position);
}

/*
 * destroy_disc_position
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: disc_position - The object to be freed.
 */
void destroy_disc_position(DISC_POSITION *disc_position)
{
  /*
   * Free the object.
   */
  DT_FREE(disc_position);
}

/*
 * destroy_disc_position_list
 *
 * Free the entire of a linked list of disc positions.
 *
 * Parameters: start_position - The beginning of the list. May be null.
 */
void destroy_disc_position_list(DISC_POSITION *start_position)
{
  /*
   * Local Variables.
   */
  DISC_POSITION *curr_position = start_position;
  DISC_POSITION *next_position;

  /*
   * Iterate over the linked list of disc positions freeing them all.
   */
  while (curr_position != NULL)
  {
    next_position = curr_position->next;
    destroy_disc_position(curr_position);
  }
}

/*
 * disc_position_at_time
 *
 * Finds where the disc will be at time t where t is a millisecond value as
 * returned by SDL_GetTicks.
 *
 * Parameters: disc_path - The path of the disc that we want to use.
 *             t - The number of milliseconds.
 *             pos_x, pos_y, pos_z - Will be filled in with the return values.
 *
 * Returns: One of DISC_POSITION_CALC_RET_CODES.
 */
int disc_position_at_time(DISC_PATH *disc_path,
                          Uint32 t,
                          DISC_POS_CALC_TYPE calc_type,
                          VECTOR3 *result)
{
  /*
   * Local Variables.
   */
  DISC_POSITION *curr_position;
  DISC_POSITION *position_before;
  DISC_POSITION *position_after;
  int ret_code = DISC_POSITION_CALC_OK;
  Uint32 delta_t;
  int num_intervals;
  int ii;

  /*
   * Check the input variables.
   */
  DT_ASSERT(disc_path != NULL);
  DT_ASSERT(disc_path->start_position != NULL);
  if (t < disc_path->time_created)
  {
    ret_code = DISC_POSITION_CALC_INPUT_TIME_BEFORE;
    goto EXIT_LABEL;
  }

  /*
   * If the time entered is after the disc will have stopped then we just return
   * the finish position for the path and set a return code so that the calling
   * function can handle it. This is NOT an error.
   */
  if (t >= disc_path->time_to_stop)
  {
    vector_copy_values(result, &(disc_path->end_position->position));
    ret_code = DISC_POSITION_CALC_DISC_STOPPED;
  }

  /*
   * Find the number of intervals between the start of the disc path and the
   * end.
   *
   * The division below is integer division.
   */
  delta_t = t - disc_path->time_created;
  num_intervals = delta_t / disc_path->interval;

  /*
     * Find the position that the disc is in after that number of intervals. We
     * know that this should exist because we have already checked that the time
     * Inputed lies between the start time and the end time of the disc path.
     */
    curr_position = disc_path->start_position;
    for (ii = 0; ii < num_intervals; ii++)
    {
      curr_position = curr_position->next;
      DT_ASSERT(curr_position != NULL);
    }
    position_before = curr_position;
    position_after = curr_position->next;

  /*
   * There are various methods that we can use for determining what positions
   * to return back. These are enumerated below and each have their own comment
   * explaining what and why.
   *
   * INTERPOLATE - A simple linear interpolation between the two intervals
   *               either side of the required time.
   * NEAREST - The closest of the two pre calculated intervals.
   * PREVIOUS - The interval before the required time.
   * NEXT - The interval after the required time.
   */
  switch(calc_type)
  {
    case INTERPOLATE:
      disc_path_find_interp_distance(disc_path,
                                     position_before,
                                     position_after,
                                     t,
                                     num_intervals * disc_path->interval,
                                     result);
      break;

    case NEAREST:
      disc_path_find_nearest_pos(disc_path,
                                 position_before,
                                 position_after,
                                 t,
                                 num_intervals * disc_path->interval,
                                 result);
      break;

    case PREVIOUS:
      vector_copy_values(result, &(position_before->position));
      break;

    case NEXT:
      vector_copy_values(result, &(position_after->position));
      break;

    default:
      ret_code = DISC_POSITION_CALC_BAD_CALC_TYPE;
      break;
  }

  EXIT_LABEL:

  return(ret_code);
}

/*
 * disc_position_at_time_nearest
 *
 * Given a disc path and a time t we want to find which of the pre calculated
 * intervals is the nearest to the desired.
 *
 * If t goes beyond the amount of time the disc path exists for then we return
 * the last position calculated.
 *
 * DO NOT CALL DIRECTLY - This is called by the general disc_position_at_time
 * function and does not include any error checking on variables.
 *
 * Parameters: disc_path - The path of the disc. Must be non null.
 *             before - The last pre calculated interval before the time t.
 *             after - The next pre calculated point in the path after the
 *                     before variable.
 *             t - The time that we want to know the position for.
 *             time_at_position_before - The absolute time in ms at the
 *                                       pre calculated value before.
 *             result - Will contain the nearest position as a vector.
 */
void disc_path_find_nearest_pos(DISC_PATH *disc_path,
                                DISC_POSITION *before,
                                DISC_POSITION *after,
                                Uint32 t,
                                Uint32 time_at_position_before,
                                VECTOR3 *result)
{
  /*
   * Determine which of two positions is the closest to the required time.
   */
  if (t - time_at_position_before < (Uint32) (disc_path->interval / 2))
  {
    vector_copy_values(result, &(before->position));
  }
  else
  {
    vector_copy_values(result, &(after->position));
  }
}

/*
 * disc_path_find_interp_distance
 *
 * Given two adjacent position in a disc path, and a time that is somewhere
 * between them this function can be used to give a linearly interpolated
 * guess of where the disc will be in x,y,z coordinates at the time t.
 *
 * IMPORTANT - This function has no error checking and should NEVER be called
 * directly. Always use the generic function disc_position_at_time instead.
 *
 * Parameters: disc_path - Contains the information like interval time.
 *             before - The last pre calculated interval before the time t.
 *             after - The next pre calculated point in the path after the
 *                     before variable.
 *             t - The time in milliseconds. Must be between before and after.
 *             time_at_position_before - The absolute time in ms at the
 *                                       pre calculated value before.
 *             x, y, z - Will contain the interpolated position.
 */
void disc_path_find_interp_distance(DISC_PATH *disc_path,
                                    DISC_POSITION *before,
                                    DISC_POSITION *after,
                                    Uint32 t,
                                    Uint32 time_at_position_before,
                                    VECTOR3 *result)
{
  /*
   * Local Variables
   */
  float scaling_factor = (float) (disc_path->interval / 
                                  (t - time_at_position_before));

  /*
   * Calculate the linear interpolated value of x,y and z using the ratio of
   * the distances.
   */
  result->x = before->position.x +
                    (after->position.x - before->position.x) * scaling_factor;
  result->y = before->position.y +
                    (after->position.y - before->position.y) * scaling_factor;
  result->z = before->position.z +
                    (after->position.z - before->position.z) * scaling_factor;
}
