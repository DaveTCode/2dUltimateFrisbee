/*
 * gl_window_handler.h
 *
 *  Created on: 16-Nov-2009
 *      Author: dat
 */

#ifndef GAME_FUNCTIONS_H_
#define GAME_FUNCTIONS_H_

struct player;
struct disc;
struct match_state;

void catch_disc(struct player *, struct disc *);
void disc_lands(struct match_state *);
void turnover(struct match_state *);

#endif /* GAME_FUNCTIONS_H_ */
