/*
 * automaton_event.h
 *
 *  Created on: 05 Mar 2010
 *      Author: David Tyler
 */

#ifndef AUTOMATON_EVENT_H_
#define AUTOMATON_EVENT_H_

/*
 * Arbitrary choice of the maximum length for the name of an event.
 */
#define MAX_EVENT_NAME_LEN 200

/*
 * AUTOMATON_EVENT
 *
 * Represents a single event in the automaton.
 *
 * id - The unique id in an automaton for this event. Corresponds to the array
 *      index in the automaton event array.
 * name - The name used to reference the event in files.
 */
typedef struct automaton_event
{
  int id;
  char name[MAX_EVENT_NAME_LEN];
} AUTOMATON_EVENT;

AUTOMATON_EVENT *create_automaton_event();
void destroy_automaton_event(AUTOMATON_EVENT *);

#endif /* AUTOMATON_EVENT_H_ */
