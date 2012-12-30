/*
 * gl_window_handler.c
 *
 * Contains the functions for initialising the open gl contexts.
 *
 *  Created on: 16-Nov-2009
 *      Author: dat
 */
#include "dt_logger.h"

#include <SDL/SDL_opengl.h>
#include "screen.h"
#include "gl_window_handler.h"

/*
 * init_gl
 *
 * Initialise the open gl context. This only needs to be called once at the
 * beginning of the application.
 *
 * Parameters: screen - The screen object that defines the size of the screen.
 *
 * Returns: One of GL_INIT_RET_CODES.
 */
int init_gl(SCREEN *screen)
{
  /*
   * Local Variables.
   */
  int rc;
  int ret_code = GL_INIT_OK;

  /*
   * Set up opengl parameters.
   */
  glShadeModel(GL_SMOOTH);
  glClearColor(0, 0.5, 0, 1);
  glDisable(GL_DEPTH_TEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

  /*
   * Set up the blending function used.
   */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /*
   * We set the viewport to have 0,0 in the top left of the screen and to be
   * the same size as the screen resolution.
   */
  glViewport(0, 0, screen->width, screen->height);

  /*
   * Set up the projection transformation.
   */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, screen->width, screen->height, 0, 100, -100);


  /*
   * All drawing is done using the model view matrix.
   */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /*
   * Check whether there have been any errors whilst setting up open gl.
   */
  rc = glGetError();
  if (GL_NO_ERROR != rc)
  {
    ret_code = GL_INIT_FAIL;
  }

  return(ret_code);
}
