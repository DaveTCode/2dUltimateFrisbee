/*
 * game_state.c
 *
 *  Created on: 23 Oct 2009
 *      Author: David
 */
#include "dt_logger.h"

#include <stddef.h>

#include "animation/animation_handler.h"
#include "automaton_handler.h"
#include "automaton/data_structures/automaton.h"
#include "automaton/data_structures/automaton_state.h"
#include "automaton/data_structures/automaton_event.h"
#include "camera_handler.h"
#include "disc.h"
#include "input_handler.h"
#include "match_state.h"
#include "match_stats.h"
#include "pitch.h"
#include "player.h"
#include "team.h"
#include "throw.h"

/*
 * create_players
 *
 * INTERNAL: This function should only be called from the create_match_state
 * function and is used to create the players for use in a match.
 */
void create_players(TEAM **teams,
                    int players_per_team,
                    AUTOMATON_HANDLER *automaton_handler)
{
  /*
   * Local Variables.
   */
  AUTOMATON *team_0_start_automaton;
  AUTOMATON *team_1_start_automaton;
  AUTOMATON_STATE *team_0_start_state;
  AUTOMATON_STATE *team_1_start_state;
  int ii;

  /*
   * Based on which team is on offence set up the two teams starting automaton.
   * This also defines the starting state for the players inside the automaton.
   */
  if (teams[0]->is_offense)
  {
    team_0_start_automaton = automaton_handler->offensive_set->start_automaton;
    team_1_start_automaton = automaton_handler->defensive_set->start_automaton;
  }
  else
  {
    team_0_start_automaton = automaton_handler->defensive_set->start_automaton;
    team_1_start_automaton = automaton_handler->offensive_set->start_automaton;
  }
  team_0_start_state = team_0_start_automaton->start_state;
  team_1_start_state = team_1_start_automaton->start_state;

  for (ii = 0; ii < players_per_team; ii++)
  {
    /*
     * Create the player object with the player id and team id.
     */
    teams[0]->players[ii] = create_player(ii, 0);
    teams[1]->players[ii] = create_player(ii, 1);

    /*
     * TODO: Sort out proper width for players.
     */
    teams[0]->players[ii]->width_x = 2.0f;
    teams[1]->players[ii]->width_x = 2.0f;
    teams[0]->players[ii]->width_y = 2.0f;
    teams[1]->players[ii]->width_y = 2.0f;
    teams[0]->players[ii]->height = 2.0f;
    teams[1]->players[ii]->height = 2.0f;

    /*
     * The player has a reference to the starting automaton for their team. We
     * also fill in the initial automaton state here.
     */
    teams[0]->players[ii]->automaton = team_0_start_automaton;
    teams[1]->players[ii]->automaton = team_1_start_automaton;
    teams[0]->players[ii]->automaton_state = team_0_start_state;
    teams[1]->players[ii]->automaton_state = team_1_start_state;
  }
}

/*
 * create_match_state
 *
 * TODO: Fill this function block in once we tie down what this actually does.
 *
 * Returns: A pointer to the new object or NULL on failure.
 */
MATCH_STATE *create_match_state(unsigned int hard_cap,
                                unsigned int game_length_s,
                                char *disc_graphic_filename,
                                char *grass_tile_filename,
                                char *offensive_xml_file,
                                char *defensive_xml_file,
                                int (*state_callback)(AUTOMATON_STATE ***, int),
                                int (*event_callback)(AUTOMATON_EVENT ***))
{
  MATCH_STATE *state;

  /*
   * Allocate the memory for the new object.
   */
  state = (MATCH_STATE *) DT_MALLOC(sizeof(MATCH_STATE));

  /*
   * Set the throw to null to start, this will be created when the user starts
   * a throw.
   */
  state->throw = create_throw();

  /*
   * Create a disc object for this game.
   */
  state->disc = create_disc(disc_graphic_filename);

  /*
   * The disc path starts as null so that we know when the first one has
   * been created.
   */
  state->disc_path = NULL;

  /*
   * Initialise the pitch objects associated with the match.
   */
  state->pitch = create_pitch(grass_tile_filename);

  /*
   * Initialise the camera handler associated with the match.
   */
  state->camera_handler = create_camera_handler();

  /*
   * Initialise the input handlers for the mouse and the keyboard.
   */
  state->key_input_state = create_key_input_state();
  state->mouse_input_state = create_mouse_input_state();

  /*
   * Initialise the animation handler. This does not load any animations as this
   * is done by the load_animation_data function.
   */
  state->animation_handler = create_animation_handler();

  /*
   * Create the timer objects for use in the game.
   */
  state->match_stats = create_match_stats(game_length_s,
                                          hard_cap);

  /*
   * Create the automaton handler, passing in the location of the two xml files
   * means that this whole structure is constructed here. This will tell us
   * what the default states are for the two teams.
   */
  state->automaton_handler = create_automaton_handler(offensive_xml_file,
                                                      defensive_xml_file,
                                                      state_callback,
                                                      event_callback,
                                                      state);
  if (NULL == state->automaton_handler)
  {
    destroy_match_state(state);
    state = NULL;
    goto EXIT_LABEL;
  }

  /*
   * Create the teams.
   */
  state->teams[0] = create_team();
  state->teams[1] = create_team();
  state->players_per_team = PLAYERS_PER_TEAM;
  create_players(state->teams,
                 state->players_per_team,
                 state->automaton_handler);

EXIT_LABEL:

  return(state);
}

/*
 * destroy_match_state
 *
 * Free the memory allocated for a match state object.
 *
 * Parameters: state - The object to be freed.
 */
void destroy_match_state(MATCH_STATE *state)
{
  /*
   * The below objects were created in the create function and so are destroyed
   * in the destroy function. This gives us a single cleanup point for the
   * whole object.
   */
  destroy_match_stats(state->match_stats);
  destroy_camera_handler(state->camera_handler);
  destroy_disc(state->disc);
  destroy_pitch(state->pitch);
  destroy_throw(state->throw);
  destroy_key_input_state(state->key_input_state);
  destroy_mouse_input_state(state->mouse_input_state);
  destroy_animation_handler(state->animation_handler);

  /*
   * The automaton handler can fail to be created in which case we destroy the
   * match state while it is still NULL.
   */
  if (NULL != state->automaton_handler)
  {
    destroy_automaton_handler(state->automaton_handler);
  }

  /*
   * Free the match state object itself.
   */
  DT_FREE(state);
}
