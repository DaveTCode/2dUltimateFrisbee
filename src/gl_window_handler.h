/*
 * gl_window_handler.h
 *
 *  Created on: 16-Nov-2009
 *      Author: dat
 */

#ifndef GL_WINDOW_HANDLER_H_
#define GL_WINDOW_HANDLER_H_

/*
 * Return codes for init_gl.
 *
 * GL_INIT_OK - Returned if the gl context was initialised without any errors.
 * GL_INIT_FAIL - Returned if the gl context could not be initalised for any
 *                reason.
 */
#define GL_INIT_OK 0
#define GL_INIT_FAIL 1

int init_gl(SCREEN *);

#endif /* GL_WINDOW_HANDLER_H_ */
