/*
 * disc_flight.h
 *
 *  Created on: 29 Jun 2011
 *      Author: David Tyler
 */

#ifndef DISC_FLIGHT_H_
#define DISC_FLIGHT_H_

#include "disc_forces.h"
#include "../data_structures/vector.h"

VECTOR3 update_disc_up_axis(VECTOR3 *, VECTOR3 *, float, float, float);
VECTOR3 find_disc_plane_axis(VECTOR3 *, VECTOR3 *);
void get_disc_acc_from_forces(DISC_FORCES *,
                              VECTOR3 *,
                              VECTOR3 *,
                              VECTOR3 *,
                              VECTOR3 *);
void update_extra_vels_from_disc_forces(DISC *, DISC_FORCES *, float);
DISC_FORCES calc_disc_forces(VECTOR3 *, VECTOR3 *, float);

#endif /* DISC_FLIGHT_H_ */
