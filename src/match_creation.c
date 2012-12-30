/*
 * match_creation.c
 *
 *  Created on: 3 Apr 2010
 *      Author: David
 */
#include "dt_logger.h"

#include "SDL/SDL.h"

#include "data_structures/vector.h"
#include "disc.h"
#include "match_state.h"
#include "match_stats.h"
#include "pitch.h"
#include "player.h"
#include "team.h"
#include "timer.h"

/*
 * put_players_on_line
 *
 * At the start of a point we put the players evenly spaced along the endzone
 * line.
 *
 * Parameters: match_state - Must have the player objects already created.
 */
void put_players_on_line(MATCH_STATE *match_state, bool team_0_left)
{
  /*
   * Local Variables.
   */
  float y_pos;
  int ls;
  int rs;
  int jj;

  /*
   * Set which team is one which side of the pitch.
   */
  ls = team_0_left ? 0 : 1;
  rs = team_0_left ? 1 : 0;

  /*
   * Loop through both teams at the same time placing the players on the pitch.
   */
  for (jj = 0; jj < match_state->players_per_team; jj++)
  {
    y_pos = 7.0f + ((float) jj) * 5.0f;

    vector_set_values(&(match_state->teams[ls]->players[jj]->position),
                      (float) match_state->pitch->endzone_depth_m,
                      y_pos,
                      0.0f);
    vector_set_values(&(match_state->teams[rs]->players[jj]->position),
                      (float) match_state->pitch->length_m -
                      (float) match_state->pitch->endzone_depth_m,
                      y_pos,
                      0.0f);
    vector_copy_values(&(match_state->teams[ls]->players[jj]->new_position),
                       &(match_state->teams[ls]->players[jj]->position));
    vector_copy_values(&(match_state->teams[rs]->players[jj]->new_position),
                       &(match_state->teams[rs]->players[jj]->position));
  }
}

/*
 * TODO: Sort this out into a better function that gives a player the disc.
 */
void put_disc_in_default_pos(MATCH_STATE *match_state)
{
  match_state->disc->position.x = 10.0f;
  match_state->disc->position.y = 10.0f;
  match_state->disc->position.z = 1.5f;
  vector_copy_values(&(match_state->disc->new_position),
                     &(match_state->disc->position));
  match_state->disc->disc_state = disc_on_ground;
}

/*
 * start_match
 *
 * This function will eventually be the entrance point for setting all the
 * initial conditions required to hand control over to the player and start a
 * match.
 *
 * Will not fill this comment block in properly until that has been completed.
 */
void start_match(MATCH_STATE *match_state)
{
  /*
   * Start the game timer running.
   */
  match_state->match_stats->game_timer->start_time_ms = SDL_GetTicks();

  /*
   * Put the players in the correct positions.
   */
  put_players_on_line(match_state, true);

  /*
   * Put the disc in the starting position for the point.
   */
  put_disc_in_default_pos(match_state);

  /*
   * TODO: Better handling of who starts on offense and which end.
   */
  match_state->teams[0]->is_offense = true;
  match_state->teams[1]->is_offense = false;
  match_state->teams[0]->attacking_left_to_right = true;
  match_state->teams[1]->attacking_left_to_right = true;
}
