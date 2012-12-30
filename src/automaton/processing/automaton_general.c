/*
 * automaton_general.c
 *
 *  Created on: 6 Mar 2010
 *      Author: David
 */
#include "../../dt_logger.h"

#include "automaton_general.h"
#include "../data_structures/automaton.h"
#include "../data_structures/automaton_event.h"
#include "../data_structures/automaton_state.h"
#include "../data_structures/automaton_transition.h"
#include "../../player.h"

/*
 * get_state_from_transition
 *
 * Takes a transition and works out what state it should move to. This is a
 * recursive function and could easily get itself into an infinite loop if the
 * person writing the automaton is an idiot so we add in some extra error
 * checking code.
 *
 * The idea is that we are happy to bail on the program rather than overflow
 * the stack.
 *
 * Parameters: automaton - Used to give us access to the lua state.
 *             transition - The current transition.
 *             call_depth - The number of recursions. Forced to be lower than
 *                          some fixed number.
 *             player - The player to whom this event happened.
 * 
 * Returns: The state to move to OR null if the function failed.
 */
AUTOMATON_STATE *get_state_from_transition(AUTOMATON *automaton,
                                           AUTOMATON_TRANSITION *transition,
                                           int *call_depth,
                                           PLAYER *player)
{
  /*
   * Local Variables.
   */
  AUTOMATON_STATE *new_state = NULL;
  AUTOMATON *new_automaton = automaton;
  int rc;

  DT_AI_LOG("(%i:%i) Finding next state/transition/automaton from transition %s\n",
            player->team_id,
            player->player_id,
            transition->name);
  
  /*
   * To avoid splatting the stack by the user defining an automaton with loops
   * we bail out gracefully if the call depth is greater than MAX_CALL_DEPTH.
   */
  if (MAX_CALL_DEPTH <= *call_depth)
  {
    DT_AI_LOG("(%i:%i) Failed to find next state from transition due to exceeding " \
              "maximum call depth: Suspected loop in automaton\n",
              player->team_id,
              player->player_id);
    return(NULL);
  }

  /*
   * NOTE: If we hit an exception of any sort whilst processing the lua
   * function or attempting to move to the next state then we simply log it
   * and remain at the current state.
   */
  lua_getglobal(automaton->lua_state, transition->lua_function_name);

  /*
   * Put any parameters the lua function expects onto the stack.
   */
  lua_pushinteger(automaton->lua_state, player->team_id);
  lua_pushinteger(automaton->lua_state, player->player_id);

  /*
   * Call the lua function with the hard coded number of arguments as pushed
   * onto the stack above.
   */
  rc = lua_pcall(automaton->lua_state, 2, 1, 0);
  if (0 != rc)
  {
    DT_AI_LOG("(%i:%i) Lua function (%s) failed with message: %s\n",
              player->team_id,
              player->player_id,
              transition->lua_function_name,
              lua_tostring(automaton->lua_state, -1));
    new_state = NULL;
  }
  else
  {
    /*
     * The return value is either 0 (false) or 1 (true).
     */
    if (lua_isnumber(automaton->lua_state, -1))
    {
      rc = (int) lua_tointeger(automaton->lua_state, -1);
      lua_pop(automaton->lua_state, 1);
      DT_AI_LOG("(%i:%i) Lua function %s returned %i\n", 
                player->team_id,
                player->player_id,
                transition->lua_function_name,
                rc);
    }
    else
    {
      DT_AI_LOG("(%i:%i) Lua failure: Return value from %s was not a number.\n",
                player->team_id,
                player->player_id,
                transition->lua_function_name);
      new_state = NULL;
      goto EXIT_LABEL;
    }

    /*
     * If the return value is 1 then we move to the true state in the
     * transition. If 0 then to the false state instead.
     */
    if (0 == rc)
    {
      /*
       * If there is a new automaton to move into then do that first. This 
       * means that any selection of states is then done on the new automaton.
       */
      if (NULL != transition->false_automaton)
      {
        DT_AI_LOG("(%i:%i) Moving to new automaton (%s)\n",
                  player->team_id, 
                  player->player_id,
                  transition->false_automaton->name);
        player->automaton = transition->false_automaton;
        new_automaton = transition->false_automaton;
      }

      /*
       * States take precedence over transitions so if there is a state we
       * move to that. Otherwise we move to the transition.
       */   
      if (NULL == transition->false_state &&
          NULL == transition->false_transition)
      {
        /*
         * If there is no transition or state to move to but there is 
         * an automaton then we assume that the player is moving to 
         * the current state in the new automaton.
         */
        new_state = find_automaton_state_by_name(
                                                new_automaton,
                                                player->automaton_state->name);
      }
      else if (NULL == transition->false_state &&
               NULL != transition->false_transition)
      {
        /*
         * There is a transition to process for this transition. It may be on
         * a new automaton but doesn't require it.
         */
        DT_AI_LOG("(%i:%i) Moving from transition %s to false transition %s\n", 
                  player->team_id, 
                  player->player_id,
                  transition->name,
                  transition->false_transition->name);
        transition = transition->false_transition;
        (*call_depth)++;
        new_state = get_state_from_transition(new_automaton, 
                                              transition, 
                                              call_depth,
                                              player);
      }
      else if (NULL != transition->false_state)
      {
        /*
         * There is a state to move to. This can include a new automaton
         * but doesn't require it.
         */
        new_state = transition->false_state;
        DT_AI_LOG("(%i:%i) Moving to false state %s\n", 
                  player->team_id, 
                  player->player_id,
                  new_state->name);
      }
    }
    else
    {
      /*
       * If there is a new automaton to move into then do that first. This 
       * means that any selection of states is then done on the new automaton.
       */
      if (NULL != transition->true_automaton)
      {
        DT_AI_LOG("(%i:%i) Moving to new automaton (%s)\n",
                  player->team_id, 
                  player->player_id,
                  transition->true_automaton->name);
        player->automaton = transition->true_automaton;
        new_automaton = transition->true_automaton;
      }

      /*
       * States take precedence over transitions so if there is a state we
       * move to that. Otherwise we move to the transition.
       */   
      if (NULL == transition->true_state &&
          NULL == transition->true_transition)
      {
        /*
         * If there is no transition or state to move to but there is 
         * an automaton then we assume that the player is moving to 
         * the current state in the new automaton.
         */
        new_state = find_automaton_state_by_name(
                                                new_automaton,
                                                player->automaton_state->name);
      }
      else if (NULL == transition->true_state &&
               NULL != transition->true_transition)
      {
        /*
         * There is a transition to process for this transition. It may be on
         * a new automaton but doesn't require it.
         */
        DT_AI_LOG("(%i:%i) Moving from transition %s to true transition %s\n", 
                  player->team_id, 
                  player->player_id,
                  transition->name,
                  transition->true_transition->name);
        transition = transition->true_transition;
        (*call_depth)++;
        new_state = get_state_from_transition(new_automaton, 
                                              transition, 
                                              call_depth,
                                              player);
      }
      else if (NULL != transition->true_state)
      {
        /*
         * There is a state to move to. This can include a new automaton
         * but doesn't require it.
         */
        new_state = transition->true_state;
        DT_AI_LOG("(%i:%i) Moving to true state %s\n", 
                  player->team_id, 
                  player->player_id,
                  new_state->name);
      }
    }
  }
  
EXIT_LABEL:

  (*call_depth)--;

  return(new_state);
}

/*
 * move_to_next_state
 *
 * Given a state and an event in the automaton we can move to the next state
 * based on the reported transition function by using this function.
 *
 * If there is no transition function then do not move anywhere.
 *
 * If one exists then call it. If it returns 0 then move to the false state
 * otherwise move to the true state.
 *
 * Any errors cause the object to remain at the current state.
 *
 * Parameters: event - The event that caused this function to be called.
 *             curr_state - The state that the object was in.
 *             automaton - We need access to the automaton to have access to the
 *                         lua state.
 *             player - Player to whom this event happened. Required to pass
 *                      parameters to the lua function.
 *
 * Returns: The new state.
 */
AUTOMATON_STATE *move_to_next_state(AUTOMATON_EVENT *event,
                                    AUTOMATON_STATE *curr_state,
                                    AUTOMATON *automaton,
                                    PLAYER *player)
{
  /*
   * Local Variables
   */
  AUTOMATON_STATE *new_state;
  AUTOMATON_TRANSITION *transition;
  int call_depth = 0;

  /*
   * Retrieve the transition from the array in the state. Recall that this is
   * indexed by the event id.
   */
  transition = curr_state->transitions[event->id];

  /*
   * If the transition is null it is because we are to remain at the current
   * state. This is done without calling any of the entrance or exit functions
   * for the current states.
   *
   * i.e. NOOP
   */
  if (NULL == transition)
  {
    new_state = curr_state;
  }
  else
  {
    /*
     * Call the exit function for the current state (we do this even if we are
     * staying at the current state).
     */
    if (NULL != curr_state->exit_function)
    {
      curr_state->exit_function(player);
    }

    /*
     * Retrieve the next state. This can be null on failure.
     */
    new_state = get_state_from_transition(automaton,
                                          transition,
                                          &call_depth,
                                          player);
    if (NULL == new_state)
    {
      DT_AI_LOG("(%i:%i) Staying at current state in automaton due to ill defined " \
                "automaton.\n",
                player->team_id,
                player->player_id);
      new_state = curr_state;
    }

    /*
     * Call the entrance function for the new state. This is done even if the
     * new state is the same as the previous state.
     */
    if (NULL != new_state->entrance_function)
    {
      new_state->entrance_function(player);
    }
  }

  return(new_state);
}
