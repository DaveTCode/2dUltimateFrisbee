/*
 * camera_handler.c
 *
 * The camera handler is our way of handling the conversion between screen and
 * world coordinates. In fact, opengl uses a matrix to do the projection from
 * world to screen coordinates for us so all we need to be able to do is
 * convert back.
 *
 * Rather than build the inverse of the projection matrix properly we just
 * store the amount of scaling and translation that has been performed.
 *
 * Note that scaling and translating are NOT independent. S2*T1x != T1x*S2
 * and so we do have to take into account the current scale factor when we
 * translate. See inline for details.
 *
 * @@@DAT - It is likely this will be converted to building the inverse matrix
 * at some point in the future. This will be increasingly necessary if we
 * decide to reenable rotation of the view screen.
 *
 *  Created on: 19 Nov 2009
 *      Author: David
 */
#include "dt_logger.h"

#include <SDL/SDL_opengl.h>
#include "camera_handler.h"
#include "input_handler.h"
#include "match_state.h"
#include "screen.h"

/*
 * reset_camera_handler
 *
 * Resets all the velocities back to the standard. This is called by the
 * create_camera_handler function.
 *
 * Parameters: handler - Will be returned with all velocities set back to
 * standard.
 */
void reset_camera_handler(CAMERA_HANDLER *handler)
{
  handler->camera_angle = DEFAULT_CAMERA_ANGLE;
  handler->scale_factor = DEFAULT_SCALE_FACTOR;
  handler->x_pos = DEFAULT_CAMERA_POS_X;
  handler->y_pos = DEFAULT_CAMERA_POS_Y;
}

/*
 * create_camera_handler
 *
 * Create a camera handler object and the underlying physics info object.
 *
 * Returns: A pointer to the new camera handler.
 */
CAMERA_HANDLER *create_camera_handler()
{
  /*
   * Local Variables.
   */
  CAMERA_HANDLER *handler;

  /*
   * Allocate the memory required for the camera handler.
   */
  handler = (CAMERA_HANDLER *) DT_MALLOC(sizeof(CAMERA_HANDLER));

  /*
   * Reset the camera handler to the default values.
   */
  reset_camera_handler(handler);

  return(handler);
}

/*
 * destroy_camera_handler
 *
 * Free the memory used for a camera handler.
 *
 * Parameters: handler - The handler to be freed.
 */
void destroy_camera_handler(CAMERA_HANDLER *handler)
{
  /*
   * Free the handler itself.
   */
  DT_FREE(handler);
}

/*
 * translate_camera
 *
 * Single entrance point for translating the camera. Used so that we can
 * guarantee that the scale factor has been applied to the x and y translates.
 *
 * Parameters: handler - The camera handler to be updated.
 *             x - The amount of translation in the x direction.
 *             y - The amount of translation in the y direction.
 */
void translate_camera(CAMERA_HANDLER *handler, float x, float y)
{
  glTranslatef(x, y, 0.0f);
  handler->x_pos += handler->scale_factor * x;
  handler->y_pos += handler->scale_factor * y;
}

/*
 * scale_camera
 *
 * Call this function with a scaling factor so that when we change the camera
 * handler's scale factor we also know that the x,y positions have been
 * update.
 *
 * Parameters: handler - The camera handler object to be updated.
 *             scale_factor - The scale factor to be applied.
 */
void scale_camera(CAMERA_HANDLER *handler, float scale_factor)
{
  glScalef(scale_factor, scale_factor, 1.0f);
  handler->scale_factor *= scale_factor;
}

/*
 * convert_screen_to_world_coords
 *
 * Takes a pair of x,y screen coordinates (e.g. the location of the mouse) and
 * returns a pair of x,y world coordinates.
 *
 * Parameters: camera_handler - Contains the location of the camera in the
 *                              world.
 *             screen_x, screen_y - Integers corresponding to the screen coords.
 *             world_x, world_y - Pointers to the returned world coordinates.
 */
void convert_screen_to_world_coords(CAMERA_HANDLER *camera_handler,
                                    int screen_x,
                                    int screen_y,
                                    float *world_x,
                                    float *world_y)
{
  /*
   * Local Variables.
   */
  float temp_x = (float) screen_x;
  float temp_y = (float) screen_y;

  /*
   * Reverse the scaling factor.
   */
  temp_x /= camera_handler->scale_factor;
  temp_y /= camera_handler->scale_factor;

  /*
   * Finally reverse the translation to the coordinates.
   */
  temp_x -= camera_handler->x_pos / camera_handler->scale_factor;
  temp_y -= camera_handler->y_pos / camera_handler->scale_factor;

  *world_x = temp_x;
  *world_y = temp_y;
}

/*
 * convert_world_to_screen_coords
 *
 * Takes a pair of x,y world coordinates (e.g. the location of the disc) and
 * returns a pair of x,y screen coordinates.
 *
 * Parameters: camera_handler - Contains the location of the camera in the
 *                              world.
 *             world_x, world_y - Integers corresponding to the world coords.
 *             screen_x, screen_y - Pointers to the returned screen
 *                                  coordinates.
 */
void convert_world_to_screen_coords(CAMERA_HANDLER *camera_handler,
                                    int world_x,
                                    int world_y,
                                    float *screen_x,
                                    float *screen_y)
{
  /*
   * Local Variables.
   */
  float temp_x = (float) world_x;
  float temp_y = (float) world_y;

  /*
   * Apply the scaling factor.
   */
  temp_x *= camera_handler->scale_factor;
  temp_y *= camera_handler->scale_factor;

  /*
   * Apply the translation transformation.
   */
  temp_x += camera_handler->x_pos;
  temp_y += camera_handler->y_pos;

  *screen_x = temp_x;
  *screen_y = temp_y;
}

/*
 * look_at_location
 *
 * Takes a 2d coordinate and moves the camera to look at that location. Calling
 * this puts the given object at the centre of the screen.
 *
 * Parameters: x, y - The coordinates to look at.
 *             camera_handler - The camera to move.
 *             screen - Required to know how wide/high the screen is.
 */
void look_at_location(CAMERA_HANDLER *handler, float x, float y, SCREEN *screen)
{
  /*
   * Local Variables.
   */
  float delta_tx;
  float delta_ty;

  delta_tx =  (screen->centre_x - handler->x_pos) / handler->scale_factor - x;
  delta_ty =  (screen->centre_y - handler->y_pos) / handler->scale_factor - y;

  translate_camera(handler, delta_tx, delta_ty);
}

/*
 * update_camera_position
 *
 * TODO: Description
 *
 * Parameters: match_state - The general match state object that is passed
 *                           around between functions.
 */
void update_camera_position(MATCH_STATE *match_state)
{
  /*
   * Local Variables.
   */
  float x_translate;
  float y_translate;
  float rotate_angle;
  float scale_factor;

  /*
   * Find the amount that the user wants to move the camera by in each of the
   * x and y directions.
   */
  user_input_to_camera_movement(match_state,
                                &x_translate,
                                &y_translate,
                                &rotate_angle,
                                &scale_factor);

  /*
   * If the camera is to be moved either on the x axis or on the y axis then
   * do so now.
   */
  if ((x_translate != 0.0f) || (y_translate) != 0.0f)
  {
    translate_camera(match_state->camera_handler, x_translate, y_translate);
  }

  /*
   * Rotate the view. @@@DAT This is commented out until I decide it is
   * necessary. When it is then we need to make some big changes to the way
   * the camera handler keeps track of how to convert back from screen to
   * world coords.
   */
  /*if (rotate_angle != 0.0f)
  {
    match_state->camera_handler->camera_angle += rotate_angle;
  }*/

  /*
   * Update the matrix to scale everything on the screen.
   */
  if (scale_factor != 1.0f)
  {
    scale_camera(match_state->camera_handler, scale_factor);
  }
}
