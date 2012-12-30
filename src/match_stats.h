/*
 * match_stats.h
 *
 * TODO: Implement soft cap, time cap etc.
 *
 *  Created on: 02 Apr 2010
 *      Author: David Tyler
 */

#ifndef MATCH_H_
#define MATCH_H_

#include "timer.h"

/*
 * MATCH_STATS
 *
 * All of the information about a ultimate frisbee match is stored in this
 * structure: timers, scores, caps etc.
 *
 * Does not hold information like animation handlers etc.
 *
 * game_timer - The timer used to keep track of the game time.
 * stall_count - The current stall count. May not be valid if no stall in place.
 * team_a_score - Score of teams[0] from match_state object.
 * team_b_score - Score of teams[1]
 * hard_cap - Team that reaches this score first wins.
 * game_length_s - Game ends after this number of s have passed regardless of
 *                 score.
 */
typedef struct match_stats
{
  struct timer *game_timer;
  struct timer *stall_count;
  unsigned int team_a_score;
  unsigned int team_b_score;
  unsigned int hard_cap;
  unsigned int game_length_s;
} MATCH_STATS;

MATCH_STATS *create_match_stats(unsigned int, unsigned int);
void destroy_match_stats(MATCH_STATS *);

#endif /* MATCH_STATS_H_ */
