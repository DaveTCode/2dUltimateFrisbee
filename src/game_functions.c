/*
 * game_functions.c
 *
 * Contains generic functions that in match functions like catching the disc
 * and switch between offense and defense.
 *
 *  Created on: 10 Apr 2010
 *      Author: David
 */

#include "dt_logger.h"

#include "ai_general/ai_event_handler.h"
#include "automaton_handler.h"
#include "disc.h"
#include "impl_automatons/generic_o_d_files/event_names.h"
#include "match_state.h"
#include "player.h"
#include "team.h"

/*
 * catch_disc
 *
 * This function is called whenever the disc is successfully caught. It sets
 * the player animation and sets up the game states such that the disc is in
 * the hands of the player who caught it.
 *
 * It also hands control of the player who caught it to the user and in doing
 * so resets the automaton.
 *
 * Parameters: player
 *             disc
 */
void catch_disc(PLAYER *player, DISC *disc)
{
  DT_DEBUG_LOG("Player (%i:%i) catching disc\n",
               player->team_id,
               player->player_id);
  /*
   * TODO: Set the player catching animation.
   */

  /*
   * Stop the player immediately.
   * TODO: Graceful deceleration.
   */
  player->velocity.x = 0.0f;
  player->velocity.y = 0.0f;

  /*
   * Set up the booleans that determine the disc state.
   */
  player->has_disc = true;
  player->is_ai_managed = false;
  disc->disc_state = disc_with_player;
  disc->holder = player;
}

/*
 * disc_lands
 *
 * Called whenever the disc lands on the floor.
 *
 * Parameters: match_state - Required to have access to all the match 
 *                           information.
 */
void disc_lands(MATCH_STATE *match_state)
{
  DT_DEBUG_LOG("Disc has hit the floor at (%f, %f)\n",
               match_state->disc->position.x,
               match_state->disc->position.y);

  /*
   * Set the disc state.
   */
  match_state->disc->disc_state = disc_on_ground;

  /*
   * Throw a multi player event to let everyone know that the disc has hit the
   * floor.
   */
  throw_multi_player_ai_event_by_name(
                match_state->teams,
                match_state->players_per_team,
                match_state->automaton_handler->offensive_set->start_automaton,
                AUTOMATON_EVENT_DISC_HIT_FLOOR);
}

/*
 * turnover
 *
 * This is the last thing called when a turnover occurs. It knows nothing about
 * what cause the turnover. It sets the game states to swap O/D and changes
 * the player automatons.
 *
 * Parameters: match_state - We pass the whole match state as various things
 *                           can be changed by this function.
 */
void turnover(MATCH_STATE *match_state)
{
  DT_DEBUG_LOG("Turnover\n");

  /*
   * Swap over which team is on offence and which is on defence.
   */
  match_state->teams[0]->is_offense = !(match_state->teams[0]->is_offense);
  match_state->teams[1]->is_offense = !(match_state->teams[1]->is_offense);

  /*
   * TODO: Move from O to D automatons.
   */
}
