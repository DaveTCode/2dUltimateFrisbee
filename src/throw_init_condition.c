/*
 * throw_init_condition.c
 *
 *  Created on: 31 Mar 2010
 *      Author: David Tyler
 */

#include "dt_logger.h"

#include "throw_init_condition.h"

/*
 * create_throw_init_condition
 *
 * Allocates the memory required for a throw_init_condition
 *
 * Returns: A pointer to the newly created memory
 */
THROW_INIT_CONDITION *create_throw_init_condition()
{
  /*
   * Local Variables
   */
  THROW_INIT_CONDITION *throw_init_condition;

  /*
   * Allocate the required memory
   */
  throw_init_condition = (THROW_INIT_CONDITION *) DT_MALLOC(sizeof(THROW_INIT_CONDITION));

  return(throw_init_condition);
}

/*
 * destroy_throw_init_condition
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: throw_init_condition - The object to be freed.
 */
void destroy_throw_init_condition(THROW_INIT_CONDITION *throw_init_condition)
{
  /*
   * Free the object.
   */
  DT_FREE(throw_init_condition);
}
