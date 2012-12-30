/*
 * init_automaton_events.c
 *
 *  Created on: 26 May 2010
 *      Author: David
 */

#include "../../dt_logger.h"

#include <string.h>
#include "event_names.h"
#include "../../automaton/data_structures/automaton_event.h"

/*
 * create_automaton_events
 *
 * Used as the callback function for the automaton to create an array of events
 * to go in the automatons.
 *
 * Parameters: event_array - Will be returned with the events created filled in.
 *
 * Returns: The number of events created.
 */
int create_automaton_events(AUTOMATON_EVENT ***event_array)
{
  /*
   * Local Variables.
   */
  int ii;

  /*
   * Allocate the event array dynamically as this is freed in the destroy
   * function.
   */
  (*event_array) = (AUTOMATON_EVENT **) DT_MALLOC(sizeof(AUTOMATON_EVENT *) *
                                                  NUM_AUTOMATON_EVENTS);

  /*
   * By looping over the number of events we can verify that all available
   * events were created correctly.
   */
  for (ii = 0; ii < NUM_AUTOMATON_EVENTS; ii++)
  {
    (*event_array)[ii] = create_automaton_event();

    switch(ii)
    {
      case 0:
        strncpy((*event_array)[ii]->name,
                AUTOMATON_EVENT_PULL_THROWN,
                MAX_EVENT_NAME_LEN);
        (*event_array)[ii]->name[strlen(AUTOMATON_EVENT_PULL_THROWN)] = '\0';
        break;
      case 1:
        strncpy((*event_array)[ii]->name,
                AUTOMATON_EVENT_PULL_DROPPED,
                MAX_EVENT_NAME_LEN);
        (*event_array)[ii]->name[strlen(AUTOMATON_EVENT_PULL_DROPPED)] = '\0';
        break;
      case 2:
        strncpy((*event_array)[ii]->name,
                AUTOMATON_EVENT_PULL_CAUGHT,
                MAX_EVENT_NAME_LEN);
        (*event_array)[ii]->name[strlen(AUTOMATON_EVENT_PULL_CAUGHT)] = '\0';
        break;
      case 3:
        strncpy((*event_array)[ii]->name,
                AUTOMATON_EVENT_DISC_PICKED_UP,
                MAX_EVENT_NAME_LEN);
        (*event_array)[ii]->name[strlen(AUTOMATON_EVENT_DISC_PICKED_UP)] = '\0';
        break;
      case 4:
        strncpy((*event_array)[ii]->name,
                AUTOMATON_EVENT_ARRIVED_AT_LOCATION,
                MAX_EVENT_NAME_LEN);
        (*event_array)[ii]->name[strlen(AUTOMATON_EVENT_ARRIVED_AT_LOCATION)] = '\0';
        break;
      case 5:
        strncpy((*event_array)[ii]->name,
                AUTOMATON_EVENT_DISC_HIT_FLOOR,
                MAX_EVENT_NAME_LEN);
        (*event_array)[ii]->name[strlen(AUTOMATON_EVENT_DISC_HIT_FLOOR)] = '\0';
        break;
      case 6:
        strncpy((*event_array)[ii]->name,
                AUTOMATON_EVENT_DISC_RELEASED,
                MAX_EVENT_NAME_LEN);
        (*event_array)[ii]->name[strlen(AUTOMATON_EVENT_DISC_RELEASED)] = '\0';
        break;
      case 7:
        strncpy((*event_array)[ii]->name,
                AUTOMATON_EVENT_DISC_CAUGHT,
                MAX_EVENT_NAME_LEN);
        break;
      default:
        DT_DEBUG_LOG("Event (%i) missing in o pull automaton creation.\n", ii);
        break;
    }

    /*
     * Note that the id is equal to the index in the array. This is useful for
     * locating states/events in the arrays.
     */
    (*event_array)[ii]->id = ii;
  }

  return(NUM_AUTOMATON_EVENTS);
}
