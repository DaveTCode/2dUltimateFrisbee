/*
 * mouse_click_state.c
 *
 *  Created on: 23 Oct 2009
 *      Author: David
 */

#include "dt_logger.h"

#include "mouse_click_state.h"

/*
 * create_mouse_state
 *
 * Allocates the memory required for a mouse state object.
 *
 * Returns: A pointer to the new object.
 */
MOUSE_STATE *create_mouse_state()
{
  MOUSE_STATE *state;

  /*
   * Allocate the required memory for this object.
   */
  state = (MOUSE_STATE *) DT_MALLOC(sizeof(MOUSE_STATE));

  return(state);
}

/*
 * destroy_mouse_state
 *
 * Free the memory allocated for the mouse state object.
 *
 * Parameters: state - The object to be freed.
 */
void destroy_mouse_state(MOUSE_STATE *state)
{
  DT_FREE(state);
}
