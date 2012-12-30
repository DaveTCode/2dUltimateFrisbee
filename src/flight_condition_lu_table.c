/*
 * flight_condition_lu_table.c
 *
 *  Created on: 31 Mar 2010
 *      Author: David Tyler
 */

#include "dt_logger.h"

#include "flight_condition_lu_table.h"

/*
 * create_flight_condition_lu_table
 *
 * Allocates the memory required for a flight_condition_lu_table
 *
 * Returns: A pointer to the newly created memory
 */
FLIGHT_CONDITION_LU_TABLE *create_flight_condition_lu_table()
{
  /*
   * Local Variables
   */
  FLIGHT_CONDITION_LU_TABLE *flight_condition_lu_table;

  /*
   * Allocate the required memory
   */
  flight_condition_lu_table = (FLIGHT_CONDITION_LU_TABLE *) DT_MALLOC(sizeof(FLIGHT_CONDITION_LU_TABLE));

  return(flight_condition_lu_table);
}

/*
 * destroy_flight_condition_lu_table
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: flight_condition_lu_table - The object to be freed.
 */
void destroy_flight_condition_lu_table(FLIGHT_CONDITION_LU_TABLE *flight_condition_lu_table)
{
  /*
   * Free the object.
   */
  DT_FREE(flight_condition_lu_table);
}
