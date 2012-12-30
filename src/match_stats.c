/*
 * match_stats.c
 *
 *  Created on: 02 Apr 2010
 *      Author: David Tyler
 */
#include "dt_logger.h"

#include "match_stats.h"
#include "timer.h"

/*
 * create_match_stats
 *
 * Allocates the memory required for a match_stats
 *
 * Returns: A pointer to the newly created memory
 */
MATCH_STATS *create_match_stats(unsigned int game_length_s,
                                unsigned int hard_cap)
{
  /*
   * Local Variables
   */
  MATCH_STATS *match_stats;

  /*
   * Allocate the required memory
   */
  match_stats = (MATCH_STATS *) DT_MALLOC(sizeof(MATCH_STATS));

  /*
   * Initialise values.
   */
  match_stats->game_length_s = game_length_s;
  match_stats->hard_cap = hard_cap;
  match_stats->team_a_score = 0;
  match_stats->team_b_score = 0;

  /*
   * Create match timers.
   */
  match_stats->game_timer = create_timer();
  match_stats->stall_count = create_timer();

  return(match_stats);
}

/*
 * destroy_match_stats
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: match_stats - The object to be freed.
 */
void destroy_match_stats(MATCH_STATS *match_stats)
{
  /*
   * Free timers.
   */
  destroy_timer(match_stats->game_timer);
  destroy_timer(match_stats->stall_count);

  /*
   * Free the object.
   */
  DT_FREE(match_stats);
}
