/*
 * o_automaton_states.c
 *
 *  Created on: 7 Mar 2010
 *      Author: David
 */
#include "../../dt_logger.h"

#include "o_automaton_states.h"
#include "o_automaton_state_funcs.h"
#include "../../automaton/data_structures/automaton_state.h"
#include "../../automaton/data_structures/automaton_event.h"

/*
 * create_o_automaton_states
 *
 * Used as the callback function for the automaton to create an array of states
 * to go in the offence pull automaton.
 *
 * Parameters: state_array - Will be returned as an array of the states.
 *             num_events - The number of events in the automaton. Used to
 *                          create the transition array in the states.
 *
 * Returns: The number of states created.
 */
int create_o_automaton_states(AUTOMATON_STATE ***state_array, int num_events)
{
  /*
   * Local Variables.
   */
  int ii;

  /*
   * Allocate the memory for the state array dynamically. This is freed when we
   * destroy the automaton object.
   */
  (*state_array) = (AUTOMATON_STATE **) DT_MALLOC(sizeof(AUTOMATON_STATE *)
                                                 * NUM_O_AUTOMATON_STATES);

  /*
   * By looping over the number of states we can verify that all available
   * states were created correctly.
   */
  for (ii = 0; ii < NUM_O_AUTOMATON_STATES; ii++)
  {
    (*state_array)[ii] = create_automaton_state(num_events);

    switch(ii)
    {
      case 0:
        strncpy((*state_array)[ii]->name,
                O_AUTOMATON_STATE_WAITING,
                MAX_EVENT_NAME_LEN);
        (*state_array)[ii]->name[strlen(O_AUTOMATON_STATE_WAITING)] = '\0';
        (*state_array)[ii]->state_function = state_waiting_function;
        (*state_array)[ii]->entrance_function = state_waiting_entrance_function;
        break;
      case 1:
        strncpy((*state_array)[ii]->name,
                O_AUTOMATON_STATE_CATCHING_PULL,
                MAX_EVENT_NAME_LEN);
        (*state_array)[ii]->name[strlen(O_AUTOMATON_STATE_CATCHING_PULL)] = '\0';
        (*state_array)[ii]->state_function = state_catching_pull_function;
        break;
      case 2:
        strncpy((*state_array)[ii]->name,
                O_AUTOMATON_STATE_PICKING_UP_DISC,
                MAX_EVENT_NAME_LEN);
        (*state_array)[ii]->name[strlen(O_AUTOMATON_STATE_PICKING_UP_DISC)] = '\0';
        (*state_array)[ii]->state_function = state_picking_up_disc_function;
        break;
      case 3:
        strncpy((*state_array)[ii]->name,
                O_AUTOMATON_STATE_MOVE_DISC_TO_BRICK,
                MAX_EVENT_NAME_LEN);
        (*state_array)[ii]->name[strlen(O_AUTOMATON_STATE_MOVE_DISC_TO_BRICK)] = '\0';
        (*state_array)[ii]->state_function = state_move_disc_to_brick_function;
        break;
      case 4:
        strncpy((*state_array)[ii]->name,
                O_AUTOMATON_STATE_MOVE_DISC_TO_SIDELINE,
                MAX_EVENT_NAME_LEN);
        (*state_array)[ii]->name[strlen(O_AUTOMATON_STATE_MOVE_DISC_TO_SIDELINE)] = '\0';
        (*state_array)[ii]->state_function = state_move_disc_to_sideline_function;
        break;
      case 5:
        strncpy((*state_array)[ii]->name,
                O_AUTOMATON_STATE_RUNNING,
                MAX_EVENT_NAME_LEN);
        (*state_array)[ii]->name[strlen(O_AUTOMATON_STATE_RUNNING)] = '\0';
        (*state_array)[ii]->state_function = state_running_function;
        (*state_array)[ii]->entrance_function = state_running_entrance_function;
        (*state_array)[ii]->exit_function = state_running_exit_function;
        break;
      case 6:
        strncpy((*state_array)[ii]->name,
                O_AUTOMATON_STATE_INTERCEPT_DISC,
                MAX_EVENT_NAME_LEN);
        (*state_array)[ii]->name[strlen(O_AUTOMATON_STATE_INTERCEPT_DISC)] = '\0';
        (*state_array)[ii]->state_function = state_intercept_disc_function;
        (*state_array)[ii]->exit_function = state_intercept_disc_exit_function;
        break;
      case 7:
        strncpy((*state_array)[ii]->name,
                O_AUTOMATON_STATE_FOLLOW_PLAYER,
                MAX_EVENT_NAME_LEN);
        (*state_array)[ii]->state_function = state_follow_player_function;
        (*state_array)[ii]->entrance_function = state_follow_player_start_function;
        (*state_array)[ii]->exit_function = state_follow_player_exit_function;
        break;
      default:
        /*
         * Hitting this branch is a coding error so we should catch it in
         * testing and eliminate them there.
         */
        DT_DEBUG_LOG("State (%i) missing in o pull automaton creation.\n", ii);
        DT_ASSERT(false);
        break;
    }

    /*
     * Note that the id is equal to the index in the array. This is useful for
     * locating states/events in the arrays.
     */
    (*state_array)[ii]->id = ii;
  }

  return(NUM_O_AUTOMATON_STATES);
}
