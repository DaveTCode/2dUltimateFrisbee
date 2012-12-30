/*
 * event_queue.c
 *
 *  Created on: 01 Feb 2010
 *      Author: David Tyler
 */

#include "../dt_logger.h"

#include <stdbool.h>
#include "event_queue.h"
#include "../automaton/data_structures/automaton_event.h"

/*
 * destroy_event_queue_node
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: event_queue_node - The object to be freed.
 */
void destroy_event_queue_node(EVENT_QUEUE_NODE *event_queue_node)
{
  /*
   * Free the object.
   */
  DT_FREE(event_queue_node);
}

/*
 * create_event_queue_node
 *
 * Allocates the memory required for a event_queue_node.
 *
 * Returns: A pointer to the newly created memory
 */
EVENT_QUEUE_NODE *create_event_queue_node()
{
  /*
   * Local Variables.
   */
  EVENT_QUEUE_NODE *node;

  /*
   * Allocate the memory required for the new node.
   */
  node = (EVENT_QUEUE_NODE *) DT_MALLOC(sizeof(EVENT_QUEUE_NODE));

  /*
   * Default the pointers to NULL so that we can test against them if required.
   */
  node->next = NULL;
  node->prev = NULL;

  return(node);
}

/*
 * create_event_queue
 *
 * Allocates the memory required for a event_queue. Also sets the default NULL
 * values for all pointers in the queue object.
 *
 * Returns: A pointer to the newly created memory
 */
EVENT_QUEUE *create_event_queue()
{
  /*
   * Local Variables
   */
  EVENT_QUEUE *event_queue;

  /*
   * Allocate the required memory
   */
  event_queue = (EVENT_QUEUE *) DT_MALLOC(sizeof(EVENT_QUEUE));

  /*
   * Defaulting the pointers and the size means that we can use this queue
   * immediately after having called this constructor.
   */
  event_queue->top = NULL;
  event_queue->bottom = NULL;

  return(event_queue);
}

/*
 * destroy_event_queue
 *
 * Frees the memory used by the passed in object.
 *
 * Parameters: event_queue - The object to be freed.
 */
void destroy_event_queue(EVENT_QUEUE *event_queue)
{
  /*
   * Local Variables.
   */
  EVENT_QUEUE_NODE *curr;
  EVENT_QUEUE_NODE *next;

  /*
   * If we are destroying the queue object then we want to destroy all the
   * nodes so we set the size to zero and clear the queue.
   */
  curr = event_queue->top;
  while (NULL != curr)
  {
    next = curr->next;
    destroy_event_queue_node(curr);
    curr = next;
  }

  /*
   * Free the object.
   */
  DT_FREE(event_queue);
}

/*
 * add_event_to_queue
 *
 * Add an event to the queue.
 *
 * Parameters: event - The event to be added.
 *             queue - Must be created but can be an empty queue.
 */
void add_event_to_queue(AUTOMATON_EVENT *event, EVENT_QUEUE *queue)
{
  /*
   * Local Variables.
   */
  EVENT_QUEUE_NODE *node = create_event_queue_node();

  /*
   * Set the nodes event.
   */
  node->event = event;

  /*
   * If the queue is empty then the top will be null. Otherwise,
   * the top will be a pointer to a queue event.
   */
  if (NULL == queue->top)
  {
    /*
     * Queue was empty so point both the top and bottom
     * at this event.
     */
    queue->bottom = node;
    queue->top = node;
    node->next = NULL;
    node->prev = NULL;
  }
  else
  {
    /*
     * There is an element on the top of the queue already. Add this element
     * to the top and push the others down 1.
     */
    queue->top->prev = node;
    node->next = queue->top;
    node->prev = NULL;
    queue->top = node;
  }
}

/*
 * get_event_from_queue
 *
 * Retrieve (and delete) the last element on the queue.
 *
 * Parameters: queue - The queue object to retrieve from.
 *             event - Will contain the event returned. Set to NULL if the
 *                     the queue was empty.
 *
 * Returns: false if the queue was empty and true otherwise.
 */
bool get_event_from_queue(EVENT_QUEUE *queue, AUTOMATON_EVENT **event)
{
  /*
   * Local Variables.
   */
  bool is_empty = false;

  /*
   * The queue is empty if the bottom of the queue is a NULL pointer.
   * In that case we return a NULL event.
   */
  if (NULL == queue->bottom)
  {
    *event = NULL;
    is_empty = true;
  }
  else
  {
    /*
     * If the queue is not empty then remove the bottom event and return it.
     */
    *event = queue->bottom->event;
    queue->bottom = queue->bottom->prev;
    destroy_event_queue_node(queue->bottom);

    /*
     * If there was more than one element in the queue prior to the removal of
     * the bottom element then we need to adjust the next pointer of the new
     * bottom element so that it points to NULL rather than the now deleted
     * element.
     */
    if (NULL != queue->bottom)
    {
      queue->bottom->next = NULL;
    }
    else
    {
      /*
       * We have just taken the last element off the queue. This element was 
       * referenced by the top of the queue as well so we set that to NULL.
       */
      queue->top = NULL;
    }
  }
  
  return(!is_empty);
}
