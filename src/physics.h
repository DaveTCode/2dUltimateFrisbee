/*
 * physics.h
 *
 *  Created on: 27 Oct 2009
 *      Author: David
 */

#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "SDL/SDL_stdinc.h"

struct vector3;
struct match_state;

/*
 * Hard code the atmospheric density. This is independent of the disc model but
 * is a constant like the acceleration due to gravity.
 */
#define ATMOSPHERIC_DENSITY_KG_PER_M3     1.23f
#define ATMOSPHERIC_DESNTIY_KG_PER_M3_SQR 1.5129f

/*
 * Acceleration due to gravity is defined as a constant and we use it in the
 * flight model.
 */
#define GRAVITY_ACC_M_PER_S2 -9.794f

void calculate_positions(struct match_state *, Uint32);
struct vector3 calc_velocity_towards_position(struct vector3 *,
                                              struct vector3 *,
                                              float,
                                              float);

#endif /* PHYSICS_H_ */
