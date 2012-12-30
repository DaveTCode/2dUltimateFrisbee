/*
 * throw_constants.h
 *
 * Contains constants relating the initial disc parameters to the user inputs
 * at the beginning of a throw.
 *
 *  Created on: 03-Dec-2009
 *      Author: dat
 */

#ifndef THROW_CONSTANTS_H_
#define THROW_CONSTANTS_H_

/*
 * These constants correspond to the maximum and minimum initial speeds of the
 * disc. They are in m/s.
 */
#define MAX_DISC_SPEED 30.0f
#define MIN_DISC_SPEED 2.0f

/*
 * These constants refer to the range of times that the mouse button can be
 * held for during which the power of the throw is increased. The units are ms.
 *
 * We map these linearly to the disc speed constants by:
 * MIN_TIME_POWER_BUTTON_HELD -> MIN_DISC_SPEED
 * MAX_TIME_POWER_BUTTON_HELD -> MAX_DISC_SPEED
 */
#define MAX_TIME_POWER_BUTTON_HELD 1000.0f
#define MIN_TIME_POWER_BUTTON_HELD 10.0f

/*
 * This ratio is defined as:
 * (MAX_DISC_SPEED - MIN_DISC_SPEED) /
 * (MAX_TIME_POWER_BUTTON_HELD - MIN_TIME_POWER_BUTTON_HELD)
 *
 * It is used to convert the amount of time a button is held down into the
 * initial speed of the disc.
 *
 * !!!MUST BE RECALCULATED EACH TIME THE ABOVE CONSTANTS CHANGE!!!
 */
#define RATIO_BUTTON_TIME_TO_SPEED 0.018f

#endif /* THROW_CONSTANTS_H_ */
