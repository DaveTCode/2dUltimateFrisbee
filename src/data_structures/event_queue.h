/*
 * event_queue.h
 *
 * The event queue data structure is completely contained in this c/h file
 * pair.
 *
 *  Created on: 01 Feb 2010
 *      Author: David Tyler
 */

#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_

#include "SDL/SDL.h"

struct automaton_event;

/*
 * EVENT_QUEUE_NODE
 *
 * A node in the event queue. Has previous and next pointers as the event
 * queue is essentially a doubly linked list. All pointers default to NULL in
 * the constructor method.
 *
 * event - The AI_EVENT enum value that caused this to be added to the queue.
 * next - The next element (towards the end we retrieve elements from).
 * prev - The previous element (towards the end we add elements to).
 */
typedef struct event_queue_node
{
  struct automaton_event *event;
  struct event_queue_node *next;
  struct event_queue_node *prev;
} EVENT_QUEUE_NODE;

/*
 * EVENT_QUEUE
 *
 * The event queue is used as a queueing mechanism for passing game
 * events to the AI players. Each AI player has their own event queue and all
 * access should be done through the designated functions.
 *
 * top and bottom pointers default to NULL.
 *
 * top - This is where elements are added.
 * bottom - This is where elements are taken.
 */
typedef struct event_queue
{
  struct event_queue_node *top;
  struct event_queue_node *bottom;
} EVENT_QUEUE;

EVENT_QUEUE *create_event_queue();
void destroy_event_queue(EVENT_QUEUE *);
void add_event_to_queue(struct automaton_event *, EVENT_QUEUE *);
bool get_event_from_queue(EVENT_QUEUE *, struct automaton_event **);

#endif /* EVENT_QUEUE_H_ */
