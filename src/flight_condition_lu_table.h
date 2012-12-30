/*
 * flight_condition_lu_table.h
 *
 *  Created on: 31 Mar 2010
 *      Author: David Tyler
 */

#ifndef FLIGHT_CONDITION_LU_TABLE_H_
#define FLIGHT_CONDITION_LU_TABLE_H_

struct disc_throw_init_condition;

/*
 * FLIGHT_CONDITION_LU_TABLE
 *
 * Contains a look up table of the initial throw conditions required to move
 * the disc a certain distance under a certain amount of roll.
 *
 * Because this cannot account for every possible set of conditions it should
 * be thought of more as a grid overlay onto R2. Then if you want to know the
 * actual throw conditions required to move distance x with roll r it is easy
 * to find the throw conditions closest on either side of x, r and interpolate
 * to produce a reasonably valid set of conditions for the throw.
 *
 * Perfection is not required and therefore not achieved with this method.
 * Accuracy can be increased by making distance_delta, roll_delta smaller.
 *
 * min_distance - The smallest distance this lookup table works for.
 * max_distance - The largest distance this table works for.
 * min_roll - The smallest amount of roll (negative) allowed.
 * max_roll - The largest amount of roll (positive) allowed.
 * distance_delta - Two adjacent conditions in the table are for distances x
 *                  and x + distance_delta.
 * roll_delta - Two adjacent conditions in the table are for roll r and
 *              r + roll_delta.
 * conditions - The table of throw conditions. Each column corresponds to a
 *              certain amount of roll and each row to a certain distance.
 */
typedef struct flight_condition_lu_table
{
  float min_distance;
  float max_distance;
  float min_roll;
  float max_roll;
  float distance_delta;
  float roll_delta;
  struct disc_throw_init_condition ***conditions;
} FLIGHT_CONDITION_LU_TABLE;

FLIGHT_CONDITION_LU_TABLE *create_flight_condition_lu_table();
void destroy_flight_condition_lu_table(FLIGHT_CONDITION_LU_TABLE *);

#endif /* FLIGHT_CONDITION_LU_TABLE_H_ */
