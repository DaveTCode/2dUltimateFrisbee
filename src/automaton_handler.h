/*
 * automaton_handler.h
 *
 *  Created on: 30 Apr 2010
 *      Author: David Tyler
 */
#ifndef AUTOMATON_HANDLER_H_
#define AUTOMATON_HANDLER_H_

struct automaton;
struct automaton_event;
struct automaton_state;
struct automaton_timed_event_queue;
struct match_state;

/*
 * AUTOMATON_SET
 *
 * TODO: Write description.
 *
 * automaton_array - Array containing all automatons from the xml file
 * num_automatons - Used to avoid calculating number based on the size of the
 *                  array.
 * start_automaton - One of the automaton_array. The starting point for
 *                   players when they switch to this set.
 */
typedef struct automaton_set
{
  struct automaton **automaton_array;
  int num_automatons;
  struct automaton *start_automaton;
} AUTOMATON_SET;

/*
 * AUTOMATON_HANDLER
 *
 * TODO: Write description.
 *
 * offensive_set - The set of automatons associated with offence.
 * defensive_set - The set of automatons associated with defence.
 */
typedef struct automaton_handler
{
  struct automaton_timed_event_queue *timed_event_queue;
  AUTOMATON_SET *offensive_set;
  AUTOMATON_SET *defensive_set;
} AUTOMATON_HANDLER;

struct automaton_set *create_automaton_set(
                                      char *,
                                      int(*)(struct automaton_state ***, int),
                                      int(*)(struct automaton_event ***),
                                      struct match_state *);
void destroy_automaton_set(AUTOMATON_SET *);
AUTOMATON_HANDLER *create_automaton_handler(
                                      char *,
                                      char *,
                                      int(*)(struct automaton_state ***, int),
                                      int(*)(struct automaton_event ***),
                                      struct match_state *);
void destroy_automaton_handler(AUTOMATON_HANDLER *);
struct automaton *get_automaton_by_name(AUTOMATON_SET *, char *);

#endif /* AUTOMATON_HANDLER_H_ */
