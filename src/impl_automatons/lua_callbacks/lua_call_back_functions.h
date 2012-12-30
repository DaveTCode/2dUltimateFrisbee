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

#ifndef LUA_CALL_BACK_FUNCTIONS_H_
#define LUA_CALL_BACK_FUNCTIONS_H_

#include "lua5.1/lua.h"

struct match_state;

void set_up_lua_callback_globals(struct match_state *);
void destroy_lua_callback_globals();
void set_lua_player(int, int);
void register_lua_callback_functions(lua_State *);
int lua_callback_get_pitch_dimensions(lua_State *);
int lua_callback_set_player_desired_pos(lua_State *);
int lua_callback_set_player_speed(lua_State *);
int lua_callback_get_disc_final_pos(lua_State *);
int lua_callback_get_disc_pos(lua_State *);
int lua_callback_get_team_positions(lua_State *);
int lua_callback_get_team_desired_positions(lua_State *);
int lua_callback_calculate_all_intercept_times(lua_State *);
int lua_callback_get_all_team_marks(lua_State *);
int lua_callback_set_desired_mark(lua_State *);
int lua_callback_get_current_position(lua_State *);
int lua_callback_stop_running(lua_State *);
int lua_callback_is_attacking_left_to_right(lua_State *);

#endif /* LUA_CALL_BACK_FUNCTIONS_H_ */
