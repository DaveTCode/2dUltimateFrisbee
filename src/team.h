/*
 * team.h
 *
 *  Created on: 15 Feb 2010
 *      Author: David Tyler
 */

#ifndef TEAM_H_
#define TEAM_H_

#include <stdbool.h>

struct player;

/*
 * This is not currently in a config file so that we don't have to code around
 * it containing stupid values
 */
#define PLAYERS_PER_TEAM 7

/*
 * TEAM
 *
 * A structure used to hold the team details.
 *
 * players - Array of players in the team.
 * is_offense - Set to true when that team is on offence and false otherwise.
 * attacking_left_to_right - Set to true or false at the start of each point.
 */
typedef struct team
{
  struct player *players[PLAYERS_PER_TEAM];
  bool is_offense;
  bool attacking_left_to_right;
} TEAM;

TEAM *create_team();
void destroy_team(TEAM *);

#endif /* TEAM_H_ */
