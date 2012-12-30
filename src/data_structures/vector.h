/*
 * vector.h
 *
 *  Created on: 06 Feb 2010
 *      Author: David Tyler
 */
#ifndef VECTOR_H_
#define VECTOR_H_

/*
 * Simple 3 dimensional vector construct. Used in positions, velocities and
 * accelerations throughout the game.
 *
 * x,y,z - The coordinates with respect to some basis. Unless explicitly stated
 *         this is the pitch coordinate system with x along the length, y along
 *         the width and z being distance from the pitch with positive z being
 *         up.
 */
typedef struct vector3
{
  float x;
  float y;
  float z;
} VECTOR3;

void vector_copy_values(VECTOR3 *, VECTOR3 *);
void vector_set_values(VECTOR3 *, float, float, float);
VECTOR3 vector_add(VECTOR3 *, VECTOR3 *);
VECTOR3 vector_minus(VECTOR3 *, VECTOR3 *);
VECTOR3 mult_vector_by_scalar(VECTOR3 *, float);
float vector_dot_product(VECTOR3 *, VECTOR3 *);
VECTOR3 vector_cross_product(VECTOR3 *, VECTOR3 *);
float vector_length(VECTOR3 *);
VECTOR3 unit_vector(VECTOR3 *);
float smallest_angle_between_vectors(VECTOR3 *, VECTOR3 *);
VECTOR3 rotate_vector_around_vector(VECTOR3 *, float, VECTOR3 *);
float dist_between_vectors_2d(VECTOR3 *, VECTOR3 *);
bool vectors_equal(VECTOR3 *, VECTOR3 *);

#endif /* VECTOR_H_ */
