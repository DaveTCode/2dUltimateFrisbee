/*
 * physics.c
 *
 * This file contains all the physics processing functions that recalculate
 * positions.
 *
 *  Created on: 26 Oct 2009
 *      Author: David
 */
#include "dt_logger.h"

#include "SDL/SDL_stdinc.h"

#include "conversion_constants.h"
#include "data_structures/vector.h"
#include "disc.h"
#include "disc_path.h"
#include "flight_mechanics/disc_flight.h"
#include "flight_mechanics/disc_forces.h"
#include "game_functions.h"
#include "match_state.h"
#include "player.h"
#include "team.h"

/*
 * update_vector_from_differential
 *
 * Takes a vector v1 and does v1 + dv1/dt * dt. This is equivalent to using
 * the acceleration to update the velocity after a fixed timestep or using the
 * velocity to update the position after a fixed timestep.
 *
 * Parameters: vector - The vector to be modified.
 *             differential - The differential with respect to time of vector.
 *             time_step - The number of s to process for.
 */
void update_vector_from_differential(VECTOR3 *vector,
                                     VECTOR3 *differential,
                                     float time_step_s)
{
  vector->x += differential->x * time_step_s;
  vector->y += differential->y * time_step_s;
  vector->z += differential->z * time_step_s;
}

/*
 * calc_velocity_towards_position
 *
 * Calculates the velocity vector for travelling at the given speed
 * to the given location.
 *
 * position - The current position.
 * desired_position - The position to run towards.
 * max_speed - The maximum speed of the entity.
 * speed_percent - The percentage of the maximum speed to run at.
 *
 * Returns: A velocity vector from the current position to the desired position
 *          travelling at the speed dictated by speed percentage and max_speed.
 */
VECTOR3 calc_velocity_towards_position(VECTOR3 *position,
                                      VECTOR3 *desired_position,
                                      float max_speed,
                                      float speed_percent)
{
  /*
   * Local Variables.
   */
  VECTOR3 velocity;
  VECTOR3 unit_direction;
  float relative_speed;

    /*
   * Move in a straight line towards the location set as the player run
   * direction.
   */
  velocity.x = desired_position->x - position->x;
  velocity.y = desired_position->y - position->y;
  velocity.z = 0;

  /*
   * Normalise the player vector as we want a unit vector along the direction
   * of travel.
   */
  unit_direction = unit_vector(&velocity);

  /*
   * Compute the players relative speed based on the speed percentage and their
   * top speed.
   */
  relative_speed = max_speed * (speed_percent / 100.0f);

  /*
   * Adjust the vector based on the players speed. This will need work later
   * when speed is not just constant!
   */
  velocity = mult_vector_by_scalar(&unit_direction, relative_speed);

  return velocity;
}

/*
 * calculate_disc_position
 *
 * This is the function that contains the logic for moving the disc around in
 * 3d world space (meters). If a better model is decided on then this is its
 * starting point.
 *
 * Parameters: disc - The disc that we are moving.
 *             time_step_s - The amount of time (s) since the last update.
 *             use_new_position - This should be set to true when actually
 *                                moving the disc as it forces the disc to put
 *                                the calculated value in the new_position
 *                                variable.
 *                                If false it allows the position to be updated
 *                                directly (bypassing collision detection) and
 *                                this is used for creating flight paths.
 */
void calculate_disc_position(DISC *disc,
                             float time_step_s,
                             bool use_new_position)
{
  /*
   * Local Variables.
   */
  VECTOR3 d1;
  VECTOR3 acceleration;
  DISC_FORCES disc_forces;
  float alpha;

  d1 = find_disc_plane_axis(&(disc->velocity), &(disc->d3));
  alpha = smallest_angle_between_vectors(&(disc->velocity), &d1);
  if (vector_dot_product(&(disc->velocity), &(disc->d3)) >= 0)
  {
    alpha = -1.0f * alpha;
  }
  disc_forces = calc_disc_forces(&(disc->velocity),
                                 &(disc->angular_velocity),
                                 alpha);
  get_disc_acc_from_forces(&disc_forces,
                           &(disc->velocity),
                           &acceleration,
                           &d1,
                           &(disc->d3));
  update_extra_vels_from_disc_forces(disc, &disc_forces, time_step_s);
  /**(disc->d3) = update_disc_up_axis(disc->d3,
                                    disc->velocity,
                                    disc->angular_velocity->y,
                                    disc->angular_velocity->x,
                                    time_step_s);*/

  DT_DEBUG_LOG("Disc velocity pre update: (%f, %f, %f)\n",
               disc->velocity.x,
               disc->velocity.y,
               disc->velocity.z);
  DT_DEBUG_LOG("Disc position pre update: (%f, %f, %f)\n",
                 disc->position.x,
                 disc->position.y,
                 disc->position.z);

  /*
   * Having done all the complex calculations update the velocity and new
   * position vectors.
   *
   * Note that we update the new position vector rather than the position
   * vector itself as that is done by the collision handler after it has
   * verified that no collisions have taken place.
   */
  update_vector_from_differential(&(disc->velocity), &acceleration, time_step_s);
  DT_DEBUG_LOG("Disc velocity post update: (%f, %f, %f)\n",
               disc->velocity.x,
               disc->velocity.y,
               disc->velocity.z);
  if (use_new_position)
  {
    update_vector_from_differential(&(disc->new_position),
                                    &(disc->velocity),
                                    time_step_s);
    DT_DEBUG_LOG("Disc position post update: (%f, %f, %f)\n",
                   disc->new_position.x,
                   disc->new_position.y,
                   disc->new_position.z);
  }
  else
  {
    update_vector_from_differential(&(disc->position),
                                    &(disc->velocity),
                                    time_step_s);
    DT_DEBUG_LOG("Disc position post update: (%f, %f, %f)\n",
                   disc->position.x,
                   disc->position.y,
                   disc->position.z);
  }
}

/*
 * calculate_positions
 *
 * Uses the physical properties (acceleration, velocity etc) to calculate a new
 * location for each of the objects passed in.
 *
 * Parameters: match_state - Contains all the match information.
 *             time_step - The number of ms since the last update.
 */
void calculate_positions(MATCH_STATE *match_state,
                         Uint32 time_step)
{
  /*
   * Local Variables.
   */
  float time_step_s = time_step / MILLISECONDS_PER_SECOND;
  int ii;

  /*
   * Only move the disc if it is in flight.
   */
  if (match_state->disc->disc_state == disc_in_air)
  {
    calculate_disc_position(match_state->disc, time_step_s, true);

    /*
     * Check if the disc is in contact with the ground and if so then register
     * it as having stopped.
     */
    if (match_state->disc->position.z <= 0)
    {
      disc_lands(match_state);
    }
  }

  /*
   * The players have only moved if the velocity vector is non-zero. If this is
   * the case then we want to update their NEW position vector not the current
   * one.
   *
   * The collision handler deals with checking the positions are valid and
   * setting the current position vector.
   */
  for (ii = 0; ii < match_state->players_per_team; ii++)
  {
    /*
     * Update the new player position on team a.
     */
    update_vector_from_differential(
                            &(match_state->teams[0]->players[ii]->new_position),
                            &(match_state->teams[0]->players[ii]->velocity),
                            time_step_s);

    /*
     * Do the same thing for the corresponding player on the other team.
     */
    update_vector_from_differential(
                            &(match_state->teams[1]->players[ii]->new_position),
                            &(match_state->teams[1]->players[ii]->velocity),
                            time_step_s);
  }
}
