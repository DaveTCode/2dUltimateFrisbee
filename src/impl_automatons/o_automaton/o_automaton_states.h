/*
 * o_automaton_states.h
 *
 *  Created on: 7 Mar 2010
 *      Author: David
 */

#ifndef O_AUTOMATON_STATES_H_
#define O_AUTOMATON_STATES_H_

struct automaton_state;

/*
 * The total number of o automaton states as named below.
 */
#define NUM_O_AUTOMATON_STATES 8

/*
 * Strings used to name the states in the automaton.
 */
#define O_AUTOMATON_STATE_FOLLOW_PLAYER "state_follow_player"
#define O_AUTOMATON_STATE_WAITING "state_waiting"
#define O_AUTOMATON_STATE_CATCHING_PULL "state_catching_pull"
#define O_AUTOMATON_STATE_PICKING_UP_DISC "state_picking_up_disc"
#define O_AUTOMATON_STATE_MOVE_DISC_TO_BRICK "state_move_disc_to_brick"
#define O_AUTOMATON_STATE_MOVE_DISC_TO_SIDELINE "state_move_disc_to_sideline"
#define O_AUTOMATON_STATE_RUNNING "state_running"
#define O_AUTOMATON_STATE_INTERCEPT_DISC "state_intercept_disc"

int create_o_automaton_states(struct automaton_state ***, int);

#endif /* O_AUTOMATON_STATES_H_ */
