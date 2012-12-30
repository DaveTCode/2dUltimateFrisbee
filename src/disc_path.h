/*
 * disc_path.h
 *
 * In order to predict where the disc is going to be at a particular time it is
 * necessary to either solve a bunch of PDEs based around the flight model OR
 * simply pre calculate the entire flight.
 *
 * What we do here is to calculate the entire flight in advance by calculating
 * the disc position after certain amounts of milliseconds have passed. If the
 * interval value is too large then the model will be wrong but as the interval
 * tends towards 0 the model becomes more and more accurate of where the disc
 * will be.
 *
 * The path models are used by the AI players to guess where the disc is going
 * to be at a particular time. This means that we can allow for players that are
 * better than others at reading the disc simply by varying the interval time
 * that they create the path with rather than having to write clever functions.
 *
 * This file contains the structure which allow for this to happen. They are
 * explained inline.
 *
 *  Created on: 14 Dec 2009
 *      Author: David Tyler
 */

#ifndef DISC_PATH_H_
#define DISC_PATH_H_

#include "SDL/SDL.h"
#include "data_structures/vector.h"

struct disc;

/*
 * MAX_DISC_PATH_ELEMENTS is the maximum number of steps in a disc path object.
 * It is used to prevent a bug in the disc path generation causing the game to
 * fall over in a heap.
 */
#define MAX_DISC_PATH_ELEMENTS 10000

/*
 * DISC_POSITION_CALC_RET_CODES
 *
 * Return codes for the functions that calculate where the disc will be at time
 * t.
 *
 * DISC_POSITION_CALC_OK - The return code if the function was successful.
 * DISC_POSITION_CALC_INPUT_TIME_BEFORE - The return code if the time inputed
 * is before the start of the throw. This should not happen.
 * DISC_POSITION_CALC_DISC_STOPPED - Returned if the disc will have stopped (
 * i.e. the path will have finished) before the time inputed.
 * DISC_POSITION_CALC_BAD_CALC_TYPE - This indicates a coding error and should
 * be handled as such. It indicates that the enum value passed in to determine
 * which algorithm the calling function requires was not one that is currently
 * handled.
 */
#define DISC_POSITION_CALC_OK                0
#define DISC_POSITION_CALC_INPUT_TIME_BEFORE 1
#define DISC_POSITION_CALC_DISC_STOPPED      2
#define DISC_POSITION_CALC_BAD_CALC_TYPE     3

typedef enum disc_pos_calc_type
{
  INTERPOLATE,
  NEAREST,
  PREVIOUS,
  NEXT
} DISC_POS_CALC_TYPE;

/*
 * Defines the maximum and minimum interval length that we can pass to the init
 * disc path function.
 */
#define DISC_PATH_INTERVAL_MIN 0.001f
#define DISC_PATH_INTERVAL_MAX 0.020f

/*
 * DISC_POSITION
 *
 * A linked list node that holds the position of the disc. A disc path object
 * has a list of these and an interval value that determines the amount of time
 * that elapses between each position.
 *
 * position - The position of the disc at this point along the path.
 * next - The next position in the path.
 * t - The relative time (s)(from creation) for this point. We need this so that
 *     a position can be passed around without referring to the original path.
 */
typedef struct disc_position
{
  VECTOR3 position;
  struct disc_position *next;
  float t;
} DISC_POSITION;

/*
 * DISC_PATH
 *
 * Each ai player creates one of these whenever a new throw is made. The better
 * the ai the smaller the interval value and so the better they will read the
 * disc. The reason for this is that the smaller the interval value the more
 * accurate the path model will be. As it tends towards 0 the path will tend
 * towards a smooth curve showing the disc flight.
 *
 * It is used by the ai players to determine where they need to run in order to
 * intercept the disc.
 *
 * We can also use this to debug the disc flight.
 *
 * interval - The amount of milliseconds between each of the calculations of
 * disc position.
 * time_created - The number of milliseconds that had elapsed when the
 * structure was created.
 * time_to_stop - The value of SDL_GetTicks at the end of the path.
 * start_position - The head of a singly linked list that contains the path for
 * the disc. The last element in the list is the last calculated point before
 * the disc hits the floor.
 * end_position - The final resting point of the disc. This is required as it
 * is unlikely that the disc stopping point corresponds exactly with an
 * interval boundary.
 */
typedef struct disc_path
{
  int interval;
  Uint32 time_created;
  Uint32 time_to_stop;
  DISC_POSITION *start_position;
  DISC_POSITION *end_position;
} DISC_PATH;

DISC_PATH *create_disc_path();
void destroy_disc_path(DISC_PATH *);
void init_disc_path(struct disc *, DISC_PATH *, float);
DISC_POSITION *create_disc_position();
void destroy_disc_position_list(DISC_POSITION *);
int disc_position_at_time(DISC_PATH *,
                          Uint32,
                          DISC_POS_CALC_TYPE,
                          VECTOR3 *);


#endif /* DISC_PATH_H_ */
