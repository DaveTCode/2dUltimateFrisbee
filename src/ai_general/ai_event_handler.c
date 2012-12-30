/*
 * ai_event_handler.c
 *
 *  Created on: 04-Feb-2010
 *      Author: DAT
 */

#include "../dt_logger.h"

#include "../player.h"
#include "../team.h"
#include "../data_structures/event_queue.h"
#include "../automaton/data_structures/automaton.h"
#include "../automaton/data_structures/automaton_event.h"

/*
 * throw_single_player_ai_event
 *
 * Throws an event to a single players ai processor. These get queued up and
 * everything on the queue get's processed once per frame update.
 *
 * Parameters: player - The player to throw an event to.
 *             event - The event to throw.
 */
void throw_single_player_ai_event(PLAYER *player, AUTOMATON_EVENT *event)
{
  DT_AI_LOG("(%i:%i) Event %s placed on queue\n",
            player->team_id,
            player->player_id,
            event->name);
  add_event_to_queue(event, player->event_queue);
}

/*
 * throw_single_player_ai_event_by_name
 *
 * Throws an event to a single players ai processor. These get queued up and
 * everything on the queue get's processed once per frame update.
 *
 * Noop if the event doesn't exist in the automaton.
 *
 * Parameters: player - The player to throw an event to.
 *             automaton - Used to find which event this refers to.
 *             event_name - The name of the event.
 */
void throw_single_player_ai_event_by_name(PLAYER *player,
                                          AUTOMATON *automaton,
                                          char *event_name)
{
  /*
   * Local Variables.
   */
  AUTOMATON_EVENT *event;

  /*
   * Attempt to find the event by name. If it doesn't exist then log the error
   * but otherwise fail silently.
   */
  event = find_automaton_event_by_name(automaton, event_name);
  if (NULL == event)
  {
    DT_DEBUG_LOG("Attempted to throw event that does not exist: %s",
                 event_name);
  }
  else
  {
    DT_AI_LOG("(%i:%i) Event %s placed on queue\n",
              player->team_id,
              player->player_id,
              event->name);
    add_event_to_queue(event, player->event_queue);
  }
}

/*
 * throw_multi_player_ai_event
 *
 * Throws an AI event to all the players in both teams. If the event does not
 * exist then it fails silently.
 *
 * Parameters: teams - Both teams with links to all players.
 *             players_per_team - Number of players in each team.
 *             automaton - Used to find the automaton event.
 *             event - The event to throw.
 */
void throw_multi_player_ai_event(TEAM **teams,
                                 int players_per_team,
                                 AUTOMATON_EVENT *event)
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
      DT_AI_LOG("(%i:%i) Event %s placed on queue\n",
                teams[jj]->players[ii]->team_id,
                teams[jj]->players[ii]->player_id,
                event->name);
      add_event_to_queue(event, teams[jj]->players[ii]->event_queue);
    }
  }
}

/*
 * throw_multi_player_ai_event
 *
 * Throws an AI event to all the players in both teams. If the event does not
 * exist then it fails silently.
 *
 * Parameters: teams - Both teams with links to all players.
 *             players_per_team - Number of players in each team.
 *             automaton - Used to find the automaton event.
 *             event - The event to throw.
 */
void throw_multi_player_ai_event_by_name(TEAM **teams,
                                         int players_per_team,
                                         AUTOMATON *automaton,
                                         char *event_name)
{
  /*
   * Local Variables.
   */
  int ii;
  int jj;
  AUTOMATON_EVENT *event;

  /*
   * Attempt to find the event by name. If it doesn't exist then log the error
   * but otherwise fail silently.
   */
  event = find_automaton_event_by_name(automaton, event_name);
  if (NULL == event)
  {
    DT_DEBUG_LOG("Attempted to throw event that does not exist: %s",
                 event_name);
  }
  else
  {
    for (ii = 0; ii < players_per_team; ii++)
    {
      for (jj = 0; jj < 2; jj++)
      {
        DT_AI_LOG("(%i:%i) Event %s placed on queue\n",
                  teams[jj]->players[ii]->team_id,
                  teams[jj]->players[ii]->player_id,
                  event->name);
        add_event_to_queue(event, teams[jj]->players[ii]->event_queue);
      }
    }
  }
}
