/*
 * throw.h
 *
 *  Created on: 23 Oct 2009
 *      Author: David
 */

#ifndef THROW_H_
#define THROW_H_

#include "data_structures/vector.h"

struct mouse_state;
struct disc;

/*
 * THROW
 *
 * Represents a throw in the game, there will only be one of these at a time
 * and it will be stored in the game state object. We use this object to
 * calculate the initial trajectory and power etc for the disc.
 *
 * start_state - A mouse_state object which describes where the mouse was and
 * which button was pressed at what time when the throw started.
 * end_state - A mouse state object which describes all the above for when the
 * throw was released.
 * in_progress - Set to true while a throw is being created. Used so that we
 *               only ever create a single copy of this alongside a match
 *               object.
 */
typedef struct disc_throw
{
  struct mouse_state *start_state;
  struct mouse_state *end_state;
  bool in_progress;
} DISC_THROW;

DISC_THROW *create_throw();
void destroy_throw(DISC_THROW *);
VECTOR3 convert_throw_to_velocity(struct disc *, DISC_THROW *);

#endif /* THROW_H_ */
