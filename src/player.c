/*
 * player.c
 *
 *  Created on: 7 Nov 2009
 *      Author: David
 */

#include "dt_logger.h"


#include <stdbool.h>
#include <stddef.h>
#include "player.h"
#include "disc.h"
#include "animation/animation_handler.h"
#include "data_structures/vector.h"
#include "data_structures/event_queue.h"

/*
 * create_player
 *
 * Create a player object.
 *
 * Parameters: index - Must be unique only across players in the current team
 *             team - Either 0 or 1, corresponds to the index in the team array.
 *
 * Returns: A pointer to the new object.
 */
PLAYER *create_player(int id, int team)
{
  PLAYER *new_player;

  /*
   * Allocate the required memory.
   */
  new_player = (PLAYER *) DT_MALLOC(sizeof(PLAYER));

  /*
   * Set the players id and team id.
   */
  new_player->player_id = id;
  new_player->team_id = team;

  /*
   * Set the automaton to NULL to start with so that we can test against it and
   * find out if it has been set externally.
   */
  new_player->automaton = NULL;
  new_player->automaton_state = NULL;

  /*
   * Create the vectors used for the player.
   */
  vector_set_values(&(new_player->position), 0.0f, 0.0f, 0.0f);
  vector_set_values(&(new_player->new_position), 0.0f, 0.0f, 0.0f);
  vector_set_values(&(new_player->velocity), 0.0f, 0.0f, 0.0f);
  vector_set_values(&(new_player->desired_position), 0.0f, 0.0f, 0.0f);

  /*
   * Create an event queue for this player.
   */
  new_player->event_queue = create_event_queue();

  /*
   * The default animation for a player is standing still.
   */
  new_player->animation_choice = standing_animation;
  new_player->curr_frame = 0;
  new_player->direction = animate_away;

  /*
   * Set up the standard speed parameters for the player.
   */
  new_player->max_speed = 10.0f;
  new_player->current_speed_percent = 100.0f;

  /*
   * Default is for players to start off managed by the ai.
   */
  new_player->is_ai_managed = true;

  return(new_player);
}

/*
 * destroy_player
 *
 * Frees the memory associated with the player object. This also frees the
 * thread that was allocated for the player ai and reduces the reference count
 * on the entity graphic (thus destroying it if this was the last object that
 * referred to it).
 *
 * Parameters: player - The player object to be freed.
 *
 * Returns: Nothing.
 */
void destroy_player(PLAYER *player)
{
  /*
   * Free the player object itself.
   */
  DT_FREE(player);
}

/*
 * set_player_velocity
 *
 * This is the setter function for the player velocity.
 *
 * Parameters: player - The player whose velocity is being changed.
 *             new_velocity - The new velocity vector.
 */
void set_player_velocity(PLAYER *player, VECTOR3 *new_velocity)
{
  vector_set_values(&(player->velocity),
                    new_velocity->x,
                    new_velocity->y,
                    new_velocity->z);
}

/*
 * complete_player_coords_update
 *
 * At the end of the collision detection phase this routine is called for each
 * player. It completes the update phase for the players so that the next time
 * that they are drawn they are in the new position.
 *
 * Parameters: player - The player object to complete the move for.
 */
void complete_player_coords_update(PLAYER *player)
{
  vector_copy_values(&(player->position), &(player->new_position));
}

/*
 * revert_player_coords_update
 *
 * The player coordinates can be reverted rather than updated at the end of
 * the update phase. This function sets the new coordinates back to the
 * current ones in the case where the movement was reverted.
 *
 * Parameters: player - The player object to revert.
 */
void revert_player_coords_update(PLAYER *player)
{
  vector_copy_values(&(player->new_position), &(player->position));
}

/*
 * can_catch_disc
 *
 * This is a simple collision based test to determine whether the player could
 * catch the disc in this frame. T
 */
bool can_catch_disc(PLAYER *player, DISC *disc)
{
  bool result = false;

  return(result);
}
