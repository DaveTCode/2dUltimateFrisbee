/*
 * automaton_timed_event_queue.h
 *
 *  Created on: 11 Jun 2010
 *      Author: David
 */

#ifndef AUTOMATON_TIMED_EVENT_QUEUE_H_
#define AUTOMATON_TIMED_EVENT_QUEUE_H_

#include <stdbool.h>
#include "SDL/SDL_stdinc.h"

struct match_state;

/*
 * AUTOMATON_TIMED_EVENT_QUEUE
 *
 * A sorted queue of events that are popped at specific times. Events are
 * entered in sorted order and periodically the top of the queue is checked to
 * see whether the event should get thrown yet. If so then that event is thrown
 * to whatever players it is set with and it is removed from the queue.
 *
 * NOTE: The time that events get thrown isn't necessarily going to be exact to
 * 1 millisecond. In fact it can be out by as much as one full frame.
 *
 * top - The first element in the queue.
 */
typedef struct automaton_timed_event_queue
{
  struct automaton_timed_event_queue_node *top;
} AUTOMATON_TIMED_EVENT_QUEUE;

/*
 * AUTOMATON_TIMED_EVENT_QUEUE_NODE
 *
 * A single node in the event queue. Consists of an event, the time at which
 * the event was added and the length of the event.
 *
 * next - Set to NULL if this is the end of the queue.
 * event - The event to which this corresponds.
 * length - The length of time this event should remain on the queue.
 * start - The time at which the event was added to the queue.
 * player_id - The player for whom this event will be thrown. Only valid if
 *             all_players == false.
 * team_id - The team that the player belongs to.
 * all_players - Set to true if this event should get thrown to ALL players.
 */
typedef struct automaton_timed_event_queue_node
{
  struct automaton_timed_event_queue_node *next;
  struct automaton_event *event;
  Uint32 length;
  Uint32 start;
  int player_id;
  int team_id;
  bool all_players;
} AUTOMATON_TIMED_EVENT_QUEUE_NODE;

AUTOMATON_TIMED_EVENT_QUEUE *create_automaton_timed_event_queue();
void destroy_automaton_timed_event_queue(AUTOMATON_TIMED_EVENT_QUEUE *);
void pop_all_timed_events(AUTOMATON_TIMED_EVENT_QUEUE *,
                          Uint32,
                          struct match_state *);


#endif /* AUTOMATON_TIMED_EVENT_QUEUE_H_ */
