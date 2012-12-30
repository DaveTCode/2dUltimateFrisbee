/*
 * automaton.h
 *
 *  Created on: 05 Mar 2010
 *      Author: David Tyler
 */

#ifndef AUTOMATON_H_
#define AUTOMATON_H_

#include "lua5.1/lua.h"
#include "ezxml/ezxml.h"

struct automaton_set;
struct automaton_state;
struct automaton_event;
struct automaton_transition;
struct match_state;

/*
 * The maximum length for the length of an automaton name.
 */
#define MAX_AUTOMATON_NAME_LEN 100

/*
 * INIT_AUTOMATON_LINK_RET_CODES
 *
 * TODO: Write comment block
 */
#define INIT_AUTOMATON_LINKS_OK 0
#define INIT_AUTOMATON_LINKS_TRANSITION_FILE_FAIL 1
#define INIT_AUTOMATON_LINKS_CSV_FILE_FAIL 2

/*
 * AUTOMATON
 *
 * Represents a single automaton with a set of states, events and transition
 * functions.
 *
 * lua_state - This is the lua state that we use to load the functions.
 * start_state - Must be one of the states and is the entrance point for this
 *               automaton.
 * states - An array of the states in the automaton. Indexed by state id.
 * events - An array of all the events in the automaton. Indexed by event id.
 * transitions - An array of all the transition functions used in the automaton
 *               indexed by id.
 * num_states - Stored to save calculating this from the size of the array.
 * num_events
 * num_transitions
 */
typedef struct automaton
{
  char name[MAX_AUTOMATON_NAME_LEN + 1];
  lua_State *lua_state;
  struct automaton_state *start_state;
  struct automaton_state **states;
  struct automaton_event **events;
  struct automaton_transition **transitions;
  int num_states;
  int num_events;
  int num_transitions;
} AUTOMATON;

int init_automaton_links(AUTOMATON *,
                         ezxml_t,
                         struct automaton_set *,
                         struct match_state *);
AUTOMATON *create_automaton(int (*)(struct automaton_state ***, int),
                            int (*)(struct automaton_event ***));
void destroy_automaton(AUTOMATON *);
struct automaton_state *find_automaton_state_by_name(AUTOMATON *, char *);
struct automaton_event *find_automaton_event_by_name(AUTOMATON *, char *);
struct automaton_transition *find_automaton_transition_by_name(AUTOMATON *,
                                                               char *);

#endif /* AUTOMATON_H_ */
