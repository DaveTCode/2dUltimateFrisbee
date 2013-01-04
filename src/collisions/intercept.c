/*
 * intercept.c
 *
 *  Created on: 25 May 2010
 *      Author: David Tyler
 */

#include "../dt_logger.h"

#include "intercept.h"
#include "../conversion_constants.h"
#include "../data_structures/vector.h"
#include "../disc_path.h"
#include "../player.h"

/*
 * create_intercept
 *
 * Allocates the memory required for a intercept
 *
 * Returns: A pointer to the newly created memory
 */
INTERCEPT *create_intercept()
{
  /*
   * Local Variables
   */
  INTERCEPT *intercept;

  /*
   * Allocate the required memory
   */
  intercept = (INTERCEPT *) DT_MALLOC(sizeof(INTERCEPT));

  /*
   * The positions of the two objects are references and are set to NULL until
   * this intercept is calculated.
   */
  intercept->object_a = NULL;
  intercept->object_b = NULL;

  return(intercept);
}

/*
 * destroy_intercept
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: intercept - The object to be freed.
 */
void destroy_intercept(INTERCEPT *intercept)
{
  /*
   * Free the object.
   */
  DT_FREE(intercept);
}

/*
 * calculate_player_disc_path_intercept
 *
 * Takes a player and a disc path and calculates when the player can first
 * intercept the disc if they are running at 100% for the whole time.
 *
 * All times are done in seconds from the current time in this function.
 *
 * Parameters: player
 *             disc_path
 *             intercept - Will be returned containing the intercept vectors.
 *                         Should already have been created.
 *
 * Returns: True if the player can intercept the disc at some point along the
 *          path and false otherwise.
 */
bool calculate_player_disc_path_intercept(PLAYER *player,
                                          DISC_PATH *disc_path,
                                          INTERCEPT *intercept)
{
  /*
   * Local Variables.
   */
  float landing_time = disc_path->end_position->t;
  float max_player_travel_dist;
  float current_time = SDL_GetTicks() / MILLISECONDS_PER_SECOND;
  DISC_POSITION *disc_position;
  float distance;
  bool found_intercept = false;
  float time_to_reach = 0.0;

  /*
   * Calculate the maximum distance that the player can travel between now and
   * the time at which the disc lands.
   */
  max_player_travel_dist = player->max_speed * (landing_time - current_time);

  /*
   * For each point along the disc path decide whether it is inside that radius,
   * whether the player can reach the disc at that point and whether they will
   * arrive there in time to get it. The first along the path for which all
   * these are true for is the first intercept point.
   */
  disc_position = disc_path->start_position;
  while(NULL != disc_position)
  {
    /*
     * Calculate the distance between the player position and the disc position
     * as two vectors projected down onto R2.
     */
    distance = dist_between_vectors_2d(&(player->position),
                                       &(disc_position->position));

    if (distance <= max_player_travel_dist)
    {
      /*
       * Check that the player can reach the disc at that point.
       */
      if (player->height >= disc_position->position.z)
      {
        /*
         * Calculate when the player will reach that point. If it is before the
         * disc then we have found an intercept point.
         */
        time_to_reach = distance / player->max_speed;
        if (time_to_reach <= disc_position->t)
        {
          found_intercept = true;
          break;
        }
      }
    }

    disc_position = disc_position->next;
  }

  /*
   * There is no guarantee that we found an intercept point. If we did then
   * fill it in here.
   */
  if (found_intercept)
  {
    intercept->object_a = &(player->position);
    intercept->object_b = &(disc_position->position);
    intercept->t = time_to_reach;
  }

  return(found_intercept);
}
