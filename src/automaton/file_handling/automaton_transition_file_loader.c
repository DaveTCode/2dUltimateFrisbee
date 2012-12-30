/*
 * automaton_transition_file_loader.c
 *
 * Transition files are in xml format with each transition object being as
 * follows:
 *
 * <transition_file>
 *   <transition>
 *     <name></name>
 *     <true_state></true_state>
 *     <false_state></false_state>
 *     <lua_func_name></lua_func_name>
 *   </transition>
 * </transition_file>
 *
 * If any of the parameters are not included then the transition is invalid.
 *
 * If an data is found to be bad the WHOLE file is aborted.
 *
 *  Created on: 5 Mar 2010
 *      Author: David
 */

#include "../../dt_logger.h"

#include <ezxml/ezxml.h>
#include <ezxml/ezxml.c>
#include "../data_structures/automaton.h"
#include "../data_structures/automaton_state.h"
#include "../data_structures/automaton_event.h"
#include "../data_structures/automaton_transition.h"
#include "../../automaton_handler.h"
#include "automaton_transition_file_loader.h"
#include "../../match_state.h"

/*
 * fill_single_automaton_links
 *
 * Internal function to this file!
 *
 * Performs the second pass over the xml file. This pass goes back over
 * filling in the information on where each transition goes.
 *
 * We cannot do a single pass as the automatons can refer to each other inside
 * transitions and transitions can also refer to other transitions in other
 * automatons.
 *
 * Parameters: xml_file - Already opened in the calling function.
 *             automaton - The automaton must have had its transition table
 *                         created and populated.
 *             automaton_set - Used to refer to other automatons in the set by
 *                             name.
 *
 * Returns: One of TRANSITION_FILE_RET_CODES.
 */
int fill_single_automaton_links(ezxml_t xml_file, 
                                AUTOMATON *automaton, 
                                AUTOMATON_SET *automaton_set)
{
  /*
   * Local Variables.
   */
  int ret_code = PARSE_SINGLE_AUTOMATON_OK;
  ezxml_t xml_transition;
  char *transition_name;
  ezxml_t xml_true;
  ezxml_t xml_false;
  ezxml_t xml_lua_func_name;
  ezxml_t xml_true_state;
  ezxml_t xml_true_transition;
  ezxml_t xml_true_automaton;
  ezxml_t xml_false_state;
  ezxml_t xml_false_transition;
  ezxml_t xml_false_automaton;
  AUTOMATON_TRANSITION *transition;
  AUTOMATON *true_automaton = automaton;
  AUTOMATON *false_automaton = automaton;

  for (xml_transition = ezxml_child(xml_file, "transition");
       xml_transition;
       xml_transition = xml_transition->next)
  {
    /*
     * Retrieve the children of the transition object in the xml. These are
     * NULL if they are missing from the file.
     */
    transition_name = (char *) ezxml_attr(xml_transition, "name");
    xml_true = ezxml_child(xml_transition, "true");
    xml_false = ezxml_child(xml_transition, "false");
    xml_lua_func_name = ezxml_child(xml_transition, "lua_function_name");
    if (NULL == transition_name || NULL == xml_lua_func_name ||
        NULL == xml_true || NULL == xml_false)
    {
      DT_DEBUG_LOG("A transition in the file was missing a required field.\n");
      ret_code = PARSE_SINGLE_AUTOMATON_PARSE_FAIL;
      goto EXIT_LABEL;
    }

    /*
     * Attempt to find the transition object by name. This should never fail as
     * we have only just filled the transitions in by name in the first pass.
     */
    transition = find_automaton_transition_by_name(automaton, transition_name);
    if (NULL == transition)
    {
      DT_DEBUG_LOG("TF Pass 2: Could not find transition name: %s\n",
                   transition_name);
      ret_code = PARSE_SINGLE_AUTOMATON_PARSE_FAIL;
      goto EXIT_LABEL;
    }

    /*
     * The true/false objects can have any of a transition, an automaton or a
     * state sub object.
     */
    xml_true_state = ezxml_child(xml_true, "state");
    xml_true_transition = ezxml_child(xml_true, "transition");
    xml_true_automaton = ezxml_child(xml_true, "automaton");
    if (NULL == xml_true_state &&
        NULL == xml_true_transition &&
        NULL == xml_true_automaton)
    {
      DT_DEBUG_LOG("No automaton, state or transition sub object for %s\n",
                   transition->name);
      ret_code = PARSE_SINGLE_AUTOMATON_PARSE_FAIL;
      goto EXIT_LABEL;
    }
    
    /*
     * If there is an automaton associated with this then register that first.
     * If the automaton tag is set but the value is bad then log the error but
     * continue anyway.
     */
    if (NULL != xml_true_automaton)
    {
      transition->true_automaton = get_automaton_by_name(
                                                       automaton_set,
                                                       xml_true_automaton->txt);
      if (NULL == transition->true_automaton)
      {
        DT_DEBUG_LOG("True automaton (%s) not found for transition (%s)\n",
                     xml_true_automaton->txt,
                     transition->name);
      }
      else
      {
        true_automaton = transition->true_automaton;
      }

      /*
       * Set the transition and the state to NULL initially.
       */
      transition->true_transition = NULL;
      transition->true_state = NULL;
    }
    else
    {
      /*
       * There is no automaton to move to from this transition so set the 
       * automaton to NULL (for testing against).
       */
      transition->true_automaton = NULL;
    }

    if (NULL != xml_true_state)
    {
      /*
       * States take precedence over transitions so set the transition element
       * to NULL.
       */
      transition->true_transition = NULL;
      
      /*
       * If the state name cannot be found then we fail the parser.
       */
      transition->true_state = find_automaton_state_by_name(
                                                           true_automaton,
                                                           xml_true_state->txt);
      if (NULL == transition->true_state)
      {
        DT_DEBUG_LOG("Transition state %s not found in state table " \
                     "when parsing transition %s\n",
                     xml_true_state->txt, transition->name);
        ret_code = PARSE_SINGLE_AUTOMATON_PARSE_FAIL;
        goto EXIT_LABEL;
      }
    }
    else if (NULL != xml_true_transition)
    {
      /*
       * We have found a true transition name. Retrieve this AND set the true
       * state for the transition to be null so that the true transition takes
       * precedence.
       */
      transition->true_state = NULL;
      
      /*
       * If the transition name cannot be found in the automaton then the parser
       * fails.
       */
      transition->true_transition = find_automaton_transition_by_name(
                                                      true_automaton,
                                                      xml_true_transition->txt);
      if (NULL == transition->true_transition)
      {
        DT_DEBUG_LOG("Transition (%s) refers to invalid transition (%s)\n",
                     transition->name,
                     xml_true_transition->txt);
        ret_code = PARSE_SINGLE_AUTOMATON_PARSE_FAIL;
        goto EXIT_LABEL;
      }
    }

    /*
     * And again for false...
     */
    xml_false_state = ezxml_child(xml_false, "state");
    xml_false_transition = ezxml_child(xml_false, "transition");
    xml_false_automaton = ezxml_child(xml_false, "automaton");
    if (NULL == xml_false_state &&
        NULL == xml_false_transition &&
        NULL == xml_false_automaton)
    {
      DT_DEBUG_LOG("No state, automaton or automaton sub object for %s\n",
                   transition->name);
      ret_code = PARSE_SINGLE_AUTOMATON_PARSE_FAIL;
      goto EXIT_LABEL;
    }
    
    /*
     * If there is an automaton associated with this then register that first.
     * If the automaton tag is set but the value is bad then log the error but
     * continue anyway.
     */
    if (NULL != xml_false_automaton)
    {
      transition->false_automaton = get_automaton_by_name(
                                                      automaton_set,
                                                      xml_false_automaton->txt);
      if (NULL == transition->false_automaton)
      {
        DT_DEBUG_LOG("False automaton (%s) not found for transition (%s)\n",
                     xml_false_automaton->txt,
                     transition->name);
      }
      else
      {
        false_automaton = transition->false_automaton;
      }

      /*
       * Set the transition and the state to NULL initially.
       */
      transition->false_transition = NULL;
      transition->false_state = NULL;
    }
    else
    {
      /*
       * There is no automaton to move to from this transition so set the 
       * automaton to NULL (for testing against).
       */
      transition->false_automaton = NULL;
    }

    if (NULL != xml_false_state)
    {
      /*
       * We have found a false state. This takes precedence over the 
       * transition.
       */
      transition->false_transition = NULL;    
    
      /*
       * If the state name cannot be found then we fail the parser.
       */
      transition->false_state = find_automaton_state_by_name(
                                                         false_automaton,
                                                         xml_false_state->txt);
      if (NULL == transition->false_state)
      {
        DT_DEBUG_LOG("Transition state %s not found in state table " \
                     "when parsing transition %s\n",
                     xml_false_state->txt, transition->name);
        ret_code = PARSE_SINGLE_AUTOMATON_PARSE_FAIL;
        goto EXIT_LABEL;
      }
    }
    else if (NULL != xml_false_transition)
    {
      /*
       * We have found a false transition name. Retrieve this AND set the false
       * state for the transition to be null so that the false transition takes
       * precedence.
       */
      transition->false_state = NULL;
      transition->false_transition = find_automaton_transition_by_name(
                                                     false_automaton,
                                                     xml_false_transition->txt);
      if (NULL == transition->false_transition)
      {
        DT_DEBUG_LOG("Transition (%s) refers to invalid transition (%s)\n",
                     transition->name,
                     xml_false_transition->txt);
        ret_code = PARSE_SINGLE_AUTOMATON_PARSE_FAIL;
        goto EXIT_LABEL;
      }
    }

    /*
     * Copy the string used for the lua function name across by value.
     */
    strncpy(transition->lua_function_name,
            xml_lua_func_name->txt,
            sizeof(transition->lua_function_name));
  }

EXIT_LABEL:

  return(ret_code);
}

/*
 * parse_single_automaton
 *
 * A complex function that uses an external xml library to load the transition
 * file in and read out the seperate transition objects.
 *
 * These are loaded into the automaton.
 *
 * If there are any missing fields or the file itself is malformed then a parse
 * error is returned rather than trying to handle it gracefully.
 *
 * Parameters: automaton_xml - The section of the xml file that contains this
 *                             xml.
 *             automaton - Must have already had the state table filled in.
 *                         Is returned with the transition table completed.
 *             automaton_set - Used to refer to other automatons by name.
 *             csv_filename - Will contain the value of the xml csv_file tag.
 *             lua_filename - Will contain the value of the xml lua_file tag.
 *
 * Returns: One of PARSE_SINGLE_AUTOMATON_RET_CODES.
 */
int parse_single_automaton(ezxml_t automaton_xml,
                           AUTOMATON *automaton,
                           AUTOMATON_SET *automaton_set,
                           char *csv_filename,
                           char *lua_filename)
{
  /*
   * Local Variables.
   */
  int ret_code = PARSE_SINGLE_AUTOMATON_OK;
  ezxml_t xml_csv_filename;
  ezxml_t xml_lua_filename;
  ezxml_t xml_start_state;

  /*
   * Retrieve the filenames of the lua and csv files associated with this
   * automaton.
   */
  xml_csv_filename = ezxml_child(automaton_xml, "csv_file");
  xml_lua_filename = ezxml_child(automaton_xml, "lua_file");
  if (NULL == xml_csv_filename)
  {
    DT_DEBUG_LOG("The transition file does not contain a csv filename\n");
    ret_code = PARSE_SINGLE_AUTOMATON_LOAD_MISSING_CSV_FILENAME;
    goto EXIT_LABEL;
  }
  if (NULL == xml_lua_filename)
  {
    DT_DEBUG_LOG("The transition file does not contain a lua filename\n");
    ret_code = PARSE_SINGLE_AUTOMATON_LOAD_MISSING_LUA_FILENAME;
    goto EXIT_LABEL;
  }
  strncpy(csv_filename, xml_csv_filename->txt, MAX_INT_FILENAME_LEN);
  strncpy(lua_filename, xml_lua_filename->txt, MAX_INT_FILENAME_LEN);

  /*
   * TODO: Does this need to be more intelligent?
   * Get the basic start state for the automaton.
   */
  xml_start_state = ezxml_child(automaton_xml, "start_state");
  if (NULL == xml_start_state)
  {
    DT_DEBUG_LOG("The transition file does not have a start state\n");
    ret_code = PARSE_SINGLE_AUTOMATON_LOAD_MISSING_START_STATE;
    goto EXIT_LABEL;
  }

  /*
   * If we can't find the automaton state that is referred to then we don't
   * use a default we just fail.
   */
  automaton->start_state = find_automaton_state_by_name(automaton,
                                                        xml_start_state->txt);
  if (NULL == automaton->start_state)
  {
    DT_DEBUG_LOG("The transition file has an invalid start state (%s)\n",
                 xml_start_state->txt);
    ret_code = PARSE_SINGLE_AUTOMATON_LOAD_INVALID_START_STATE;
    goto EXIT_LABEL;
  }

  /*
   * Fill in links between automatons, states, and transitions.
   */
  ret_code = fill_single_automaton_links(automaton_xml, 
                                         automaton, 
                                         automaton_set);
  if (PARSE_SINGLE_AUTOMATON_OK != ret_code) goto EXIT_LABEL;

EXIT_LABEL:

  return(ret_code);
}

/*
 * parse_automaton_set_xml_file
 *
 * Parse the entire of an automaton set xml file.
 *
 * This is called once per xml file. It creates the automaton structure and 
 * fills it in joining up the links. Any error is considered fatal and will
 * cause a return code along with debug remarks.
 *
 * Parameters: filename - If this cannot be read then the function will fail.
 *             automaton_set - The set to fill in a structure for.
 *             state_callback - Callback to generate the states.
 *             event_callback - Callback to generate the events.
 *             match_state - Used to set up lua globals.
 */
int parse_automaton_set_xml_file(char *filename,
                                 AUTOMATON_SET *automaton_set,
                                 int (*state_callback)(AUTOMATON_STATE ***,int),
                                 int (*event_callback)(AUTOMATON_EVENT ***),
                                 MATCH_STATE *match_state)
{
  /*
   * Local Variables.
   */
  int ret_code = AUTOMATON_XML_FILE_OK;
  ezxml_t xml_file;
  ezxml_t xml_automaton;
  ezxml_t xml_start_automaton;
  int ii;
  int rc;

  /*
   * The transition file is strictly an xml file so we use an external xml
   * library to load it and look for values.
   *
   * If the file comes back null then it failed to load for some reason.
   */
  xml_file = ezxml_parse_file(filename);
  if (NULL == xml_file)
  {
    DT_DEBUG_LOG("Failed to load xml_automaton file %s: error %s\n", 
                 filename,
                 ezxml_error(xml_file));
    ret_code = AUTOMATON_XML_FILE_LOAD_FAIL;
    goto EXIT_LABEL;
  }

  /*
   * This allocates the memory required for the automaton array and creates 
   * the automaton structures. It does not fill in any information.
   */
  create_automatons_from_xml(&xml_file, 
                             automaton_set, 
                             state_callback, 
                             event_callback);

  /*
   * Fill in the names of the automatons and transitions from the file. This
   * is required so that we can refer to them from each other when building
   * up the links later.
   */
  ret_code = parse_automaton_names(&xml_file, automaton_set);
  if (AUTOMATON_XML_FILE_OK != ret_code)
  {
    DT_DEBUG_LOG("Failed to parse automaton names\n");
    goto EXIT_LABEL;
  }

  /*
   * Finally now that the automatons are created and have their names filled in
   * we can set up all the links between transitions, states, events and other
   * automatons.
   */
  ii = 0;
  for (xml_automaton = ezxml_child(xml_file, "automaton");
       xml_automaton;
       xml_automaton = xml_automaton->next)
  {
    /*
     * Attempt to set up the automaton links.
     */
    rc = init_automaton_links(automaton_set->automaton_array[ii],
                              xml_automaton,
                              automaton_set,
                              match_state);
    if (INIT_AUTOMATON_LINKS_OK != rc)
    {
      DT_DEBUG_LOG("Failed setting up automaton links for automaton %s\n",
                   automaton_set->automaton_array[ii]->name);
      ret_code = AUTOMATON_XML_FILE_LINKS_FAIL;
      goto EXIT_LABEL;
    }

    ii++;
  }

  /*
   * The automaton that this set starts in is hardcoded into the xml file so
   * retrieve it now and verify that it exists.
   */
  xml_start_automaton = ezxml_child(xml_file, "start_automaton");
  if (NULL == xml_start_automaton)
  {
    DT_DEBUG_LOG("There is no starting automaton in the set.\n");
    ret_code = AUTOMATON_XML_FILE_START_AUTOMATON_BAD;
    goto EXIT_LABEL;
  }

  /*
   * There was a starting automaton so retrieve it by name.
   */
  automaton_set->start_automaton = get_automaton_by_name(
                                                      automaton_set,
                                                      xml_start_automaton->txt);
  if (NULL == automaton_set->start_automaton)
  {
    DT_DEBUG_LOG("The starting automaton listed (%s) does not exist in the " \
                 "set.\n",
                 xml_start_automaton->txt);
    ret_code = AUTOMATON_XML_FILE_START_AUTOMATON_BAD;
    goto EXIT_LABEL;
  }

EXIT_LABEL:

  return(ret_code);
}

/*
 * create_automatons_from_xml
 *
 * Once this functions is complete all the structure will be in place in the 
 * automaton_set to fill in the names of the automatons and transitions.

 * It goes through the file twice. The first time counting the number of 
 * automatons and then allocating the memory required for that number.
 * The second time it creates each of the automatons and puts them into the
 * automaton set.
 *
 * They get entered into the array in the order that they appear in the file.
 *
 * Parameters: xml_file - A reference to the structure that contains the entire
 *                        xml.
 *             automaton_set - The collection of automatons to be filled in.
 *             state_callback - Callback function to generate the states for
 *                              each of the automatons.
 *             event_callback - Callback function to generate the events for
 *                              each of the automatons.
 */
void create_automatons_from_xml(ezxml_t *xml_file, 
                                AUTOMATON_SET *automaton_set,
                                int (*state_callback)(AUTOMATON_STATE ***,int),
                                int (*event_callback)(AUTOMATON_EVENT ***))
{
  /*
   * Local Variables.
   */
  int ii;
  int num_automatons;
  ezxml_t xml_automaton;

  /*
   * Count the number of automaton objects in this file. We need to know this
   * so that we allocate the memory for the automaton array.
   */
  num_automatons = 0;
  for (xml_automaton = ezxml_child(*xml_file, "automaton");
       xml_automaton;
       xml_automaton = xml_automaton->next)
  {
    num_automatons++;
  }
  automaton_set->num_automatons = num_automatons;

  /*
   * Attempt to allocate the memory for the automaton array.
   */
  automaton_set->automaton_array =
                (AUTOMATON **) DT_MALLOC(sizeof(AUTOMATON *) * num_automatons);

  /*
   * Create each of the automatons individually.
   */
  ii = 0;
  for (xml_automaton = ezxml_child(*xml_file, "automaton");
       xml_automaton;
       xml_automaton = xml_automaton->next)
  {
    /*
     * This allocated the memory and sets up the state, event tables.
     */
    automaton_set->automaton_array[ii] = create_automaton(state_callback,
                                                          event_callback);
    ii++;
  }
}

/*
 * parse_automaton_names
 *
 * Given a file with xml, fill in all the names that do not contain any 
 * external references. This is the the automaton names and the transition
 * names.
 *
 * Note that this also allocates all the memory required for the automatons
 * themselves including the transition table and creating the individual
 * transitions.
 *
 * Parameters: xml_file - The entire xml document.
 *             automaton_set - The automaton set to fill in.
 *
 * Returns: One of AUTOMATON_XML_FILE_RET_CODES.
 */
int parse_automaton_names(ezxml_t *xml_file, AUTOMATON_SET *automaton_set)
{
  /*
   * Local Variables.
   */
  int ret_code = AUTOMATON_XML_FILE_OK;
  ezxml_t xml_automaton;
  int ii;

  /*
   * Loop through all the automatons.
   */
  ii = 0;
  for (xml_automaton = ezxml_child(*xml_file, "automaton");
       xml_automaton;
       xml_automaton = xml_automaton->next)
  {
    /*
     * Set the name of the automaton. We refer to the automatons by name in the
     * transitions.
     */
    if (NULL == ezxml_attr(xml_automaton, "name"))
    {
      DT_DEBUG_LOG("Automaton is missing name attribute in xml file.\n");
      ret_code = AUTOMATON_XML_FILE_LINKS_FAIL;
      goto EXIT_LABEL;
    }
    strncpy(automaton_set->automaton_array[ii]->name,
            ezxml_attr(xml_automaton, "name"),
            MAX_AUTOMATON_NAME_LEN);

    /*
     * We have the automaton name so now retrieve the transition names for
     * the automaton. Note that this doesn't read into whether the transition
     * is valid yet or not.
     */
    ret_code = parse_transition_names(&xml_automaton, 
                                      automaton_set->automaton_array[ii]);
    if (AUTOMATON_XML_FILE_OK != ret_code)
    {
      goto EXIT_LABEL;
    }

    ii++;
  }

EXIT_LABEL:

  return ret_code;
}

/*
 * parse_transition_names
 *
 * Called once for each automaton in the xml. Creates the array of transitions
 * and fills it in with the transition names. Does not do any link creation.
 *
 * Parameters: xml_automaton - The xml to read.
 *             automaton - The automaton to fill in.
 *
 * Returns: One of AUTOMATON_XML_FILE_RET_CODES.
 */
int parse_transition_names(ezxml_t *xml_automaton, AUTOMATON *automaton)
{
  /*
   * Local Variables.
   */
  ezxml_t xml_transition;
  int num_transitions;
  int ii;
  int ret_code = AUTOMATON_XML_FILE_OK;

  /*
   * Count the number of transitions there are for this automaton.
   */
  num_transitions = 0;
  for (xml_transition = ezxml_child(*xml_automaton, "transition");
       xml_transition;
       xml_transition = xml_transition->next)
  {
    num_transitions++;
  }
  automaton->num_transitions = num_transitions;
  DT_DEBUG_LOG("Automaton (%s) has (%i) transitions\n",
               automaton->name,
               num_transitions);

  /*
   * Allocate the memory required for the transition array.
   */
  automaton->transitions = (AUTOMATON_TRANSITION **) 
                   DT_MALLOC(sizeof(AUTOMATON_TRANSITION *) * num_transitions);

  /*
   * Second pass through is to fill in the automaton names.
   */
  ii = 0;
  for (xml_transition = ezxml_child(*xml_automaton, "transition");
       xml_transition;
       xml_transition = xml_transition->next)
  {
    /*
     * Allocate the memory for this individual transition and set it's id.
     */
    automaton->transitions[ii] = create_automaton_transition();
    automaton->transitions[ii]->id = ii;

    /*
     * Set the name of the transition. We can use this to refer to other 
     * transitions.
     *
     * Note that we deliberately don't exist as soon as we find a bad
     * transition so that we have a chance to create all of them.
     * This provides a more useful collection of errors for the user.
     */
    if (NULL == ezxml_attr(xml_transition, "name"))
    {
      DT_DEBUG_LOG("Transition is missing name attribute in xml file.\n");
      ret_code = AUTOMATON_XML_FILE_LINKS_FAIL;
    }
    else
    {
      strncpy(automaton->transitions[ii]->name,
              ezxml_attr(xml_transition, "name"),
              sizeof(automaton->transitions[ii]->name));
      DT_DEBUG_LOG("Automaton (%s) has transition (%s)\n", 
                   automaton->name,
                   automaton->transitions[ii]->name)
    }

    ii++;
  }

EXIT_LABEL:

  return ret_code;
}