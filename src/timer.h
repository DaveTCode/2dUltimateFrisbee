/*
 * timer.h
 *
 *  Created on: 27 Feb 2010
 *      Author: David Tyler
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <SDL/SDL.h>

/*
 * TIMER
 *
 * This represents a game timer. The counter is incremented on the timestep.
 *
 * WARNING: This updating is done on the main thread and is therefore only
 * updated once per frame. Do NOT set the time_step to be less than the number
 * of milliseconds for a frame or you will get undesired behaviour.
 *
 * start_time_ms - The time it was created (gathered from SDL_GetTicks()).
 */
typedef struct timer
{
  Uint32 start_time_ms;
} TIMER;

TIMER *create_timer();
void destroy_timer(TIMER *);

#endif /* TIMER_H_ */
