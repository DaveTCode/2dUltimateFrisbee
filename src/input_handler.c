/*
 * input_handler.c
 *
 *  Created on: 24 Oct 2009
 *      Author: David
 */

#include "dt_logger.h"

#include "SDL/SDL.h"

#include "ai_general/ai_event_handler.h"
#include "automaton_handler.h"
#include "camera_handler.h"
#include "disc.h"
#include "disc_path.h"
#include "impl_automatons/generic_o_d_files/event_names.h"
#include "input_handler.h"
#include "match_state.h"
#include "mouse_click_state.h"
#include "screen.h"
#include "throw.h"

/*
 * create_key_input_state
 *
 * Create a key_input_state object. This also sets all the key booleans to
 * false.
 *
 * Returns: A pointer to the newly created memory.
 */
KEY_INPUT_STATE *create_key_input_state()
{
  KEY_INPUT_STATE *state;

  /*
   * Allocate the memory requried for the key input state.
   */
  state = (KEY_INPUT_STATE *) DT_MALLOC(sizeof(KEY_INPUT_STATE));

  /*
   * Set the whole of the state to false using memset. This means that all keys
   * start with a value of false. Not good coding practice but it saves on odd
   * bugs where we forget to initialise a new key to false in an init routine.
   */
  memset(state, '\0', sizeof(KEY_INPUT_STATE));

  return(state);
}

/*
 * destroy_key_input_state
 *
 * Releases the memory allocated for a KEY_INPUT_STATE object.
 */
void destroy_key_input_state(KEY_INPUT_STATE *state)
{
  DT_FREE(state);
}

/*
 * create_mouse_input_state
 *
 * Create a mouse_input_state object. This also sets all the mouse booleans to
 * false.
 *
 * Returns: A pointer to the newly created memory.
 */
MOUSE_INPUT_STATE *create_mouse_input_state()
{
  MOUSE_INPUT_STATE *state;

  /*
   * Allocate the memory requried for the mouse input state.
   */
  state = (MOUSE_INPUT_STATE *) DT_MALLOC(sizeof(MOUSE_INPUT_STATE));

  /*
   * Set the whole of the state to false using memset. This means that all keys
   * start with a value of false. Not good coding practice but it saves on odd
   * bugs where we forget to initialise a new key to false in an init routine.
   */
  memset(state, '\0', sizeof(MOUSE_INPUT_STATE));

  return(state);
}

/*
 * destroy_mouse_input_state
 *
 * Releases the memory allocated for a MOUSE_INPUT_STATE object.
 */
void destroy_mouse_input_state(MOUSE_INPUT_STATE *state)
{
  DT_FREE(state);
}

/*
 * handle_keyboard_event
 *
 * Called when there is a keyboard event in the main game loop. This function
 * only sets the key input handler states so that they can be dealt with in
 * other functions.
 *
 * Parameters: event - The event that causes this function call.
 *             type - The type of keyboard event. Either a key up or down.
 */
void handle_keyboard_event(SDL_KeyboardEvent *event,
                           SDL_EventType type,
                           MATCH_STATE *match_state,
                           SCREEN *screen)
{
  /*
   * Local Variables.
   */
  bool set_key_val;

  /*
   * If this is a key press then we are going to set a key state to true.
   * Otherwise we set it to false.
   */
  if (type == SDL_KEYDOWN)
  {
    set_key_val = true;
  }
  else
  {
    set_key_val = false;
  }

  /*
   * Set the key value for whatever key was pressed.
   */
  if (event->keysym.sym == SDLK_LEFT)
  {
    match_state->key_input_state->left = set_key_val;
  }
  else if (event->keysym.sym == SDLK_RIGHT)
  {
    match_state->key_input_state->right = set_key_val;
  }
  else if (event->keysym.sym == SDLK_DOWN)
  {
    match_state->key_input_state->down = set_key_val;
  }
  else if (event->keysym.sym == SDLK_UP)
  {
    match_state->key_input_state->up = set_key_val;
  }
  else if (event->keysym.sym == SDLK_EQUALS)
  {
    match_state->key_input_state->equals = set_key_val;
  }
  else if (event->keysym.sym == SDLK_MINUS)
  {
    match_state->key_input_state->minus = set_key_val;
  }
  else if (event->keysym.sym == SDLK_LCTRL)
  {
    match_state->key_input_state->left_ctrl = set_key_val;
  }
  else if (event->keysym.sym == SDLK_RCTRL)
  {
    match_state->key_input_state->right_ctrl = set_key_val;
  }
  else if (event->keysym.sym == SDLK_LSHIFT)
  {
    match_state->key_input_state->left_shift = set_key_val;
  }
  else if (event->keysym.sym == SDLK_RSHIFT)
  {
    match_state->key_input_state->right_shift = set_key_val;
  }
}

/*
 * handle_mousebutton_event
 *
 * Called when there is a mouse button event (this includes the mousewheel
 * being moved). The function parses out what the event was and hands control
 * over to more specific functions.
 *
 * Parameters: event - The event that caused the function call.
 *             match_state - The state of the match.
 */
void handle_mousebutton_event(SDL_MouseButtonEvent *event,
                              MATCH_STATE *match_state)
{
  /*
   * First we check whether there is already a throw in progress. This will be
   * not null from the moment a mouse button is pressed until it is released
   * and the disc attributes are calculated from it.
   */
  if (match_state->throw->in_progress)
  {
    if (event->type == SDL_MOUSEBUTTONUP)
    {
      if (event->button == match_state->throw->start_state->create_button)
      {
        /*
         * Fill in the details for the end state of the throw from the event.
         * The throw object was created when the mouse button was first pressed
         * and that created the end_state object underlying it.
         */
        match_state->throw->end_state->screen_x = event->x;
        match_state->throw->end_state->screen_y = event->y;
        match_state->throw->end_state->create_time = SDL_GetTicks();
        match_state->throw->end_state->create_button = event->button;

        /*
         * Convert the screen coordinates into world coordinates for use in
         * calculating where the throw should go.
         */
        convert_screen_to_world_coords(
            match_state->camera_handler,
            event->x,
            event->y,
            &(match_state->throw->end_state->world_x),
            &(match_state->throw->end_state->world_y));

        /*
         * Set disc state using the throw object that has just been created.
         */
        set_init_disc_conditions(match_state->disc, match_state->throw);

        /*
         * Create a disc path to associate with the current throw. This disc
         * path has a very low interval so that we can use it to plot parts of
         * the path if required.
         *
         * If there was already a disc path object then free it up.
         */
        if (NULL != match_state->disc_path)
        {
          destroy_disc_path(match_state->disc_path);
        }
        match_state->disc_path = create_disc_path();
        init_disc_path(match_state->disc,
                       match_state->disc_path,
                       DISC_PATH_INTERVAL_MAX);

        /*
         * Set the information on who the thrower was.
         */ 
        match_state->disc->thrower = match_state->disc->holder;

        /*
         * Throw an event to all players to indicate that the disc is now in 
         * the air again.
         */ 
        throw_multi_player_ai_event_by_name(match_state->teams,
                                            match_state->players_per_team,
                                            match_state->automaton_handler->offensive_set->start_automaton,
                                            AUTOMATON_EVENT_DISC_RELEASED);

        /*
         * Set the throw in progress flag to false for next time.
         */
        match_state->throw->in_progress = false;
      }
      else
      {
        /*
         * If the buttons don't match then cancel the throw.
         *
         * We don't need to do anything with the created throw as it will just
         * get overwritten the next time that a moust button is clicked.
         */
        match_state->throw->in_progress = false;
      }
    }
  }
  else if (disc_with_player == match_state->disc->disc_state)
  {
    if (event->type == SDL_MOUSEBUTTONDOWN)
    {
      if (event->button == SDL_BUTTON_LEFT ||
          event->button == SDL_BUTTON_RIGHT)
      {
        /*
         * Set the initial variables for the mouse state.
         */
        match_state->throw->start_state->screen_x = event->x;
        match_state->throw->start_state->screen_y = event->y;
        match_state->throw->start_state->create_time = SDL_GetTicks();
        match_state->throw->start_state->create_button = event->button;

        /*
         * Convert the screen coordinates into world coordinates for use in
         * calculating where the throw should go.
         *
         * Note that this fills in variables in the throw start state!
         */
        convert_screen_to_world_coords(
            match_state->camera_handler,
            event->x,
            event->y,
            &(match_state->throw->start_state->world_x),
            &(match_state->throw->start_state->world_y));

        /*
         * By telling the match state that there is a throw in progress it
         * means that we can both complete the throw or cancel it.
         */
        match_state->throw->in_progress = true;
      }
    }
  }
}

/*
 * user_input_to_camera_movement
 *
 * Converts the user input in the match state object into distances that the
 * camera should move this frame.
 *
 * Parameters: match_state - The general game state object that is passed around
 *                           between functions.
 *             translate_x - Will be returned as the distance that the camera
 *                           is going to move in the x axis.
 *             translate_y - Will be returned as the distance that the camera
 *                           is going to move in the y axis.
 *             rotate_angle - Will be returned as the angle of rotation for the
 *                            camera.
 *             scale_factor - Will be returned as the scale factor for the
 *                            camera.
 */
void user_input_to_camera_movement(MATCH_STATE *match_state,
                                   float *x_translate,
                                   float *y_translate,
                                   float *rotate_angle,
                                   float *scale_factor)
{
  /*
   * Local Variables.
   */
  float step_movement = DEFAULT_ADD_PLANE_VEL;
  float angle_movement = DEFAULT_ADD_ANGLE_VEL;
  float scale_movement = DEFAULT_ADD_SCALE_FACTOR;

  /*
   * Set the passed in variables to their default values.
   */
  *x_translate = 0.0f;
  *y_translate = 0.0f;
  *rotate_angle = 0.0f;
  *scale_factor = 1.0f;

  /*
   * If the user is currently holding either shift key then the camera
   * displacement is multiplied by a factor.
   */
  if (match_state->key_input_state->left_shift ||
      match_state->key_input_state->right_shift)
  {
    step_movement *= SHIFT_FACTOR;
    angle_movement *= SHIFT_FACTOR;
  }

  /*
   * If the user has pressed any of the arrow keys then we move the camera in
   * that direction by the distance defined above.
   */
  if (match_state->key_input_state->left_ctrl ||
      match_state->key_input_state->right_ctrl)
  {
    if (match_state->key_input_state->left)
    {
      *rotate_angle -= angle_movement;
    }
    if (match_state->key_input_state->right)
    {
      *rotate_angle += angle_movement;
    }
    if (match_state->key_input_state->up)
    {
      // Unused
    }
    if (match_state->key_input_state->down)
    {
      // Unused
    }
  }
  else
  {
    if (match_state->key_input_state->left)
    {
      (*x_translate) -= step_movement;
    }
    if (match_state->key_input_state->right)
    {
      (*x_translate) += step_movement;
    }
    if (match_state->key_input_state->up)
    {
      (*y_translate) -= step_movement;
    }
    if (match_state->key_input_state->down)
    {
      (*y_translate) += step_movement;
    }
  }

  /*
   * Set the scaling factor of the camera based on the keys that are pressed.
   * Increase the scaling factor if the = button is pressed and decrease it if
   * the - button is pressed.
   */
  if (match_state->key_input_state->equals)
  {
    *scale_factor += scale_movement;
  }
  if (match_state->key_input_state->minus)
  {
    *scale_factor -= scale_movement;
  }
}
