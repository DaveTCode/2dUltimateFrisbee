/*
 * intercept.h
 *
 *  Created on: 25 May 2010
 *      Author: David Tyler
 */

#ifndef INTERCEPT_H_
#define INTERCEPT_H_

#include "../data_structures/vector.h"

struct player;
struct disc_path;

/*
 * INTERCEPT
 *
 * Contains information on the intercept of two objects.
 *
 * object_a - The first object's position.
 * object_b - The second object's position.
 * intercept - The location of the intercept (one must exist for this to be
 *             created).
 * t - The time at which the intercept occurs in fractional s.
 */
typedef struct intercept
{
  VECTOR3 *object_a;
  VECTOR3 *object_b;
  VECTOR3 intercept;
  float t;
} INTERCEPT;

INTERCEPT *create_intercept();
void destroy_intercept(INTERCEPT *);
bool calculate_player_disc_path_intercept(struct player *,
                                          struct disc_path *,
                                          INTERCEPT *);

#endif /* INTERCEPT_H_ */
