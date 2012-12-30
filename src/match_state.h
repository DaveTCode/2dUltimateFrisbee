/*
 * match_state.h
 *
 * Contains the master information on the match.
 *
 *  Created on: 23 Oct 2009
 *      Author: David
 */

#ifndef MATCH_STATE_H_
#define MATCH_STATE_H_

struct automaton_state;
struct automaton_event;
struct automaton_handler;
struct mouse_input_state;
struct key_input_state;
struct disc;
struct pitch;
struct disc_throw;
struct disc_path;
struct team;
struct camera_handler;
struct animation_handler;
struct match_stats;

/*
 * MATCH_STATE
 *
 * An object which contains any information which has a single instance per
 * match.
 *
 * pitch - The pitch object associated with the game.
 * throw - A DISC_THROW object that contains information on the current throw. This
 *         will be null unless there is a throw motion in progress. After the
 *         motion completes all information is passed onto the disc object.
 * disc - Each match only has a single active disc object. This will never be
 *        null after the match has been started.
 * disc_path - The path of the disc object with a small interval. This path is
 *             used to generate debugging information.
 * teams - Contains all the player objects along with team specific info.
 * players_per_team - Saves calculating on the fly.
 * camera_handler - Information on where the camera is currently pointing.
 * mouse_input_state - The current state of mouse buttons.
 * key_input_state - The current state of keys, which are pushed down.
 * animation_handler - Contains references to all player animations.
 * automaton_handler - Contains all the information on ai automatons used in the
 *                     game.
 * match_stats - Statistics relevant to the game. Score, timers etc.
 */
typedef struct match_state
{
  struct pitch *pitch;
  struct disc_throw *match_throw;
  struct disc *disc;
  struct disc_path *disc_path;
  struct team *teams[2];
  int players_per_team;
  struct camera_handler *camera_handler;
  struct mouse_input_state *mouse_input_state;
  struct key_input_state *key_input_state;
  struct animation_handler *animation_handler;
  struct automaton_handler *automaton_handler;
  struct match_stats *match_stats;
} MATCH_STATE;

MATCH_STATE *create_match_state(unsigned int,
                                unsigned int,
                                char *,
                                char *,
                                char *,
                                char *,
                                int(*)(struct automaton_state ***, int),
                                int(*)(struct automaton_event ***));
void destroy_match_state(MATCH_STATE *);

#endif /* MATCH_STATE_H_ */
