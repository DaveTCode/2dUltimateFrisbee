/*
 * disc_flight_constants.h
 *
 * These are the constants that are entirely specific to disc flight. It
 * will not include such things as atmospheric density.
 *
 *  Created on: 6 Feb 2010
 *      Author: David
 */

#ifndef DISC_FLIGHT_CONSTANTS_H_
#define DISC_FLIGHT_CONSTANTS_H_

/*
 * These constants pertain to the flight model of a flying disc as described in
 * "Identification of Frisbee aerodynamic coefficients using flight data",
 * Sara A. Hummel * Mont Hubbard 2002
 *
 * Don't tweak these numbers unless you know what you're doing.
 */
#define CONST_L_ZERO     0.2f
#define CONST_L_ALPHA    2.96f
#define CONST_D_ZERO     0.08f
#define CONST_D_ALPHA    2.72f
#define CONST_M_ZERO    -0.02f
#define CONST_M_ALPHA    0.13f
#define CONST_M_Q        0.014f
#define CONST_R_R        0.003f
#define CONST_R_P       -0.013f
#define CONST_N_R       -0.000034f
#define CONST_ALPHA_ZERO 0.06981317f

#endif /* DISC_FLIGHT_CONSTANTS_H_ */
