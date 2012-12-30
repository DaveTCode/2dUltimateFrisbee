/*
 * mouse_click_state.h
 *
 * Contains structures for handling the state of the mouse when it is clicked.
 * This information can then be used by other functions that do things like
 * create a throw object.
 *
 *  Created on: 23 Oct 2009
 *      Author: David
 */

#ifndef MOUSE_CLICK_STATE_H_
#define MOUSE_CLICK_STATE_H_

/*
 * MOUSE_STATE
 *
 * The state of the mouse when it was clicked.
 *
 * screen_x - The x coordinate of the cursor on the screen when the first button
 * was clicked
 * screen_y - The y coordinate of the cursor on the screen when the first button
 * was clicked.
 * create_button - The button that was clicked first (and thus created this
 * object). One of SDL_BUTTON_LEFT etc.
 * create_time - The time (as retrieved by SDL_GetTicks()) that this object was
 * created.
 */
typedef struct mouse_state
{
  int screen_x;
  int screen_y;
  float world_x;
  float world_y;
  int create_button;
  int create_time;
} MOUSE_STATE;

MOUSE_STATE *create_mouse_state();
void destroy_mouse_state(MOUSE_STATE *);

#endif /* MOUSE_CLICK_STATE_H_ */
