/*
 * automaton_state.h
 *
 *  Created on: 05 Mar 2010
 *      Author: David Tyler
 */

#ifndef AUTOMATON_STATE_H_
#define AUTOMATON_STATE_H_

#include "SDL/SDL_stdinc.h"

struct automaton_transition;
struct player;

/*
 * The maximum length of the name of a state in the automaton. Arbitrary number
 * that can be changed.
 */
#define MAX_STATE_NAME_LEN 200

/*
 * AUTOMATON_STATE
 *
 * Represents a single state in the automaton. Carries references to an array
 * of transition functions. There will be one element in that array for each
 * event in the automaton.
 *
 * id - Unique id for this state in the automaton. Corresponds to the array
 *      index in the automaton state array.
 * name - Name used to describe this state in the automaton.
 * transitions - The array of transition functions.
 * state_function - A function pointer to the function that gets called each
 *                  update for this state.
 * entrance_function - The function that is called when this state is first
 *                     transitioned into.
 * exit_function - The function that is called when this state is left.
 */
typedef struct automaton_state
{
  int id;
  char name[MAX_STATE_NAME_LEN];
  struct automaton_transition **transitions;
  int (*state_function)(struct player *, Uint32);
  void (*entrance_function)(struct player *);
  void (*exit_function)(struct player *);
} AUTOMATON_STATE;

AUTOMATON_STATE *create_automaton_state(int);
void destroy_automaton_state(AUTOMATON_STATE *);

#endif /* AUTOMATON_STATE_H_ */
