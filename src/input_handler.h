/*
 * input_handler.h
 *
 * If any new keys are added to either of these then they MUST be initialised
 * in the appropriate init_*_input_state function.
 *
 *  Created on: 23 Nov 2009
 *      Author: David
 */

#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include <stdbool.h>
#include "SDL/SDL_events.h"

struct match_state;
struct screen;

/*
 * KEY_INPUT_STATE
 *
 * The state of the keys on the keyboard. We store these (and the mouse button
 * states) so that we can process what happens if multiple buttons are pressed
 * at the same time. For example: we may wish to have a different action occur
 * if the left ctrl is pressed at the same time as another key.
 */
typedef struct key_input_state
{
  bool left;
  bool right;
  bool up;
  bool down;
  bool left_shift;
  bool right_shift;
  bool left_ctrl;
  bool right_ctrl;
  bool esc;
  bool minus;
  bool equals;
} KEY_INPUT_STATE;

/*
 * MOUSE_INPUT_STATE
 *
 * Contains the state of each of the different mouse buttons. The only
 * difference is that the mouse wheel events are triggered every time the wheel
 * moves. So we keep track of the number of turns in a given direction. This is
 * capped at a maximum as defined by MAX_MOUSE_WHEEL_TURNS so that we don't end
 * up overflowing an integer because the user is an idiot.
 */
typedef struct mouse_input_state
{
  bool left_button;
  bool right_button;
  bool middle_button;
  int mouse_wheel_turns;
} MOUSE_INPUT_STATE;

KEY_INPUT_STATE *create_key_input_state();
void destroy_key_input_state(KEY_INPUT_STATE *);
MOUSE_INPUT_STATE *create_mouse_input_state();
void destroy_mouse_input_state(MOUSE_INPUT_STATE *);
void handle_keyboard_event(SDL_KeyboardEvent *,
                           SDL_EventType,
                           struct match_state *,
                           struct screen *);
void handle_mousebutton_event(SDL_MouseButtonEvent *, struct match_state *);
void user_input_to_camera_movement(struct match_state *,
                                   float *,
                                   float *,
                                   float *,
                                   float *);

#endif /* INPUT_HANDLER_H_*/
