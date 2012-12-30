/*
 * disc.h
 *
 *  Created on: 21 Oct 2009
 *      Author: David Tyler
 */

#ifndef DISC_H_
#define DISC_H_

#include "data_structures/vector.h"

struct player;
struct disc_throw;
struct entity_graphic;

/*
 * These are the different states that the disc can be in.
 */
typedef enum disc_states
{
  disc_in_air,
  disc_with_player,
  disc_on_ground,
  disc_out_of_bounds
} DISC_STATES;

/*
 * DISC
 *
 * A structure that holds information on the single disc object in the game.
 *
 * graphic - The graphical representation of the disc on screen.
 * position - The current position of the disc.
 * new_position - The next position of the disc post update completion.
 * velocity - The velocity of the disc in the world frame of reference.
 * angular_rotation - The rotation around each of the principal disc axis
 *                    d1,d2,d3.
 * angular_velocity - The angular velocity of the disc around the primary axis.
 * diameter_m - The diameter of the disc in meters.
 * thickness_m - The thickness of the disc in meters.
 * disc_state - One of the disc states enum used to determine what the current
 *              state of the disc is.
 * thrower - Keeps track of the last player to throw the disc.
 * holder - Keeps track of the player holding the disc.
 */
typedef struct disc
{
  struct entity_graphic *graphic;
  VECTOR3 position;
  VECTOR3 new_position;
  VECTOR3 velocity;
  VECTOR3 angular_velocity;
  VECTOR3 d3;
  float diameter_m;
  float thickness_m;
  float rotation_angle_rad;
  DISC_STATES disc_state;
  struct player *thrower;
  struct player *holder;
} DISC;

DISC *create_disc(char *);
void destroy_disc(DISC *);
void create_disc_replica(DISC *, DISC *);
void set_init_disc_conditions(DISC *, struct disc_throw *);

#endif /* DISC_H_ */
