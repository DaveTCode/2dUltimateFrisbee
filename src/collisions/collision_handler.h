/*
 * collision_handler.h
 *
 *  Created on: 26 May 2010
 *      Author: David
 */

#ifndef COLLISION_HANDLER_H_
#define COLLISION_HANDLER_H_

struct team;
struct disc;

/*
 * The distance at which objects are said to be interacting. This is
 * unitless in itself but is expected to be measured in meters.
 */
#define DISTANCE_TO_INTERACT 0.1

void detect_and_handle_collisions(struct team **, int, struct disc *);

#endif /* COLLISION_HANDLER_H_ */
