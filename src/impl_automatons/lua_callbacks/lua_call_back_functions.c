/*
 * lua_call_back_functions.c
 *
 * Contains all the call back functions that are used in any automaton as well
 * as functions to set up and close down the globals required by the functions.
 *
 *  Created on: 12 Mar 2010
 *      Author: David
 */
#include "../../dt_logger.h"

#include "lua5.1/lua.h"
#include "lua_callback_globals.h"

#include "../../collisions/intercept.h"
#include "../../data_structures/vector.h"
#include "../../disc.h"
#include "../../disc_path.h"
#include "../../match_state.h"
#include "../../player.h"
#include "../../pitch.h"
#include "../../team.h"

/*
 * These are global variables defined in the lua_callback_globals.h file.
 *
 * g_automaton_references - Incremented each time an automaton calls the set up
 *                          function for the globals. Decremented for each call
 *                          to the destroy function. When it hits 0 again we
 *                          can free the memory used for the arrays.
 * g_curr_player - Set to the player index of the player currently processing
 *                 their lua functions.
 * g_curr_team - Set to the team index of the player currently processing their
 *                 lua functions.
 * g_match_state - lua callbacks have access to everything underneath the
 *                 match state banner.
 */
int g_automaton_references = 0;
int g_curr_player = 0;
int g_curr_team = 0;
MATCH_STATE *g_match_state;

/*
 * set_up_lua_callback_globals
 *
 * Sets up the global variables required by the lua call back functions. Any
 * memory allocated here MUST be freed in the destroy_lua_callback_function.
 *
 * Parameters: match_state - The lua callback functions can access any data in
 *                           the match state heirachy.
 */
void set_up_lua_callback_globals(MATCH_STATE *match_state)
{
  if (0 == g_automaton_references)
  {
    g_match_state = match_state;
  }

  g_automaton_references++;
}

/*
 * destroy_lua_callback_globals
 *
 * This function is required as the set up function allocates memory for the
 * global player array.
 */
void destroy_lua_callback_globals()
{
  if (0 == g_automaton_references)
  {
    // Noop at the moment.
  }
  else
  {
    g_automaton_references--;
  }
}

/*
 * set_lua_player
 *
 * Called once per player ai update to indicate which player is going to be
 * making calls to the lua ai subsystem. This means that we know who the lua
 * callbacks will refer to.
 *
 * Parameters: team_index - These also double as array indices.
 *             player_index
 */
void set_lua_player(int team_index, int player_index)
{
  g_curr_team = team_index;
  g_curr_player = player_index;
}

/*
 * lua_callback_get_pitch_dimensions
 *
 * Retrieves the dimensions for the pitch as a table of values. This is
 * required because the pitch dimensions are not totally fixed from one game to
 * the next and the lua files do not have hard coded values.
 *
 * Parameters: none
 *
 * Returns: A table with keys: {width, length, endzone_depth} and floating
 *          point values.
 */
int lua_callback_get_pitch_dimensions(lua_State *lua_state)
{
  /*
   * Create a table to contain all the pitch dimension information.
   */
  lua_newtable(lua_state);

  lua_pushstring(lua_state, "length");
  lua_pushnumber(lua_state, g_match_state->pitch->length_m);
  lua_settable(lua_state, -3);

  lua_pushstring(lua_state, "width");
  lua_pushnumber(lua_state, g_match_state->pitch->width_m);
  lua_settable(lua_state, -3);

  lua_pushstring(lua_state, "endzone_depth");
  lua_pushnumber(lua_state, g_match_state->pitch->endzone_depth_m);
  lua_settable(lua_state, -3);

  return(1);
}

/*
 * lua_callback_set_player_desired_pos
 *
 * Sets the position that the lua function would like the player to run to. At
 * the moment we don't do any verification of this input. Can decide later
 * whether that is necessary at this level.
 *
 * Parameters Order: {top}y_pos, x_pos{bottom}
 *
 * Parameters: [[Note: Although these are not strictly parameters they are put
 *                     onto the stack by lua and are pulled off here so we
 *                     treat them as parameters anyway.]]
 *             x_pos - The x coordinate of the desired position.
 *             y_pos - The y coordinate of the desired position.
 *
 * Returns: The number of results (this is forced by the lua api).
 */
int lua_callback_set_player_desired_pos(lua_State *lua_state)
{
  /*
   * Local Variables.
   */
  float x_pos;
  float y_pos;
  PLAYER *player = g_match_state->teams[g_curr_team]->players[g_curr_player];

  /*
   * Lua stack verification to check that there are the right number of
   * parameters and that they are the right variable type.
   */
  if (lua_gettop(lua_state) != 2)
  {
    DT_AI_LOG("(%i:%i) callback_set_player_desired_pos called with wrong " \
              "number of parameters (%i)\n",
              player->team_id, player->player_id,
              lua_gettop(lua_state));
    return 1;
  }
  else if (!lua_isnumber(lua_state, -2))
  {
    DT_AI_LOG("(%i:%i) callback_set_player_desired_pos called with wrong " \
              "parameter type for x coordinate (%s)\n",
              player->team_id, player->player_id,
              lua_typename(lua_state,lua_type(lua_state, -2)));
    return 1;
  }
  else if (!lua_isnumber(lua_state, -1))
  {
    DT_AI_LOG("(%i:%i) callback_set_player_desired_pos called with wrong " \
              "parameter type for y coordinate (%s)\n",
              player->team_id, player->player_id,
              lua_typename(lua_state,lua_type(lua_state, -1)));
    return 1;
  }


  x_pos = (float) lua_tonumber(lua_state, -2);
  y_pos = (float) lua_tonumber(lua_state, -1);

  /*
   * Retrieve the current player object and set their desired position to the
   * incoming values.
   */
  player->desired_position.x = x_pos;
  player->desired_position.y = y_pos;

  DT_AI_LOG("(%i:%i) callback_set_player_desired_pos called with x=%f,y=%f\n",
            player->team_id,
            player->player_id,
            x_pos, y_pos);

  return(0);
}

/*
 * lua_callback_set_player_speed
 *
 * Callback function to set the relative speed of the player. Obviously we
 * can't let the AI scripts just come up with an arbitrary number so we get it
 * as a percentage and enforce it to be between 0 and 100 (by adjusting it if
 * it isn't).
 *
 * Parameters Order: {top}speed_percentage{bottom}
 *
 * Parameters: [[Note: Although these are not strictly parameters they are put
 *                     onto the stack by lua and are pulled off here so we
 *                     treat them as parameters anyway.]]
 *             speed_percentage - Percentage of top speed that player should
 *                                run at. [0-100]
 */
int lua_callback_set_player_speed(lua_State *lua_state)
{
  /*
   * Local Variables.
   */
  PLAYER *player = g_match_state->teams[g_curr_team]->players[g_curr_player];
  float speed_percentage;

  /*
   * Lua stack verification. Checks that we got the right number and type of
   * parameters.
   */
  if (lua_gettop(lua_state) != 1 || !lua_isnumber(lua_state, -1))
  {
    DT_AI_LOG("(%i:%i) Error: Attempt to set speed percentage to non integer: %s",
              g_curr_team,
              g_curr_player,
              lua_typename(lua_state, -1));
    return 0;
  }

  speed_percentage = (float) lua_tonumber(lua_state, -1);

  /*
   * Clamp the speed percentage between 0 and 100. Don't bother reporting errors
   * here (apart from in the log files).
   */
  if (speed_percentage < 0)
  {
    DT_AI_LOG("(%i:%i )Speed passed in from lua was < 0...adjusting.\n",
              player->team_id, player->player_id);
    speed_percentage = 0;
  }
  else if (speed_percentage > 100)
  {
    DT_AI_LOG("(%i:%i) Speed passed in from lua was > 100...adjusting.\n",
              player->team_id, player->player_id);
    speed_percentage = 100;
  }

  /*
   * Retrieve the current player from the team array and set their current
   * speed percentage.
   */
  player = g_match_state->teams[g_curr_team]->players[g_curr_player];
  player->current_speed_percent = speed_percentage;

  DT_AI_LOG("(%i:%i) callback_set_player_speed called with %f%%\n",
            player->team_id, player->player_id,
            speed_percentage);

  return 0;
}

/*
 * lua_callback_get_disc_final_pos
 *
 * Retrieves the final resting position of the disc if it is allowed to finish
 * it's flight.
 *
 * Parameters: None.
 *
 * Returns: A table with x,y as the keys and the values being floating point
 * representations of the final position of the disc.
 */
int lua_callback_get_disc_final_pos(lua_State *lua_state)
{
  /*
   * Local Variables.
   */
  VECTOR3 *final_disc_pos;
  PLAYER *player = g_match_state->teams[g_curr_team]->players[g_curr_player];

  /*
   * If the disc is not in the air then this function returns the current
   * location of the disc.
   */
  if (g_match_state->disc->disc_state != disc_in_air)
  {
    final_disc_pos = &(g_match_state->disc->position);
  }
  else
  {
    final_disc_pos = &(g_match_state->disc_path->end_position->position);
  }

  /*
   * Create a new table to contain the 2D vector result.
   */
  lua_newtable(lua_state);

  lua_pushstring(lua_state, "x");
  lua_pushnumber(lua_state, final_disc_pos->x);
  lua_settable(lua_state, -3);

  lua_pushstring(lua_state, "y");
  lua_pushnumber(lua_state, final_disc_pos->y);
  lua_settable(lua_state, -3);

  DT_AI_LOG("(%i:%i) callback_get_disc_final_pos called. Returned (%f, %f)\n",
            player->team_id, player->player_id,
            final_disc_pos->x, final_disc_pos->y);

  return 1;
}

/*
 * lua_callback_get_disc_pos
 *
 * Returns the current disc position vector as 3 separate floats. Used to work
 * out where players should move to relative to the disc position when the disc
 * is STATIONARY! When it is in flight it is probably better to get the disc
 * path in some form and calculate collisions.
 *
 * Returns: {top}z, y, x{bottom}
 */
int lua_callback_get_disc_pos(lua_State *lua_state)
{
  /*
   * Create a new table which will contain the results. Add the x,y,z components
   * of the disc position to the table using x,y,z as keys.
   */
  lua_newtable(lua_state);

  lua_pushstring(lua_state, "x");
  lua_pushnumber(lua_state, g_match_state->disc->position.x);
  lua_settable(lua_state, -3);

  lua_pushstring(lua_state, "y");
  lua_pushnumber(lua_state, g_match_state->disc->position.y);
  lua_settable(lua_state, -3);

  lua_pushstring(lua_state, "z");
  lua_pushnumber(lua_state, g_match_state->disc->position.z);
  lua_settable(lua_state, -3);

  return 1;
}

/*
 * lua_callback_get_team_positions
 *
 * Retrieves a table of the positions that each player in the current team are
 * currently in.
 *
 * Returns: One table object keyed on the player indexes. Each value in that
 *          table is another table which has keys being "x" and "y" and values
 *          being the players locations.
 */
int lua_callback_get_team_positions(lua_State *lua_state)
{
  /*
   * Local Variables.
   */
  int ii;
  VECTOR3 position;

  /*
   * Create a table to contain the teams desired positions. This also pushes
   * the table onto the lua stack.
   */
  lua_newtable(lua_state);

  /*
   * For each player in the current team add their current x,y coordinates to
   * the table
   */
  for (ii = 0; ii < g_match_state->players_per_team; ii++)
  {
     vector_copy_values(
                  &(position), 
                  &(g_match_state->teams[g_curr_team]->players[ii]->position));

    /*
     * Use the player index as the key into the main table. The values will be
     * individual tables containing the player positions keyed by the strings
     * 'x' and 'y'.
     */
    lua_pushinteger(lua_state, ii);

    /*
     * Create the table containing the player position.
     */
    lua_newtable(lua_state);
    lua_pushstring(lua_state, "x");
    lua_pushnumber(lua_state, position.x);
    lua_settable(lua_state, -3);
    lua_pushstring(lua_state, "y");
    lua_pushnumber(lua_state, position.y);
    lua_settable(lua_state, -3);

    /*
     * This sets the value under the player index to be the newly created table.
     */
    lua_settable(lua_state, -3);
  }

  return 1;
}

/*
 * lua_callback_get_team_desired_positions
 *
 * Retrieves a table of the positions that each player in the current team are
 * attempting to run towards. This can be used to decide where is still free.
 *
 * Returns: One table object keyed on the player indexes. Each value in that
 *          table is another table which has keys being "x" and "y" and values
 *          being the players desired locations.
 */
int lua_callback_get_team_desired_positions(lua_State *lua_state)
{
  /*
   * Local Variables.
   */
  int ii;
  VECTOR3 position;

  /*
   * Create a table to contain the teams desired positions. This also pushes
   * the table onto the lua stack.
   */
  lua_newtable(lua_state);

  /*
   * For each player in the current team add their current x,y coordinates to
   * the table
   */
  for (ii = 0; ii < g_match_state->players_per_team; ii++)
  {
    vector_copy_values(&(position),
          &(g_match_state->teams[g_curr_team]->players[ii]->desired_position));

    /*
     * Use the player index as the key into the main table. The values will be
     * individual tables containing the player positions keyed by the strings
     * 'x' and 'y'.
     */
    lua_pushinteger(lua_state, ii);

    /*
     * Create the table containing the player position.
     */
    lua_newtable(lua_state);
    lua_pushstring(lua_state, "x");
    lua_pushnumber(lua_state, position.x);
    lua_settable(lua_state, -3);
    lua_pushstring(lua_state, "y");
    lua_pushnumber(lua_state, position.y);
    lua_settable(lua_state, -3);

    /*
     * This sets the value under the player index to be the newly created table.
     */
    lua_settable(lua_state, -3);
  }

  return 1;
}

/*
 * lua_callback_calculate_all_intercept_times
 *
 * Called when the disc in the air. Calculates all the intercept times for all
 * players on the field. Notably this includes the player who threw the disc.
 *
 * Returns: One table keyed on player_index + num_players *[01] based on team.
 *          The rows in this table contain the player index, team index, time
 *          to intercept and the intercept coordinates. These are in meters and
 *          seconds.
 *
 *          Only players who will intercept the disc are in the table.
 */
int lua_callback_calculate_all_intercept_times(lua_State *lua_state)
{
  /*
   * Local Variables.
   */
  PLAYER *player;
  DISC_PATH *disc_path = g_match_state->disc_path;
  INTERCEPT intercept;
  int ii;
  int jj;

  /*
   * If the disc is not in the air then this function returns nothing.
   */
  if (disc_in_air != g_match_state->disc->disc_state)
  {
    return 0;
  }

  /*
   * Create a new table to hold the intercept locations for each player. This
   * will be passed back to the calling lua.
   */
  lua_newtable(lua_state);

  for (ii = 0; ii < g_match_state->players_per_team; ii++)
  {
    for (jj = 0; jj < 2; jj++)
    {
      player = g_match_state->teams[jj]->players[ii];

      /*
       * Calculate the intercept point for that player.
       */
      if (calculate_player_disc_path_intercept(player, disc_path, &intercept))
      {
        /*
         * The index into the main lua table is the player_index + num players *
         * team_index
         */
        lua_pushinteger(lua_state, ii + g_match_state->players_per_team * jj);

        /*
         * The player can catch the disc so we add a new row into the table.
         * The row is actually another table containing the team index, player
         * index, time of intercept (from now in seconds) and the x,y coords at
         * which the intercept can take place.
         */
        lua_newtable(lua_state);
        lua_pushstring(lua_state, "player_index");
        lua_pushinteger(lua_state, ii);
        lua_settable(lua_state, -3);
        lua_pushstring(lua_state, "team_index");
        lua_pushinteger(lua_state, jj);
        lua_settable(lua_state, -3);
        lua_pushstring(lua_state, "time_to_intercept");
        lua_pushnumber(lua_state, intercept.t);
        lua_settable(lua_state, -3);
        lua_pushstring(lua_state, "x");
        lua_pushnumber(lua_state, intercept.intercept.x);
        lua_settable(lua_state, -3);
        lua_pushstring(lua_state, "y");
        lua_pushnumber(lua_state, intercept.intercept.y);
        lua_settable(lua_state, -3);

        /*
         * This sets the player/team index to point to the table created above.
         */
        lua_settable(lua_state, -3);
      }
    }
  }

  return 1;
}

/*
 * callback_get_all_team_marks
 *
 * Retrieves a table linking each player on the current team to the players
 * which they are marking.
 *
 * Returns: One table keyed on player_index + team_index * players per team.
 *          The table consists of player_index, team_index, 
 *          mark_player_index, mark_team_index.
 */
int lua_callback_get_all_team_marks(lua_State *lua_state)
{
  /*
   * Local Variables.
   */
  PLAYER *player;
  int ii;
  int other_team_index = (0 == g_curr_team ? 1 : 0);

  DT_AI_LOG("(%i:%i) Player requested all team marks",
            g_curr_player,
            g_curr_team);

  /*
   * This table will hold one row per player in the current team.
   */
  lua_newtable(lua_state);

  for (ii = 0; ii < g_match_state->players_per_team; ii++)
  {
    player = g_match_state->teams[g_curr_team]->players[ii];

    /*
      * The index into the main lua table is the player_index + num players *
      * team_index
      */
    lua_pushinteger(lua_state, 
                    ii + g_match_state->players_per_team * g_curr_team);

    /*
     * Create the table linking player/team to the person that they are 
     * marking.
     */
    lua_newtable(lua_state);
    lua_pushstring(lua_state, "player_index");
    lua_pushinteger(lua_state, ii);
    lua_settable(lua_state, -3);
    lua_pushstring(lua_state, "team_index");
    lua_pushinteger(lua_state, g_curr_team);
    lua_settable(lua_state, -3);
    lua_pushstring(lua_state, "mark_player_index");
    lua_pushnumber(lua_state, player->marked_player_index);
    lua_settable(lua_state, -3);
    lua_pushstring(lua_state, "mark_team_index");
    lua_pushnumber(lua_state, other_team_index);
    lua_settable(lua_state, -3);

    /*
      * This sets the player/team index to point to the table created above.
      */
    lua_settable(lua_state, -3);
  }

  return 1;
}

/*
 * callback_set_desired_mark
 *
 * Checks that the requested mark index is valid and sets the player to mark
 * if it is.
 *
 * Parameters Order: {top}mark_index{bottom}
 *
 * Parameters: mark_index - The player index of the player on the other team
 *                          to mark.
 */
int lua_callback_set_desired_mark(lua_State *lua_state)
{
  /*
   * Local Variables.
   */
  int mark_index;
  PLAYER *player = g_match_state->teams[g_curr_team]->players[g_curr_player];

  /*
   * Lua stack verification. Checks that we got the right number and type of
   * parameters.
   */
  if (lua_gettop(lua_state) != 1 || !lua_isnumber(lua_state, -1))
  {
    DT_AI_LOG("(%i:%i) Error: Attempt to set desired mark to non integer: %s",
              g_curr_team,
              g_curr_player,
              lua_typename(lua_state, -1));
    return 0;
  }

  /*
   * Check that the desired mark is a valid player index.
   */
  mark_index = (int) lua_tonumber(lua_state, -1);
  if (mark_index < 0 || mark_index >= g_match_state->players_per_team)
  {
    DT_AI_LOG("(%i:%i) Error: Attempt to set desired mark to invalid index: %i",
              g_curr_team,
              g_curr_player,
              mark_index);
    return 0;
  }

  /*
   * Set the marked index.
   */
  player->marked_player_index = mark_index;

  return 0;
}

/*
 * lua_callback_get_current_position
 *
 * Retrieves the current position of the disc as a table of x,y,z coordinates.
 *
 * Returns: A table keyed on strings x,y,z with the floating point coords.
 */
int lua_callback_get_current_position(lua_State *lua_state)
{
  /*
   * Local Variables.
   */
  PLAYER *player = g_match_state->teams[g_curr_team]->players[g_curr_player];

  DT_AI_LOG("(%i:%i) Current position requested",
            g_curr_player,
            g_curr_team);

  /*
   * This table will hold the position as x, y coordinates.
   */
  lua_newtable(lua_state);

  /*
    * The index into the main lua table is the player_index + num players *
    * team_index
    */
  lua_pushstring(lua_state, "x");
  lua_pushnumber(lua_state, (lua_Number) player->position.x);
  lua_settable(lua_state, -3);
  lua_pushstring(lua_state, "y");
  lua_pushnumber(lua_state, (lua_Number) player->position.y);
  lua_settable(lua_state, -3);
  lua_pushstring(lua_state, "z");
  lua_pushnumber(lua_state, (lua_Number) player->position.z);
  lua_settable(lua_state, -3);

  return 1;
}

/*
 * lua_callback_stop_running
 *
 * Stops the player from running (doesn't change state). Will cause the 
 * arrived_at_location event to be thrown.
 */
int lua_callback_stop_running(lua_State *lua_state)
{
  /*
   * Local Variables.
   */
  PLAYER *player = g_match_state->teams[g_curr_team]->players[g_curr_player];

  /*
   * By setting the desired position to the current position the player will
   * hit the arrived at location event and will not move anywhere even if that
   * is not handled.
   */
  vector_copy_values(&(player->desired_position), &(player->position));

  return 0;
}

/*
 * lua_callback_is_attacking_left_to_right
 *
 * Returns 1 if the team that the current player is on is attacking from left
 * to right and 0 if they are going from right to left.
 */
int lua_callback_is_attacking_left_to_right(lua_State *lua_state)
{
  /*
   * Local Variables.
   */
  bool l_to_r = g_match_state->teams[g_curr_team]->attacking_left_to_right;
  int l_to_r_as_integer = l_to_r ? 1 : 0;

  lua_pushinteger(lua_state, l_to_r_as_integer);

  return 1;
}

/*
 * register_lua_callback_functions
 *
 * Called once for a lua_state object. Sets up callback names for each of the
 * functions that we can call from the lua scripts.
 *
 * Parameters: lua_state - This is the lua state that we are registering the
 *                         functions with.
 */
void register_lua_callback_functions(lua_State *lua_state)
{
  /*
   * Register each of the above functions. The names are arbitrary but if they
   * are changed then any current lua scripts will need changing as well.
   */
  lua_register(lua_state,
               "callback_is_attacking_left_to_right",
               lua_callback_is_attacking_left_to_right);
  lua_register(lua_state,
               "callback_stop_running",
               lua_callback_stop_running);
  lua_register(lua_state,
               "callback_get_current_position",
               lua_callback_get_current_position);
  lua_register(lua_state,
               "callback_set_player_desired_pos",
               lua_callback_set_player_desired_pos);
  lua_register(lua_state,
               "callback_set_player_speed",
               lua_callback_set_player_speed);
  lua_register(lua_state,
               "callback_get_team_desired_positions",
               lua_callback_get_team_desired_positions);
  lua_register(lua_state,
               "callback_get_team_positions",
               lua_callback_get_team_positions);
  lua_register(lua_state,
               "callback_get_disc_pos",
               lua_callback_get_disc_pos);
  lua_register(lua_state,
               "callback_calculate_all_intercept_times",
               lua_callback_calculate_all_intercept_times);
  lua_register(lua_state,
               "callback_get_disc_final_pos",
               lua_callback_get_disc_final_pos);
  lua_register(lua_state,
               "callback_get_pitch_dimensions",
               lua_callback_get_pitch_dimensions);
  lua_register(lua_state,
               "callback_get_all_team_marks",
               lua_callback_get_all_team_marks);
  lua_register(lua_state,
               "callback_set_desired_mark",
               lua_callback_set_desired_mark);
}
