/*
 * disc_dimensions.h
 *
 * The hard coded dimensions of a disc. These are used in the disc flight
 * calculations as well as in other places.
 *
 *  Created on: 6 Feb 2010
 *      Author: David
 */

#ifndef DISC_DIMENSIONS_H_
#define DISC_DIMENSIONS_H_

/*
 * These constants define the physical properties of the disc and reasonably
 * accurately represent the properties of a standard disc used in ultimate.
 *
 * @@@DAT - The thickness is currently just a guess as we don't use it...
 */
#define DISC_MASS_KG 0.175f
#define DISC_INERTIA 0.00122f
#define DISC_AXIAL_INERTIA 0.00235f
#define DISC_DIAMETER_M 0.269f
#define DISC_THICKNESS_M 0.05f
#define DISC_AREA_M2 0.057f

#endif /* DISC_DIMENSIONS_H_ */
