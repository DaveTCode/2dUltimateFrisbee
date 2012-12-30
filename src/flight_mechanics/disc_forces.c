/*
 * disc_forces.c
 *
 *  Created on: 05 Feb 2010
 *      Author: David Tyler
 */
#include "../dt_logger.h"

#include "disc_forces.h"

/*
 * create_disc_forces
 *
 * Allocates the memory required for a disc_forces
 *
 * Returns: A pointer to the newly created memory
 */
DISC_FORCES *create_disc_forces()
{
  /*
   * Local Variables
   */
  DISC_FORCES *disc_forces;

  /*
   * Allocate the required memory
   */
  disc_forces = (DISC_FORCES *) DT_MALLOC(sizeof(DISC_FORCES));

  return(disc_forces);
}

/*
 * destroy_disc_forces
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: disc_forces - The object to be freed.
 */
void destroy_disc_forces(DISC_FORCES *disc_forces)
{
  /*
   * Free the object.
   */
  DT_FREE(disc_forces);
}
