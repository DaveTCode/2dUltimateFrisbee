/*
 * o_automaton_state_funcs.c
 *
 *  Created on: 7 Mar 2010
 *      Author: David
 */

#include "../../dt_logger.h"

#include "SDL/SDL_stdinc.h"

#include "../generic_o_d_files/event_names.h"
#include "../lua_callbacks/lua_callback_globals.h"
#include "../../animation/animation.h"
#include "../../ai_general/ai_event_handler.h"
#include "../../collisions/collision_handler.h"
#include "../../collisions/intercept.h"
#include "../../data_structures/vector.h"
#include "../../disc.h"
#include "../../disc_path.h"
#include "../../match_state.h"
#include "../../physics.h"
#include "../../player.h"
#include "../../team.h"

/*
 * state_waiting_entrance_function
 *
 * Called when moving into the waiting state. Only sets the waiting animation
 * at the moment.
 *
 * Parameters: player - The player moving to the new state.
 */
void state_waiting_entrance_function(PLAYER *player)
{
  DT_AI_LOG("(%i:%i) Entering state waiting.\n",
            player->team_id,
            player->player_id);

  /*
   * Set the animation to the standing animation.
   */
  if (standing_animation != player->animation_choice)
  {
    player->animation_choice = standing_animation;
  }
}

int state_waiting_function(PLAYER *player, Uint32 dt)
{
  return(0);
}

int state_catching_pull_function(PLAYER *player, Uint32 dt)
{
  return(0);
}

int state_picking_up_disc_function(PLAYER *player, Uint32 dt)
{
  return(0);
}

int state_move_disc_to_brick_function(PLAYER *player, Uint32 dt)
{
  return(0);
}

int state_move_disc_to_sideline_function(PLAYER *player, Uint32 dt)
{
  return(0);
}

/*
 * state_running_entrance_function
 *
 * The function that is called when the player moves into the running state.
 * Sets the animation for running and the direction in which it is run on.
 *
 * Parameters: player - All start/end functions require this parameter.
 */
void state_running_entrance_function(PLAYER *player)
{
  DT_AI_LOG("(%i:%i) Entering state running\n",
            player->team_id, 
            player->player_id);

  /*
   * The player has just started running so change the animation.
   */
  player->animation_choice = running_animation;
  player->curr_frame = 0;

  /*
   * Determine the direction in which to play the animation based on where the
   * player is going to run to compared to where they are.
   */
  player->direction = calculate_animation_direction(&(player->position),
                                                    &(player->desired_position));
}

/*
 * state_running_function
 *
 * Function that is called each update loop whilst the player is in the running
 * state. At the moment it is not dependent on time and simply forces the
 * velocity vector to be in the direction requested at a constant speed.
 *
 * Parameters: player - The player who is in the running state.
 *             dt - The number of milliseconds since the last update.
 */
int state_running_function(PLAYER *player, Uint32 dt)
{
  /*
   * Local Variables.
   */
  VECTOR3 player_velocity;

  /*
   * If the player is within a small delta of the desired location then 
   * throw the arrived at location event so that the ai can handle it.
   */
  if (dist_between_vectors_2d(&(player->desired_position), 
                              &(player->position)) <= DISTANCE_TO_INTERACT)
  {
    throw_single_player_ai_event_by_name(player, 
                                         player->automaton, 
                                         AUTOMATON_EVENT_ARRIVED_AT_LOCATION);
  }

  /*
   * Set the player velocity using the desired position from above.
   */
  player_velocity = calc_velocity_towards_position(
                                                 &(player->position),
                                                 &(player->desired_position),
                                                 player->max_speed,
                                                 player->current_speed_percent);
  set_player_velocity(player, &player_velocity);

  return(0);
}

/*
 * state_running_exit_function
 *
 * Called when a player is leaving the running state. Sets the player's
 * velocity to 0.
 *
 * Parameters: player - The player leaving the running state.
 */
void state_running_exit_function(PLAYER *player)
{
  DT_AI_LOG("(%i:%i) Exiting state running\n",
            player->team_id, 
            player->player_id);

  /*
   * When leaving the running state we set the velocity to 0.
   */
  player->velocity.x = 0.0f;
  player->velocity.y = 0.0f;
  player->velocity.z = 0.0f;

  /*
   * Set the desired run location to be the position of the player.
   */
  vector_copy_values(&(player->desired_position), &(player->position));
}

/*
 * state_intercept_disc
 *
 * State to put a player in when they want to intercept the disc without giving
 * any thought to the path that they take. Will force a player to run towards
 * the place where they will meet the disc earliest.
 *
 * Parameters: player - The player who will intercept the disc.
 *             dt - ms since last update.
 */
int state_intercept_disc_function(PLAYER *player, Uint32 dt)
{
  /*
   * Local Variables.
   */
  INTERCEPT intercept;
  VECTOR3 player_velocity;

  /*
   * If the disc is not in the air then the intercept point will be the 
   * current location of the disc.
   */
  if (disc_in_air != g_match_state->disc->disc_state)
  {
    vector_copy_values(&(player->desired_position), 
                       &(g_match_state->disc->position));
  }
  else
  {
    /*
     * Calculate the position at which the player will intercept the disc.
     *
     * If the player cannot intercept the disc and they are in this state then
     * just go to where the disc will stop.
     */
    if (calculate_player_disc_path_intercept(player,
                                             g_match_state->disc_path,
                                             &intercept))
    {
      vector_copy_values(&(player->desired_position), &(intercept.intercept));
    }
    else
    {
      vector_copy_values(&(player->desired_position),
                         &(g_match_state->disc_path->end_position->position));
    }
  }

  /*
   * Set the player velocity using the desired position from above.
   */
  player_velocity = calc_velocity_towards_position(
                                                 &(player->position),
                                                 &(player->desired_position),
                                                 player->max_speed,
                                                 player->current_speed_percent);
  set_player_velocity(player, &player_velocity);

  return 0;
}

/*
 * state_intercept_disc_exit_function
 *
 * Called whenever the player leaves the intercept disc state. Stops any
 * running that was happening and resets the desired position.
 *
 * Parameters: player - The player leaving the intercept disc state.
 */
void state_intercept_disc_exit_function(PLAYER *player)
{
  DT_AI_LOG("(%i:%i) Exiting state intercept disc\n",
            player->team_id, 
            player->player_id);

  /*
   * When leaving the running state we set the velocity to 0.
   */
  player->velocity.x = 0.0f;
  player->velocity.y = 0.0f;
  player->velocity.z = 0.0f;

  /*
   * Set the desired run location to be the position of the player.
   */
  vector_copy_values(&(player->desired_position), &(player->position));
}

/*
 * state_follow_player_function
 *
 * This is a state to put the player in when the marked player index has been
 * set and you want to make the player follow that player on the other team.
 *
 * Parameters: player - The player will have their velocity set by this.
 *             dt - Number of ms since last update.
 *
 * Returns: 0 always.
 */
int state_follow_player_function(PLAYER *player, Uint32 dt)
{
  /*
   * Local Variables.
   */
  VECTOR3 player_velocity;
  TEAM *other_team = g_match_state->teams[g_curr_team == 0 ? 1 : 0];
  int mark_index = player->marked_player_index;

  
  if (mark_index < 0 || 
      mark_index >= g_match_state->players_per_team)
  {
    /*
     * If the player does not have a valid mark then this state will not do 
     * anything.
     */
    vector_copy_values(&(player->desired_position), &(player->position));
  }
  else
  {
    /*
     * There is valid mark so set the desired position of this player to be 
     * where the other player currently is.
     *
     * TODO: More sensible positioning for marker.
     */
    vector_copy_values(&(player->desired_position),
                       &(other_team->players[mark_index]->position));
  }

  /*
   * Set the player velocity using the desired position from above.
   */
  player_velocity = calc_velocity_towards_position(
                                                 &(player->position),
                                                 &(player->desired_position),
                                                 player->max_speed,
                                                 player->current_speed_percent);
  set_player_velocity(player, &player_velocity);

  return 0;
}

/*
 * state_follow_player_start_function
 *
 * TODO: Fill in function definition
 */
void state_follow_player_start_function(PLAYER *player)
{
  DT_AI_LOG("(%i:%i) Entering state follow player. Player to follow set as %i",
            player->team_id,
            player->player_id,
            player->marked_player_index);
}

/*
 * state_follow_player_exit_function
 *
 * TODO: Fill in function definition
 */
void state_follow_player_exit_function(PLAYER *player)
{
  DT_AI_LOG("(%i:%i) Exiting state follow player.",
            player->team_id,
            player->player_id);
}

