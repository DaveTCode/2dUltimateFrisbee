/*
 * lua_callback_globals.h
 *
 * Lua callback functions cannot be passed pointers (safely) so we have a bunch
 * of global states that are actually only global within the call back function
 * c file.
 *
 * This header file should only be included in files that contain lua call back
 * functions.
 *
 *  Created on: 12 Mar 2010
 *      Author: David
 */

#ifndef LUA_CALLBACK_GLOBALS_H_
#define LUA_CALLBACK_GLOBALS_H_

#include "../../match_state.h"

/*
 * Used to tell the number of automatons currently using the global variables.
 * If this goes down to 0 then we can free the memory used in these automatons.
 */
extern int g_automaton_references;

/*
 * Whenever we call into the lua functions we set the current player and team
 * index here. This allows the lua functions to ONLY affect the current player
 * when making callbacks.
 */
extern int g_curr_player;
extern int g_curr_team;

/*
 *
 */
extern MATCH_STATE *g_match_state;

#endif /* LUA_CALLBACK_GLOBALS_H_ */
