/*
 * automaton_handler.c
 *
 *  Created on: 30 Apr 2010
 *      Author: David Tyler
 */

#include "dt_logger.h"

#include "automaton_handler.h"
#include "automaton/data_structures/automaton.h"
#include "automaton/data_structures/automaton_state.h"
#include "automaton/data_structures/automaton_event.h"
#include "automaton/data_structures/automaton_timed_event_queue.h"
#include "automaton/file_handling/automaton_transition_file_loader.h"
#include "match_state.h"

/*
 * create_automaton_set
 *
 * Creates the entire of the automaton structure including filling in the
 * starting automaton for the set.
 *
 * Parameters: xml_filename - The xml file describing the transition matrix.
 *             state_callback - Callback used to create the automaton states.
 *             event_callback - Callback used to create the automaton events.
 *             match_state - The match state object gives access to the lua
 *                           global state which is set up internally here.
 *
 * Returns: NULL on failure (error codes handled internally) or a pointer to
 *          the new memory.
 */
AUTOMATON_SET *create_automaton_set(
                                 char *xml_filename,
                                 int (*state_callback)(AUTOMATON_STATE ***,int),
                                 int (*event_callback)(AUTOMATON_EVENT ***),
                                 MATCH_STATE *match_state)
{
  /*
   * Local Variables.
   */
  AUTOMATON_SET *automaton_set;
  int rc;

  /*
   * Allocate the memory required for this object.
   */
  automaton_set = (AUTOMATON_SET *) DT_MALLOC(sizeof(AUTOMATON_SET));

  /*
   * Parse the file into the automaton set. If this fails for any reason then
   * log an appropriate error and return NULL. No point letting it continue.
   */
  rc = parse_automaton_set_xml_file(xml_filename,
                                    automaton_set,
                                    state_callback,
                                    event_callback,
                                    match_state);
  if (AUTOMATON_XML_FILE_OK != rc)
  {
    DT_DEBUG_LOG("Failed to parse xml automaton file %s into automaton set.\n",
                 xml_filename);
    destroy_automaton_set(automaton_set);
    automaton_set = NULL;
    goto EXIT_LABEL;
  }

EXIT_LABEL:

  return(automaton_set);
}

/*
 * destroy_automaton_set
 *
 * Free the memory used inside an automaton set.
 *
 * Parameters: automaton_set - The set to be destroyed.
 */
void destroy_automaton_set(AUTOMATON_SET *automaton_set)
{
  /*
   * Local Variables.
   */
  int ii;

  /*
   * Destroy the entire automaton structure. Note that we don't free the start
   * automaton because it will be freed as part of the loop anyway.
   */
  for (ii = 0; ii < automaton_set->num_automatons; ii++)
  {
    if (NULL != automaton_set->automaton_array[ii])
    {
      destroy_automaton(automaton_set->automaton_array[ii]);
    }
  }
  DT_FREE(automaton_set->automaton_array);

  /*
   * Free the object.
   */
  DT_FREE(automaton_set);
}

/*
 * create_automaton_handler
 *
 * Allocates the memory required for a automaton_handler
 *
 * Parameters: offensive_xml_file - File containing the offensive automatons.
 *             defensive_xml_file - File containing the defensive automatons.
 *             state_callback - Callback to generate automaton states.
 *             event_callback - Callback to generate automaton events.
 *             match_state - Used to get access to the lua global state.
 *
 * Returns: A pointer to the newly created memory
 */
AUTOMATON_HANDLER *create_automaton_handler(
                                 char *offensive_xml_file,
                                 char *defensive_xml_file,
                                 int (*state_callback)(AUTOMATON_STATE ***,int),
                                 int (*event_callback)(AUTOMATON_EVENT ***),
                                 MATCH_STATE *match_state)
{
  /*
   * Local Variables
   */
  AUTOMATON_HANDLER *automaton_handler;

  /*
   * Allocate the required memory
   */
  automaton_handler = (AUTOMATON_HANDLER *) DT_MALLOC(sizeof(AUTOMATON_HANDLER));

  /*
   * Create the timed event queue for this handler. It is fully owned by the
   * handler.
   */
  automaton_handler->timed_event_queue = create_automaton_timed_event_queue();

  /*
   * Create the two automaton sets that this handler controls.
   */
  automaton_handler->offensive_set = create_automaton_set(offensive_xml_file,
                                                          state_callback,
                                                          event_callback,
                                                          match_state);
  automaton_handler->defensive_set = create_automaton_set(defensive_xml_file,
                                                          state_callback,
                                                          event_callback,
                                                          match_state);
  if (NULL == automaton_handler->offensive_set ||
      NULL == automaton_handler->defensive_set)
  {
    destroy_automaton_handler(automaton_handler);
    automaton_handler = NULL;
    goto EXIT_LABEL;
  }

EXIT_LABEL:

  return(automaton_handler);
}

/*
 * destroy_automaton_handler
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: automaton_handler - The object to be freed.
 */
void destroy_automaton_handler(AUTOMATON_HANDLER *automaton_handler)
{
  /*
   * Free up the automaton sets controlled by this object. They can be NULL if
   * the creation of the object failed.
   */
  if (NULL != automaton_handler->offensive_set)
  {
    destroy_automaton_set(automaton_handler->offensive_set);
  }
  if (NULL != automaton_handler->defensive_set)
  {
    destroy_automaton_set(automaton_handler->defensive_set);
  }

  /*
   * Free up the event queue. It cannot be NULL if the object was created.
   */
  destroy_automaton_timed_event_queue(automaton_handler->timed_event_queue);

  /*
   * Free the object.
   */
  DT_FREE(automaton_handler);
}

/*
 * get_automaton_by_name
 *
 * Retrieve an automaton by name from it's set. This uses a naive string name
 * search on the automaton array which would be grossly inefficient if there
 * were many hundreds of automatons. There aren't so it isn't.
 *
 * Parameters: set - The automaton set.
 *             name - The name we are searching for.
 *
 * Returns: The automaton or NULL on failure.
 */
AUTOMATON *get_automaton_by_name(AUTOMATON_SET *set, char *name)
{
  /*
   * Local Variables.
   */
  int ii;
  AUTOMATON found_automaton = NULL;

  /*
   * Loop through the automaton array until we run out or find the automaton
   * that we're looking for.
   */
  for (ii = 0; ii < set->num_automatons; ii++)
  {
    if (0 == strncmp(set->automaton_array[ii]->name, name, strlen(name)))
    {
      found_automaton = set->automaton_array[ii];
      goto EXIT_LABEL;
    }
  }

EXIT_LABEL:

  return(found_automaton);
}
