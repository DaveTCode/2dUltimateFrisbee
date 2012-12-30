/*
 * player_ai.h
 *
 *  Created on: 29/06/2011
 *      Author: David Tyler
 */

#ifndef PLAYER_AI_H_
#define PLAYER_AI_H_

#include "SDL/SDL_stdinc.h"

struct team;

void process_all_player_ai(struct team **, int, Uint32);

#endif /* PLAYER_AI_H_ */
