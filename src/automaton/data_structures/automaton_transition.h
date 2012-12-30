/*
 * automaton_transition.h
 *
 *  Created on: 05 Mar 2010
 *      Author: David Tyler
 */

#ifndef AUTOMATON_TRANSITION_H_
#define AUTOMATON_TRANSITION_H_

struct automaton_state;
struct automaton;

/*
 * The maximum length that the name of a transition can be in the automaton.
 * Although this is an arbitrary number if a transition exceeds it we reject it
 * rather than attempting to handle gracefully.
 */
#define MAX_TRANSITION_NAME_LEN   200

/*
 * We load lua functions from files by name and this is the maximum allowed
 * length of the function names. Again this is arbitrary but rather important
 * that is not exceeded.
 */
#define MAX_LUA_FUNCTION_NAME_LEN 200

/*
 * AUTOMATON_TRANSITION
 *
 * id - The unique id of this transition. Corresponds to the array index in the
 *      automaton transition array.
 * name - The name used externally to describe this transition.
 * true_state - The automaton state that this transition moves to if the lua
 *              function returns true.
 * false_state - The automaton state that this transition moves to if the lua
 *               function returns false.
 * true_transition - If there is no true state then this must point to a
 *                   transition object. In that case on a true value being
 *                   returned from the lua function we call this transition.
 * false_transition - If there is no false state then this must point to a
 *                    transition object. In that case on a false value being
 *                    returned from the lua function we call this transition.
 * true_automaton - If this is set then this transition takes us to a new
 *                  automaton when the lua function returns true.
 * false_automaton - If this is set then this transition takes us to a new
 *                   automaton when the lua function returns false.
 * lua_function_name - We call lua functions by name so this is the exact name
 *                     of the function in the lua transitions file.
 */
typedef struct automaton_transition
{
  int id;
  char name[MAX_TRANSITION_NAME_LEN];
  struct automaton_state *true_state;
  struct automaton_state *false_state;
  struct automaton_transition *true_transition;
  struct automaton_transition *false_transition;
  struct automaton *true_automaton;
  struct automaton *false_automaton;
  char lua_function_name[MAX_LUA_FUNCTION_NAME_LEN];
} AUTOMATON_TRANSITION;

AUTOMATON_TRANSITION *create_automaton_transition();
void destroy_automaton_transition(AUTOMATON_TRANSITION *);

#endif /* AUTOMATON_TRANSITION_H_ */
