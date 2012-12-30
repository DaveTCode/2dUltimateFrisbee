/*
 * camera_handler.h
 *
 * The camera handler is an object which controls the movement of the camera
 * around the screen.
 *
 *  Created on: 19 Nov 2009
 *      Author: David
 */

#ifndef CAMERA_HANDLER_H_
#define CAMERA_HANDLER_H_

struct screen;
struct match_state;

/*
 * The starting position of the camera and its scaling factor. These are linked.
 */
#define DEFAULT_SCALE_FACTOR 1.0f
#define DEFAULT_CAMERA_POS_X 0.0f
#define DEFAULT_CAMERA_POS_Y 0.0f
#define DEFAULT_CAMERA_ANGLE 0.0f

/*
 * Usually when the user pressed a key to move the camera we have a defined
 * distance that the camera moves each frame. If they are also holding the shift
 * key then this is the factor we multiply the usual distance by so that the
 * camera moves further with shift pressed.
 */
#define SHIFT_FACTOR 10.0f

/*
 * The default amount to add to each of the camera velocities if a key is
 * pressed. There is no reason to make these more realistic than just being
 * constants at the moment.
 */
#define DEFAULT_ADD_PLANE_VEL 1.0f
#define DEFAULT_SUB_PLANE_VEL -1.0f
#define DEFAULT_ADD_SCALE_FACTOR 0.01f
#define DEFAULT_SUB_SCALE_FACTOR 0.99f
#define DEFAULT_ADD_ANGLE_VEL 0.01f
#define DEFAULT_SUB_ANGLE_VEL -0.01f

/*
 * CAMERA_HANDLER
 *
 * Whenever the camera positions is updated on the MODELVIEW matrix this is
 * updated to correspond to it.
 *
 * It is used to convert BACK from screen coordinates to model coordinates.
 * That way, when the user clicks the screen we can call a function and get
 * back a pair of x,y world coordinates from it.
 */
typedef struct camera_handler
{
  float x_pos;
  float y_pos;
  float scale_factor;
  float camera_angle;
} CAMERA_HANDLER;

CAMERA_HANDLER *create_camera_handler();
void destroy_camera_handler(CAMERA_HANDLER *);
void translate_camera(CAMERA_HANDLER *, float, float);
void scale_camera(CAMERA_HANDLER *, float);
void convert_screen_to_world_coords(CAMERA_HANDLER *, int, int, float *, float *);
void convert_world_to_screen_coords(CAMERA_HANDLER *, int, int, float *, float *);
void look_at_location(CAMERA_HANDLER *, float, float, struct screen *);
void update_camera_position(struct match_state *);

#endif /* CAMERA_HANDLER_H_ */
