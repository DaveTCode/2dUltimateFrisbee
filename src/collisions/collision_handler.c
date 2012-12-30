/*
 * collision_handler.c
 *
 * This file contains functions that relate to collision handling between the
 * players and also player disc collisions.
 *
 * For information on how the collision handler works and why this method was
 * chosen see the documentation.
 *
 *  Created on: 28-Jan-2010
 *      Author: DAT
 */

#include "../dt_logger.h"

#include <stdbool.h>
#include "../game_functions.h"
#include "../team.h"
#include "../disc.h"
#include "../player.h"
#include "../data_structures/vector.h"
#include "collision_handler.h"

/*
 * test_cube_collision
 *
 * This is private utility function for this file to use in its collision
 * detection. It is the most simple and obvious way of performing a bounding
 * box test on two objects. If this returns true then the bounding boxes
 * intersect in some way.
 *
 * Parameters: x11,y11,z11 - The top left of object A.
 *             x12,y12,z12 - The opposite corner of object A.
 *             x21,y21,z21 - The top left of object B.
 *             x22,y22,z22 - The opposite corner of object B.
 *
 * Returns: True if the cubes intersect.
 */
bool test_cube_collision(VECTOR3 *obj1_cornerA,
                         VECTOR3 *obj1_cornerB,
                         VECTOR3 *obj2_cornerA,
                         VECTOR3 *obj2_cornerB)
{
  if (obj2_cornerB->x < obj1_cornerA->x ||
      obj2_cornerA->x > obj1_cornerB->x) return false;
  if (obj2_cornerB->y < obj1_cornerA->y ||
      obj2_cornerA->y > obj1_cornerB->y) return false;
  if (obj2_cornerB->z < obj1_cornerA->z ||
      obj2_cornerA->z > obj1_cornerB->z) return false;

  return true;
}

/*
 * detect_and_handle_collisions
 *
 * The main entrance point for the collision handler. This function DOES
 * modify the player coordinates directly.
 *
 * The ordering of collision detection and handling in this function is mostly
 * unimportant; although we have not decoupled the detection and handling
 * stages as we should the likelihood of it having a negative effect on the
 * game is extremely minimal. See documentation for a full discussion of
 * potential problems.
 *
 * Parameters: teams - Contains all the players in the game.
 *             players_per_team - The number of players in each team.
 *             disc - The disc object in use during the game.
 */
void detect_and_handle_collisions(TEAM **teams,
                                  int players_per_team,
                                  DISC *disc)
{
  /*
   * Local Variables.
   */
  int ii;

  /*
   * Player player collisions detected and handled first.
   */
  detect_player_player_collisions(teams, players_per_team);

  /*
   * Once player player collisions all resolved do disc player collisions.
   */
  handle_player_disc_collisions(teams, players_per_team, disc);

  /*
   * Commit the new positions as they have now been resolved to avoid
   * conflicts.
   */
  for (ii = 0; ii < players_per_team; ii++)
  {
    complete_player_coords_update(teams[0]->players[ii]);
    complete_player_coords_update(teams[1]->players[ii]);
  }
  vector_copy_values(&(disc->position), &(disc->new_position));
}

void detect_player_player_collisions(TEAM *teams[], int players_per_team)
{
  /*
   * Local Variables.
   */
  VECTOR3 player1_cornerA;
  VECTOR3 player1_cornerB;
  VECTOR3 player2_cornerA;
  VECTOR3 player2_cornerB;
  int ii;
  int jj;
  int kk;
  int ll;

  /*
   * For each player loop through the REMAINING players detecting for possible
   * collisions.
   */
  for (ii = 0; ii < players_per_team; ii++)
  {
    for (jj = 0; jj < 2; jj++)
    {
      player1_cornerA.x = teams[jj]->players[ii]->new_position.x;
      player1_cornerA.y = teams[jj]->players[ii]->new_position.y;
      player1_cornerA.z = teams[jj]->players[ii]->new_position.z;
      player1_cornerB.x = player1_cornerA.x + teams[jj]->players[ii]->width_x;
      player1_cornerB.y = player1_cornerA.y + teams[jj]->players[ii]->width_y;
      player1_cornerB.z = player1_cornerA.z + teams[jj]->players[ii]->height;

      for (kk = ii + 1; kk < players_per_team; kk++)
      {
        for (ll = 0; ll < 2; ll++)
        {
          player2_cornerA.x = teams[ll]->players[kk]->new_position.x;
          player2_cornerA.y = teams[ll]->players[kk]->new_position.y;
          player2_cornerA.z = teams[ll]->players[kk]->new_position.z;
          player2_cornerB.x = player2_cornerA.x + teams[ll]->players[kk]->width_x;
          player2_cornerB.y = player2_cornerA.y + teams[ll]->players[kk]->width_y;
          player2_cornerB.z = player2_cornerA.z + teams[ll]->players[kk]->height;

          /*
           * Basic test is just a bounding box test around the cube that contains
           * each player.
           */
          if (test_cube_collision(&player1_cornerA,
                                  &player1_cornerB,
                                  &player2_cornerA,
                                  &player2_cornerB))
          {
            handle_player_player_collision(teams[jj]->players[ii],
                                           teams[ll]->players[kk]);
          }
        }
      }
    }
  }
}

/*
 * handle_player_player_collision
 *
 * This function is called with a pair of players which have had their
 * bounding boxes collide.
 *
 * At present this is handled simply by putting them back to where they were
 * before the collision took place. This is a very simplistic way of dealing
 * with the problem but should be reasonably realistic for small update times.
 *
 * Parameters: player1, player2 - Ordering of players in parameters is
 *                                irrelevant to this function.
 */
void handle_player_player_collision(PLAYER *player1, PLAYER *player2)
{
  //DT_DEBUG_LOG("Collision occurred between players (%i:%i) and (%i:%i)\n",
  //             player1->team_id, player1->player_id,
  //             player2->team_id, player2->player_id);
  //revert_player_coords_update(player1);
  //revert_player_coords_update(player2);
}

/*
 * handle_player_disc_collisions
 *
 * At the moment, if a player collides with the disc then all that happens is
 * an event is thrown to the AI which will get added to the queue. It is up to
 * the player AI whether they catch it or it becomes a turnover.
 *
 * Parameters: teams - The team objects hold all the player objects in the game.
 *             players_per_team - Required to iterate over the array of players.
 *             disc - The disc object.
 */
void handle_player_disc_collisions(TEAM **teams,
                                   int players_per_team,
                                   DISC *disc)
{
  /*
   * Local Variables.
   */
  VECTOR3 player_cornerA;
  VECTOR3 player_cornerB;
  VECTOR3 disc_cornerA;
  VECTOR3 disc_cornerB;
  int ii;
  int jj;

  /*
   * Set up the corners of the discs bounding box.
   */
  vector_copy_values(&(disc_cornerA), &(disc->new_position));
  disc_cornerB.x = disc_cornerA.x + disc->diameter_m;
  disc_cornerB.y = disc_cornerA.y + disc->diameter_m;
  disc_cornerB.z = disc_cornerA.z + disc->diameter_m;

  for (ii = 0; ii < players_per_team; ii++)
  {
    for (jj = 0; jj < 2; jj++)
    {
      /*
       * Set up the corners of the players bounding box.
       */
      player_cornerA.x = teams[jj]->players[ii]->new_position.x;
      player_cornerA.y = teams[jj]->players[ii]->new_position.y;
      player_cornerA.z = teams[jj]->players[ii]->new_position.z;
      player_cornerB.x = player_cornerA.x + teams[jj]->players[ii]->width_x;
      player_cornerB.y = player_cornerA.y + teams[jj]->players[ii]->width_y;
      player_cornerB.z = player_cornerA.z + teams[jj]->players[ii]->height;

      if (ii == 1)
      {
        ii = 1;
      }

      /*
       * Test the bounding boxes of the player and the disc. If the bounding boxes
       * intersect then we declare it a collision.
       *
       * Note that the bounding box around the disc is not done properly as the
       * diameter is used as the height in the z direction. It would be better to
       * use a bounding box oriented to the direction of the disc.
       */
      if (test_cube_collision(&player_cornerA,
                              &player_cornerB,
                              &disc_cornerA,
                              &disc_cornerB))
      {
        /*
         * On collision pass control of the player to the user providing that
         * no other player is already holding the disc.
         */
        if ((disc->disc_state != disc_with_player) &&
            (disc->thrower != teams[jj]->players[ii]))
        {
          DT_DEBUG_LOG("Player (%i:%i) has collided with the disc\n",
                       teams[jj]->players[ii]->team_id,
                       teams[jj]->players[ii]->player_id);

          /*
           * For now just let the player catch the disc.
           */
          catch_disc(teams[jj]->players[ii], disc);
        }
      }
    }
  }
}
