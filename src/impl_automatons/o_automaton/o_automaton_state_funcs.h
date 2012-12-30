/*
 * o_automaton_states.h
 *
 *  Created on: 7 Mar 2010
 *      Author: David
 */

#ifndef O_AUTOMATON_STATE_FUNCS_H_
#define O_AUTOMATON_STATE_FUNCS_H_

#include "SDL/SDL_stdinc.h"

struct player;

void state_waiting_entrance_function(struct player *);
int state_waiting_function(struct player *, Uint32);
int state_catching_pull_function(struct player *, Uint32);
int state_picking_up_disc_function(struct player *, Uint32);
int state_move_disc_to_brick_function(struct player *, Uint32);
int state_move_disc_to_sideline_function(struct player *, Uint32);
void state_running_entrance_function(struct player *);
int state_running_function(struct player *, Uint32);
void state_running_exit_function(struct player *);
int state_intercept_disc_function(struct player *, Uint32);
void state_intercept_disc_exit_function(struct player *);
int state_follow_player_function(struct player *, Uint32);
void state_follow_player_start_function(struct player *);
void state_follow_player_exit_function(struct player *);

#endif /* O_AUTOMATON_STATE_FUNCS_H_ */
