/*
 * timer.c
 *
 *  Created on: 27 Feb 2010
 *      Author: David Tyler
 */
#include "dt_logger.h"

#include "timer.h"

/*
 * create_timer
 *
 * Allocates the memory required for a timer
 *
 * Returns: A pointer to the newly created memory
 */
TIMER *create_timer()
{
  /*
   * Local Variables
   */
  TIMER *timer;

  /*
   * Allocate the required memory
   */
  timer = (TIMER *) DT_MALLOC(sizeof(TIMER));

  /*
   * Set all internal variables to 0.
   */
  memset(timer, 0, sizeof(TIMER));

  return(timer);
}

/*
 * destroy_timer
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: timer - The object to be freed.
 */
void destroy_timer(TIMER *timer)
{
  /*
   * Free the object.
   */
  DT_FREE(timer);
}
