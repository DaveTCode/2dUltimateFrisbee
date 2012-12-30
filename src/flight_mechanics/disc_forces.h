/*
 * disc_forces.h
 *
 *  Created on: 05 Feb 2010
 *      Author: David Tyler
 */

#ifndef DISC_FORCES_H_
#define DISC_FORCES_H_

/*
 * DISC_FORCES
 *
 * Puts all the forces acting on the disc into one structure that can be passed
 * around more easily.
 *
 * Parameters are all as forces not accelerations. Modify them using inertia or
 * mass as appropriate to get accelerations.
 */
typedef struct disc_forces
{
  float lift_force;
  float spin_down_moment;
  float pitch_moment;
  float roll_moment;
  float drag_force;
} DISC_FORCES;

DISC_FORCES *create_disc_forces();
void destroy_disc_forces(DISC_FORCES *);

#endif /* DISC_FORCES_H_ */
