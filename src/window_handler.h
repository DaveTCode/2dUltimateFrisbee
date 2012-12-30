/*
 * window_handler.h
 *
 * Holds the structures and error codes used in the SDL initialisation and
 * screen creation.
 *
 *  Created on: 17 Oct 2009
 *      Author: David
 */

#ifndef WINDOW_HANDLER_H_
#define WINDOW_HANDLER_H_

struct screen;

/*
 * INIT_WINDOW_RET_CODES
 *
 * Return codes from the init screen function.
 *
 * INIT_OK - Initialisation was successful.
 * INIT_SDL_FAIL - Failed to initialise SDL subsystems.
 * INIT_SCREEN_FAIL - Failed to set up requested video mode.
 * INIT_TTF_FAIL - Failed to set up the TTF font subsystem.
 */
#define INIT_OK          0
#define INIT_SDL_FAIL    1
#define INIT_SCREEN_FAIL 2
#define INIT_TTF_FAIL    3

/*
 * Return codes from the update screen function.
 */
#define UPDATE_SCREEN_OK 0
#define FLIP_SCREEN_FAILED 1

int init_window_system(struct screen *, int, int, int);

#endif /* WINDOW_HANDLER_H_ */
