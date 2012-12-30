/*
 * automaton_transition.c
 *
 *  Created on: 05 Mar 2010
 *      Author: David Tyler
 */
#include "../../dt_logger.h"

#include "automaton_transition.h"

/*
 * create_automaton_transition
 *
 * Allocates the memory required for a automaton_transition
 *
 * Returns: A pointer to the newly created memory
 */
AUTOMATON_TRANSITION *create_automaton_transition()
{
  /*
   * Local Variables
   */
  AUTOMATON_TRANSITION *automaton_transition;

  /*
   * Allocate the required memory
   */
  automaton_transition = (AUTOMATON_TRANSITION *) DT_MALLOC(sizeof(AUTOMATON_TRANSITION));

  return(automaton_transition);
}

/*
 * destroy_automaton_transition
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: automaton_transition - The object to be freed.
 */
void destroy_automaton_transition(AUTOMATON_TRANSITION *automaton_transition)
{
  /*
   * Free the object.
   */
  DT_FREE(automaton_transition);
}
