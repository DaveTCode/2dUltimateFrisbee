/*
 * automaton_event.c
 *
 *  Created on: 05 Mar 2010
 *      Author: David Tyler
 */
#include "../../dt_logger.h"

#include "automaton_event.h"

/*
 * create_automaton_event
 *
 * Allocates the memory required for a automaton_event
 *
 * Returns: A pointer to the newly created memory
 */
AUTOMATON_EVENT *create_automaton_event()
{
  /*
   * Local Variables
   */
  AUTOMATON_EVENT *automaton_event;

  /*
   * Allocate the required memory
   */
  automaton_event = (AUTOMATON_EVENT *) DT_MALLOC(sizeof(AUTOMATON_EVENT));

  return(automaton_event);
}

/*
 * destroy_automaton_event
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: automaton_event - The object to be freed.
 */
void destroy_automaton_event(AUTOMATON_EVENT *automaton_event)
{
  /*
   * Free the object.
   */
  DT_FREE(automaton_event);
}
