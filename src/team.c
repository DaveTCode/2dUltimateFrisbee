/*
 * team.c
 *
 *  Created on: 15 Feb 2010
 *      Author: David Tyler
 */
#include "dt_logger.h"

#include "team.h"

/*
 * create_team
 *
 * Allocates the memory required for a team
 *
 * Returns: A pointer to the newly created memory
 */
TEAM *create_team()
{
  /*
   * Local Variables
   */
  TEAM *team;

  /*
   * Allocate the required memory
   */
  team = (TEAM *) DT_MALLOC(sizeof(TEAM));

  return(team);
}

/*
 * destroy_team
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: team - The object to be freed.
 */
void destroy_team(TEAM *team)
{
  /*
   * Free the object.
   */
  DT_FREE(team);
}
