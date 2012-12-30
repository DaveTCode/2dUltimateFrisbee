/*
 * screen.c
 *
 *  Created on: 19 Oct 2009
 *      Author: David
 */
#include "dt_logger.h"

#include <SDL/SDL.h>
#include "screen.h"

/*
 * dt_create_screen
 *
 * Creates a new screen object..
 *
 * Returns: A pointer to the screen object created.
 *
 * Parameters: None.
 */
SCREEN *create_screen()
{
  /*
   * Local Variables.
   */
  SCREEN *temp_screen;

  /*
   * Allocate memory for the screen object.
   */
  temp_screen = (SCREEN *) DT_MALLOC(sizeof(SCREEN));

  /*
   * Set up the viewport to be a null surface.
   */
  temp_screen->viewport = NULL;

  return(temp_screen);
}

/*
 * destroy_screen
 *
 * Free the memory allocated to a screen object.
 *
 * Returns: Nothing.
 *
 * Parameters: IN     screen - The object to be freed.
 *
 * Operation: Free memory.
 */
void destroy_screen(SCREEN *screen)
{
  /*
   * If the screen has a viewport attached then free it.
   */
  if (NULL != screen->viewport)
  {
    SDL_FreeSurface(screen->viewport);
  }

  DT_FREE(screen);

  return;
}
