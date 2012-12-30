/*
 * player.h
 *
 * Contains the player structure.
 *
 *  Created on: 17 Oct 2009
 *      Author: DaveT
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <stdbool.h>
#include "data_structures/vector.h"
#include "animation/animation_handler.h"

struct disc;
struct event_queue;

/*
 * PLAYER
 *
 * A structure containing the data specific to a given player object.
 *
 * The position vectors are only accessed by the main thread.
 *
 * The velocity vector is set by the player thread and accessed by the main
 * thread. As such it must not be accessed directly but only by using the
 * wrapper functions in player.c get/set_player_velocity.
 *
 * player_id - A unique id which also acts as an array index.
 * team_id - Either 0 or 1. Corresponds to the team array index.
 * position - A 3d vector giving the position of the player.
 * new_position - A 3d vector giving the new position of the player after the
 *                update.
 * velocity - The velocity vector tells us how to update the player position
 *            each time the location update loop runs.
 * desired_position - Set by the ai. Tells us where the player wants to get to.
 * width_x - The width of the player in m (x direction).
 * width_y - The width of the player in m (y direction).
 * height - The height of the player as measured from top of head to floor.
 * max_speed - The top speed of the player (m/s).
 * current_speed_percent - The percentage of their top speed that the player
 *                         is running at. This can be set by the lua ai
 *                         functions.
 * animation_choice - A value that determines which animation is playing
 * curr_frame - The current frame in the animation.
 * direction - The direction to display the animation in.
 * event_queue - A thread safe queue containing game events that the player has
 *               not yet processed.
 * automaton - The automaton currently being used.
 * automaton_state - A state in the currently used automaton.
 * has_disc - Set to true if the player is holding the disc. False otherwise.
 * is_ai_managed - Set to true if the ai is managing this player and false if
 *                 the user is managing them.
 * marked_player_index - The index of the player in the other team that this 
 *                       player is marking.
 */
typedef struct player
{
  int player_id;
  int team_id;
  VECTOR3 position;
  VECTOR3 new_position;
  VECTOR3 velocity;
  VECTOR3 desired_position;
  float width_x;
  float width_y;
  float height;
  float max_speed;
  float current_speed_percent;
  ANIMATION_CHOICE animation_choice;
  int curr_frame;
  ANIMATION_DIRECTION direction;
  struct event_queue *event_queue;
  struct automaton *automaton;
  struct automaton_state *automaton_state;
  bool has_disc;
  bool is_ai_managed;
  int marked_player_index;
} PLAYER;

PLAYER *create_player(int, int);
void destroy_player(PLAYER *);
void set_player_velocity(PLAYER *, struct vector3 *);
void complete_player_coords_update(PLAYER *);
void revert_player_coords_update(PLAYER *);
bool can_catch_disc(PLAYER *player, struct disc *);

#endif /* PLAYER_H_ */
