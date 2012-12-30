/*
 * automaton.c
 *
 *  Created on: 05 Mar 2010
 *      Author: David Tyler
 */

#include "../../dt_logger.h"

#include <string.h>
#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#include <lua5.1/lauxlib.h>
#include <ezxml/ezxml.h>

#include "automaton.h"
#include "automaton_event.h"
#include "automaton_state.h"
#include "automaton_transition.h"
#include "../file_handling/automaton_csv_file_loader.h"
#include "../file_handling/automaton_transition_file_loader.h"
#include "../../automaton_handler.h"
#include "../../impl_automatons/lua_callbacks/lua_call_back_functions.h"
#include "../../match_state.h"

/*
 * create_automaton
 *
 * Allocates the memory required for a automaton. It then fills in the state
 * and event arrays but does not fill in the transition table or use the csv
 * file at all as this is done by a separate function.
 *
 * We pass in callbacks rather than arrays so that all memory allocation
 * regarding this object is done internally to the create function, thus all
 * freeing of memory can be done in the destroy function.
 *
 * Parameters: state_callback - A callback function pointer to a function which
 *                              will create all the state objects and fill them
 *                              in.
 *             event_callback - A callback function pointer to a function which
 *                              will create all the event objects and fill them
 *                              in.
 *
 * Returns: A pointer to the newly created memory or NULL on failure.
 */
AUTOMATON *create_automaton(int (*state_callback)(AUTOMATON_STATE ***, int),
                            int (*event_callback)(AUTOMATON_EVENT ***))
{
  /*
   * Local Variables
   */
  AUTOMATON *automaton;

  /*
   * Allocate the required memory. Forcing everything to 0 means that we can
   * test against NULL when destroying the object even if it is only half
   * created.
   */
  automaton = (AUTOMATON *) DT_MALLOC(sizeof(AUTOMATON));
  memset(automaton, '\0', sizeof(AUTOMATON));

  /*
   * Make the callback to get the array of events used in this automaton.
   */
  automaton->num_events = event_callback(&(automaton->events));

  /*
   * Make the callback to get the array of states used in this automaton.
   */
  automaton->num_states = state_callback(&(automaton->states),
                                         automaton->num_events);

  return(automaton);
}

/*
 * destroy_automaton
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: automaton - The object to be freed.
 */
void destroy_automaton(AUTOMATON *automaton)
{
  /*
   * Local Variables.
   */
  int ii;

  /*
   * The states, events and transitions were allocated in the create function
   * so we free them in the destroy function.
   */
  if (NULL != automaton->states)
  {
    for (ii = 0; ii < automaton->num_states; ii++)
    {
      destroy_automaton_state(automaton->states[ii]);
    }

    DT_FREE(automaton->states);
  }

  if(NULL != automaton->events)
  {
    for (ii = 0; ii < automaton->num_events; ii++)
    {
      destroy_automaton_event(automaton->events[ii]);
    }
    DT_FREE(automaton->events);
  }

  if (NULL != automaton->transitions)
  {
    for (ii = 0; ii < automaton->num_transitions; ii++)
    {
      destroy_automaton_transition(automaton->transitions[ii]);
    }
    DT_FREE(automaton->transitions);
  }

  /*
   * Destroy the lua callback function globals.
   */
  destroy_lua_callback_globals();

  /*
   * Free up the lua state object.
   */
  if (NULL != automaton->lua_state)
  {
    lua_close(automaton->lua_state);
  }

  /*
   * Free the object.
   */
  DT_FREE(automaton);
}

/*
 * init_automaton_lua_state
 *
 * Private function to the automaton object. Sets up the lua state and loads the
 * passed in lua filename so that functions from it can be called.
 *
 * Parameters: automaton - Must be in the process of beign created.
 *             lua_filename - The location of the file containing the transition
 *                            function scripts.
 *             match_state - The match state is required here so that we can set
 *                           up the global variables required by the lua call
 *                           back functions.
 */
void init_automaton_lua_state(AUTOMATON *automaton,
                              char *lua_filename,
                              MATCH_STATE *match_state)
{
  /*
   * Local Variables
   */
  int rc;

  /*
   * Open the lua state and load up the base libraries.
   */
  automaton->lua_state = lua_open();
  luaL_openlibs(automaton->lua_state);

  /*
   * Load the passed in lua file into the state. We will use it later and close
   * the state down in the destruction of the object.
   */
  rc = luaL_dofile(automaton->lua_state, lua_filename);
  if (0 != rc)
  {
    DT_DEBUG_LOG("Error loading lua file %s: %s\n",
                 lua_filename,
                 lua_tostring(automaton->lua_state, -1));
  }

  /*
   * Set up the lua call backs. These are independent of automaton.
   */
  set_up_lua_callback_globals(match_state);

  /*
   * Registering the callback functions with lua means that we are able to call
   * them from the lua files.
   */
  register_lua_callback_functions(automaton->lua_state);
}

/*
 * init_automaton_links
 *
 * Once all the automatons in a set have been created this can be called to
 * put all the links between states, events and other automatons into place.
 *
 * Also sets up the lua state for this automaton.
 *
 * Parameters: automaton - The automaton to be processed.
 *             automaton_xml - The section of the xml file referring to this
 *                             automaton.
 *             automaton_set - The parent set of automatons. The rest of the
 *                             automatons should have been created so that they
 *                             can be referenced.
 *             match_state - Required for setting up lua globals.
 *
 * Returns: One of INIT_AUTOMATON_LINK_RET_CODE;
 */
int init_automaton_links(AUTOMATON *automaton,
                         ezxml_t automaton_xml,
                         AUTOMATON_SET *automaton_set,
                         MATCH_STATE *match_state)
{
  int rc;
  int ret_code = INIT_AUTOMATON_LINKS_OK;
  char lua_filename[MAX_TRANSITION_FILE_LINE_LEN];
  char csv_filename[MAX_TRANSITION_FILE_LINE_LEN];

  /*
   * Fill in the transitions from the file that the user requested. This also
   * retrieves the csv and lua filenames for this automaton as they are stored
   * in the xml file with the transitions.
   */
  rc = parse_single_automaton(automaton_xml,
                              automaton,
                              automaton_set,
                              (char *)csv_filename,
                              (char *)lua_filename);
  if (PARSE_SINGLE_AUTOMATON_OK != rc)
  {
    DT_DEBUG_LOG("Could not load data from transition file.\n");
    ret_code = INIT_AUTOMATON_LINKS_TRANSITION_FILE_FAIL;
    goto EXIT_LABEL;
  }

  /*
   * Fill in the links from states to transitions using the csv file that was
   * referred to in the transition file.
   */
  rc = load_automaton_csv_file(csv_filename, automaton);
  if (CSV_FILE_LOAD_OK != rc)
  {
    DT_DEBUG_LOG("Could not load data from csv file. %s\n", csv_filename);
    ret_code = INIT_AUTOMATON_LINKS_CSV_FILE_FAIL;
    goto EXIT_LABEL;
  }

  /*
   * TODO: Should this have error checking? Don't understand lua api :(
   *
   * Initialise the lua state that is associated with this automaton object.
   */
  init_automaton_lua_state(automaton, lua_filename, match_state);

EXIT_LABEL:

  return(ret_code);
}

/*
 * find_automaton_state_by_name
 *
 * Takes a string and finds the matching state in the automaton state table.
 *
 * Parameters: automaton
 *             state
 *
 * Returns: NULL if no state was found by that name.
 */
AUTOMATON_STATE *find_automaton_state_by_name(AUTOMATON *automaton,
                                              char *state)
{
  /*
   * Local Variables.
   */
  int ii;

  /*
   * Loop through the automaton states comparing the name with each in the
   * array.
   *
   * @@DAT - If this at any point gets moved so that the automaton loading
   * is speed dependent then we should change this so that we are searching a
   * binary tree instead.
   */
  for (ii = 0; ii < automaton->num_states; ii++)
  {
    if (0 == strcmp(automaton->states[ii]->name, state))
    {
      return(automaton->states[ii]);
    }
  }

  return(NULL);
}

/*
 * find_automaton_event_by_name
 *
 * Takes a string and finds the matching state in the automaton event table.
 *
 * Parameters: automaton - Automaton to find the event in.
 *             event - The name of the event.
 *
 * Returns: NULL if no event was found by that name.
 */
AUTOMATON_EVENT *find_automaton_event_by_name(AUTOMATON *automaton,
                                              char *event)
{
  /*
   * Local Variables.
   */
  int ii;

  /*
   * Loop through the automaton events comparing the name with each in the
   * array.
   *
   * @@DAT - If this at any point gets moved so that the automaton loading
   * is speed dependent then we should change this so that we are searching a
   * binary tree instead.
   */
  for (ii = 0; ii < automaton->num_events; ii++)
  {
    if (0 == strcmp(automaton->events[ii]->name, event))
    {
      return(automaton->events[ii]);
    }
  }

  return(NULL);
}

/*
 * find_automaton_transition_by_name
 *
 * Takes a string and finds the matching transition in the automaton state
 * table.
 *
 * Parameters: automaton
 *             transition
 *
 * Returns: NULL if no transition was found by that name.
 */
AUTOMATON_TRANSITION *find_automaton_transition_by_name(AUTOMATON *automaton,
                                                        char *transition)
{
  /*
   * Local Variables.
   */
  int ii;

  /*
   * Loop through the automaton transitions comparing the name with each in the
   * array.
   *
   * @@DAT - If this at any point gets moved so that the automaton loading
   * is speed dependent then we should change this so that we are searching a
   * binary tree instead.
   */
  for (ii = 0; ii < automaton->num_transitions; ii++)
  {
    if (0 == strcmp(automaton->transitions[ii]->name, transition))
    {
      return(automaton->transitions[ii]);
    }
  }

  return(NULL);
}
