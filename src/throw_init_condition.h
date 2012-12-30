/*
 * throw_init_condition.h
 *
 *  Created on: 31 Mar 2010
 *      Author: David Tyler
 */

#ifndef THROW_INIT_CONDITION_H_
#define THROW_INIT_CONDITION_H_

#include "data_structures/vector.h"

/*
 * THROW_INIT_CONDITION
 *
 * Corresponds to the conditions required to move the disc a certain distance
 * with a certain amount of roll. By precalculating these we can tabulate them
 * and use them as a lookup (with interpolation) to make the disc move a
 * certain distance.
 *
 * velocity - The initial velocity of the disc.
 * up - The initial up vector of the disc.
 * distance - The distance that this look up corresponds to.
 * roll - The amount of roll this look up corresponds to.
 */
typedef struct disc_throw_init_condition
{
  VECTOR3 velocity;
  VECTOR3 up;
  float distance;
  float roll;
} THROW_INIT_CONDITION;

THROW_INIT_CONDITION *create_throw_init_condition();
void destroy_throw_init_condition(THROW_INIT_CONDITION *);

#endif /* THROW_INIT_CONDITION_H_ */
