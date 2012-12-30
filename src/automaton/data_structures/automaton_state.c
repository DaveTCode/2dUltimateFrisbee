/*
 * automaton_state.c
 *
 *  Created on: 05 Mar 2010
 *      Author: David Tyler
 */
#include "../../dt_logger.h"

#include "automaton_state.h"
#include "automaton_transition.h"

/*
 * create_automaton_state
 *
 * Allocates the memory required for a automaton_state
 *
 * Returns: A pointer to the newly created memory
 */
AUTOMATON_STATE *create_automaton_state(int num_events)
{
  /*
   * Local Variables
   */
  AUTOMATON_STATE *automaton_state;

  /*
   * Allocate the required memory
   */
  automaton_state = (AUTOMATON_STATE *) DT_MALLOC(sizeof(AUTOMATON_STATE));

  /*
   * Allocate the array of transitions. There is one per event in the automaton.
   */
  automaton_state->transitions = (AUTOMATON_TRANSITION **)
                         DT_MALLOC(sizeof(AUTOMATON_TRANSITION *) * num_events);
  memset(automaton_state->transitions,
         '\0',
         sizeof(AUTOMATON_TRANSITION *) * num_events);

  /*
   * The entrance and exit functions start as null pointers as not all states
   * need use them.
   */
  automaton_state->entrance_function = NULL;
  automaton_state->exit_function = NULL;

  return(automaton_state);
}

/*
 * destroy_automaton_state
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: automaton_state - The object to be freed.
 */
void destroy_automaton_state(AUTOMATON_STATE *automaton_state)
{
  /*
   * Although we do not want to free the transition objects themselves yet we
   * do need to free the array if it has been created.
   */
  if (NULL != automaton_state->transitions)
  {
    DT_FREE(automaton_state->transitions);
  }

  /*
   * Free the object.
   */
  DT_FREE(automaton_state);
}
