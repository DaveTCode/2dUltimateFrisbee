/*
 * automaton_general.h
 *
 *  Created on: 10 Mar 2010
 *      Author: David
 */

#ifndef AUTOMATON_GENERAL_H_
#define AUTOMATON_GENERAL_H_

struct automaton_event;
struct automaton_state;
struct automaton;
struct player;

/*
 * The maximum number of successive calls to transitions that the automaton
 * allows. This must be set to something small in order that we don't splat the
 * stack with an ill defined automaton.
 */
#define MAX_CALL_DEPTH 5

struct automaton_state *move_to_next_state(struct automaton_event *,
                                           struct automaton_state *,
                                           struct automaton *,
                                           struct player *);

#endif /* AUTOMATON_GENERAL_H_ */
