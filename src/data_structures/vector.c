/*
 * vector.c
 *
 * This file contains the list of functions that can act on vectors.
 *
 * All the functions that return a vector use a new structure and pass it back
 * by value to avoid having to allocate any new memory or act in place on a
 * structure.
 *
 *  Created on: 06 Feb 2010
 *      Author: David Tyler
 */
#include "../dt_logger.h"

#include <math.h>
#include "vector.h"

/*
 * vector_copy_values
 *
 * Copy the values of the second parameter into the first. Will overwrite the
 * values in the first vector.
 *
 * Parameters: vector_old - The vector which will be overwritten.
 *             vector_new - The vector whose values will be copied.
 */
void vector_copy_values(VECTOR3 *vector_old, VECTOR3 *vector_new)
{
  vector_old->x = vector_new->x;
  vector_old->y = vector_new->y;
  vector_old->z = vector_new->z;
}

/*
 * vector_set_values
 *
 * Set the values of a vector with the passed in values.
 *
 * Parameters: vector - The vector to be overwritten.
 *             x, y, z - The values to be copied.
 */
void vector_set_values(VECTOR3 *vector, float x, float y, float z)
{
  vector->x = x;
  vector->y = y;
  vector->z = z;
}

/*
 * vector_add
 *
 * Adds two vectors returning the value as a new vector. Order of variables is
 * irrelevant.
 *
 * Parameters: vector1
 *             vector2
 *
 * Returns: vector1 + vector2
 */
VECTOR3 vector_add(VECTOR3 *vector1, VECTOR3 *vector2)
{
  /*
   * Local Variables.
   */
  VECTOR3 ret_vector;

  ret_vector.x = vector1->x + vector2->x;
  ret_vector.y = vector1->y + vector2->y;
  ret_vector.z = vector1->z + vector2->z;

  return(ret_vector);
}

/*
 * vector_minus
 *
 * Returns the result of vector1 - vector2.
 *
 * Parameters: vector1
 *             vector2
 *
 * Returns: A new vector.
 */
VECTOR3 vector_minus(VECTOR3 *vector1, VECTOR3 *vector2)
{
  /*
   * Local Variables.
   */
  VECTOR3 ret_vector;

  ret_vector.x = vector1->x - vector2->x;
  ret_vector.y = vector1->y - vector2->y;
  ret_vector.z = vector1->z - vector2->z;

  return(ret_vector);
}

/*
 * mult_vector_by_scalar
 *
 * Multiplies a vector by a scalar value and returns the result as a new vector.
 *
 * Parameters: vector
 *             scalar
 *
 * Returns: scalar.vector
 */
VECTOR3 mult_vector_by_scalar(VECTOR3 *vector, float scalar)
{
  /*
   * Local Variables.
   */
  VECTOR3 ret_vector;

  ret_vector.x = vector->x * scalar;
  ret_vector.y = vector->y * scalar;
  ret_vector.z = vector->z * scalar;

  return(ret_vector);
}

/*
 * vector_dot_product
 *
 * Performs a vector dot product on two vectors and returns the result as a
 * floating point number.
 *
 * Parameters: vector1
 *             vector2
 *
 * Returns: vector1.vector2
 */
float vector_dot_product(VECTOR3 *vector1, VECTOR3 *vector2)
{
  return(vector1->x * vector2->x +
         vector1->y * vector2->y +
         vector1->z * vector2->z);
}

/*
 * vector_cross_product
 *
 * Performs a cross product of two 3 dimensional vectors an returns the result
 * as a new vector.
 *
 * Parameters: vector1
 *             vector2
 *
 * Returns: vector1 x vector2
 */
VECTOR3 vector_cross_product(VECTOR3 *vector1, VECTOR3 *vector2)
{
  /*
   * Local Variables.
   */
  VECTOR3 ret_vector;

  ret_vector.x = vector1->y * vector2->z - vector1->z * vector2->y;
  ret_vector.y = vector1->z * vector2->x - vector1->x * vector2->z;
  ret_vector.z = vector1->x * vector2->y - vector1->y * vector2->x;

  return(ret_vector);
}

/*
 * vector_length
 *
 * Calculates the length of the vector as a floating point number.
 *
 * Parameters: vector
 *
 * Returns: (v.v)^(1/2)
 */
float vector_length(VECTOR3 *vector)
{
  return(sqrtf(vector_dot_product(vector, vector)));
}

/*
 * unit_vector
 *
 * Finds a unit vector in the direction of the given vector.
 *
 * Parameters: vector
 *
 * Returns: v / |v|
 */
VECTOR3 unit_vector(VECTOR3 *vector)
{
  /*
   * Local Variables.
   */
  float length = vector_length(vector);

  return(mult_vector_by_scalar(vector, 1.0f / length));
}

/*
 * smallest_angle_between_vectors
 *
 * Calculates the angle between two vectors using the formula:
 * v1.v2 / |v1||v2| = cos(angle)
 *
 * Obviously the two vectors cannot be zero or this equation doesn't make sense.
 *
 * Note that acosf only returns values in the range [0, PI] so we do not have
 * to adjust the output of that function to get the smallest angle
 *
 * Parameters: vector1, vector2 - The order of vectors makes no difference.
 *
 * Returns: The shortest angle between the two vectors.
 */
float smallest_angle_between_vectors(VECTOR3 *vector1, VECTOR3 *vector2)
{
  /*
   * Local Variables.
   */
  float angle;
  float acos_angle;

  DT_ASSERT(vector_length(vector1) > 0 && vector_length(vector2) > 0);

  acos_angle = vector_dot_product(vector1, vector2) /
      (vector_length(vector1) * vector_length(vector2));

  /*
   * Due to rounding errors |acos_angle| may be greater than 1. To remove this
   * issue (we can't take acos(angle)) simply reduce it if that is the case.
   */
  if (acos_angle > 1.0f)
  {
    acos_angle = 1.0f;
  }
  else if (acos_angle < -1.0f)
  {
    acos_angle = -1.0f;
  }
  angle = acosf(acos_angle);

  return(angle);
}

/*
 * rotate_vector_around_vector
 *
 * Takes one vector and rotates it a certain number of radians around a second
 * vector. The second vector has one end at the origin so we don't need to do
 * the translation part of the matrix manipulation.
 *
 * The maths in this function has been tested rigorously against other rotation
 * routines and found to be correct in all cases.
 *
 * It is an expanded form of the following matrix manipulation:
 * rotated_vector = v . (R_x * R_y * R_z * R_y^(-1) * R_x^(-1))
 * where R_x is the matrix that rotates vector 2 onto the xz plane, R_y is the
 * matrix that rotates vector2 onto the z axis and R_z is the z axis rotation
 * of angle theta.
 *
 * RECALL: The rotation is anti-clockwise from the perspective of looking down
 * vector2 towards the origin.
 *
 * Parameters: vector1 - The vector to be rotated.
 *             angle - The angle in radians of the rotation.
 *             vector2 - The vector around which we will rotate. This need not
 *                       be normalised as that is done in this function.
 */
VECTOR3 rotate_vector_around_vector(VECTOR3 *vector1,
                                   float angle,
                                   VECTOR3 *vector2)
{
  /*
   * Local Variables.
   */
  VECTOR3 rotated_vector;
  float sin_angle = sinf(angle);
  float cos_angle = cosf(angle);
  float x = vector1->x;
  float y = vector1->y;
  float z = vector1->z;

  /*
   * The rotation only works around a unit vector so convert the input vector
   * into a unit vector first.
   */
  VECTOR3 unit_vector2 = unit_vector(vector2);
  float u = unit_vector2.x; float u2 = u * u;
  float v = unit_vector2.y; float v2 = v * v;
  float w = unit_vector2.z; float w2 = w * w;
  float vector1_dot_vector2 = vector_dot_product(vector1, &unit_vector2);

  /*
   * The following is equivalent to rotating the rotation axis to lie along the
   * z axis, rotating the vector along that axis and then performing the inverse
   * rotation to put the axis back.
   *
   * We expand it for speed of calculation.
   */
  rotated_vector.x = u * vector1_dot_vector2 +
                     cos_angle * (x * (v2 + w2) - u * (v * y + w * z)) +
                     (v * z - w * y) * sin_angle;
  rotated_vector.y = v * vector1_dot_vector2 +
                     cos_angle * (y * (u2 + w2) - v * (u * x + w * z)) +
                     (w * x - u * z) * sin_angle;
  rotated_vector.z = w * vector1_dot_vector2 +
                     cos_angle * (z * (u2 + v2) - w * (u * x + v * y)) +
                     (u * y - v * x) * sin_angle;

  return(rotated_vector);
}

/*
 * dist_between_vectors_2d
 *
 * Calculates the distance between two vectors projected down on to the plane.
 * Sum of squares.
 *
 * Parameters: vector1
 *             vector2
 *
 * Returns: The result of (x1-x2)^2 + (y1-y2)^2
 */
float dist_between_vectors_2d(VECTOR3 *vector1, VECTOR3 *vector2)
{
  return (vector1->x - vector2->x) * (vector1->x - vector2->x) +
         (vector1->y - vector2->y) * (vector1->y - vector2->y);
}

/*
 * vectors_equal
 *
 * Returns true if two vectors are identical. Note this doesn't take
 * into account float rounding errors.
 *
 * Parameters: vector1, vector2 - Vectors to compare. Order is irrelevant.
 *
 * Returns: true if the vectors are identical and false otherwise.
 */
bool vectors_equal(VECTOR3 *vector1, VECTOR3 *vector2)
{
  return (vector1->x == vector2->x && 
          vector1->y == vector2->y && 
          vector1->z == vector2->z);
}
