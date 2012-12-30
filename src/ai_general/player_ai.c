/*
 * player_ai.c
 *
 * Contains functions relating to the ai loop. This is no longer a threaded
 * system and just calls each player update loop in turn.
 *
 *  Created on: 7 Nov 2009
 *      Author: David Tyler
 */
#include "../dt_logger.h"

#include "SDL/SDL.h"
#include "../automaton/data_structures/automaton_state.h"
#include "../automaton/data_structures/automaton_event.h"
#include "../automaton/processing/automaton_general.h"
#include "../data_structures/event_queue.h"
#include "../impl_automatons/lua_callbacks/lua_call_back_functions.h"
#include "../match_state.h"
#include "../player.h"
#include "../team.h"

/*
 * process_player_ai
 *
 * Single function called for each player whenever we want to run the ai
 * update. It moves the player around in the state machine and performs one
 * lot of state processing.
 *
 * Parameters: player - The player to update.
 *             dt - Timestep in ms since last call to this function.
 */
void process_player_ai(PLAYER *player, Uint32 dt)
{
  /*
   * Local Variables.
   */
  AUTOMATON_EVENT *remote_event;

  /*
   * Check the queue for any new events. Process all new events on the queue
   * even if that means that we don't have time to do anything with some of
   * them.
   */
  while (get_event_from_queue(player->event_queue, &remote_event))
  {
    player->automaton_state = move_to_next_state(remote_event,
                                                 player->automaton_state,
                                                 player->automaton,
                                                 player);
  }

  /*
   * Perform per state processing.
   *
   * It may also throw internal events on to the queue.
   */
  player->automaton_state->state_function(player, dt);
}

/*
 * process_all_player_ai
 *
 * Called once per update to update all the player ais. Check that the player
 * is currently being managed by the ai and then calls the update function for
 * that player.
 *
 * Parameters: teams - Contains links to all players.
 *             players_per_team - The size of the players array in each team.
 *             dt - The number of ms since last ai update.
 */
void process_all_player_ai(TEAM **teams, int players_per_team, Uint32 dt)
{
  /*
   * Local Variables.
   */
  int ii;
  int jj;

  for (ii = 0; ii < players_per_team; ii++)
  {
    for (jj = 0; jj < 2; jj++)
    {
      if (teams[jj]->players[ii]->is_ai_managed)
      {
        /*
         * Set the lua callback globals to indicate which player is being
         * processed.
         */
        set_lua_player(jj, ii);

        /*
         * Call the update function for this player (performs actual
         * processing).
         */
        process_player_ai(teams[jj]->players[ii], dt);
      }
    }
  }
}
