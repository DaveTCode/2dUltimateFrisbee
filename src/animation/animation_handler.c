/*
 * animation_handler.c
 *
 *  Created on: 24 Feb 2010
 *      Author: David Tyler
 */
#include "../dt_logger.h"

#include "animation.h"
#include "animation_handler.h"

/*
 * create_animation_handler
 *
 * Allocates the memory required for a animation_handler.
 *
 * Returns: A pointer to the newly created memory
 */

ANIMATION_HANDLER *create_animation_handler()
{
  /*
   * Local Variables
   */
  ANIMATION_HANDLER *animation_handler;

  /*
   * Allocate the required memory
   */
  animation_handler = (ANIMATION_HANDLER *) DT_MALLOC(sizeof(ANIMATION_HANDLER));

  return(animation_handler);
}

/*
 * destroy_animation_handler
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: animation_handler - The object to be freed.
 */
void destroy_animation_handler(ANIMATION_HANDLER *animation_handler)
{
  /*
   * Free the object.
   */
  DT_FREE(animation_handler);
}

/*
 * load_animation_data
 *
 * This function creates and loads up the different animations that are to be
 * used in the game.
 *
 * @@@DAT - currently this uses hard coded strings for the animation files.
 *          There is potential that we can change this in the future and pass
 *          some data in.
 *
 * Parameters: animation_handler - This must have been created previously and
 *                                 if the function is succesful it will be
 *                                 returned with an array of animation objects
 *                                 that describe all possible animations.
 *
 * Returns: One of LOAD_ANIMATION_DATA_RET_CODES.
 */
int load_animation_data(ANIMATION_HANDLER *animation_handler)
{
  /*
   * Local Variables.
   */
  int ret_code = LOAD_ANIMATION_DATA_OK;
  int rc;
  char *filename;
  int frame_width;
  int frame_height;
  ANIMATION_CHOICE curr_animation;

  /*
   * Go through all the different animation values in the enumeration of
   * animation choices. For each, load the appropriate animation object and put
   * it into the array.
   */
  for (curr_animation = 0;
       curr_animation < num_animations;
       curr_animation++)
  {
    /*
     * The filename is hardcoded based on which animation we are loading. It is
     * a coding error for their to be no corresponding animation file but we
     * still pass control back to the caller in case they want to handle it.
     */
    switch (curr_animation)
    {
      case running_animation:
        filename = "resources/animations/player_running_sheet.png";
        frame_width = 16;
        frame_height = 16;
        break;
      case standing_animation:
        filename = "resources/animations/player_standing_sheet.png";
        frame_width = 10;
        frame_height = 10;
        break;
      case jumping_animation:
        filename = "resources/animations/player_jumping_sheet.png";
        frame_width = 10;
        frame_height = 10;
        break;
      default:
        DT_DEBUG_LOG("Attempting to load animation for %i. No file specified\n",
                     curr_animation);
        ret_code = LOAD_ANIMATION_DATA_INVALID_ANIMATION;
        goto EXIT_LABEL;
    }

    /*
     * This function creates the animation object, loads the sprite sheet as an
     * image and puts it into memory.
     *
     * It can fail if the file did not exist or if SDL/opengl failed to convert
     * the image for use.
     */
    rc = create_animation(filename,
                          frame_width,
                          frame_height,
                          &(animation_handler->animations[curr_animation]));
    if (ANIMATION_CREATE_OK != rc)
    {
      DT_DEBUG_LOG("Failed to create an animation object. Err code: %i\n", rc);
      ret_code = LOAD_ANIMATION_DATE_CREATE_FAIL;
      goto EXIT_LABEL;
    }
  }

EXIT_LABEL:

  return(ret_code);
}

/*
 * handler_get_animation
 *
 * Retrieves the animation that corresponds to a value in the animation choice
 * enumeration.
 *
 * @@@DAT - At the moment this doesn't need to be a function as it simply
 * retrieves it directly from the array using the choice as an index.
 *
 * Parameters: animation_handler - The handler that controls all animations.
 *             animation_choice - The choice of animation as stored by the
 *                                player.
 *
 * Returns: The appropriate animation object.
 */
ANIMATION *handler_get_animation(ANIMATION_HANDLER *animation_handler,
                                 ANIMATION_CHOICE animation_choice)
{
  return(animation_handler->animations[animation_choice]);
}
