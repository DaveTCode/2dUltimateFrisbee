/*
 * automaton_timed_event_queue.c
 *
 *  Created on: 11 Jun 2010
 *      Author: David
 */
#include <stddef.h>

#include "../../dt_logger.h"

#include "automaton_timed_event_queue.h"
#include "../../ai_general/ai_event_handler.h"
#include "../../match_state.h"
#include "../../player.h"
#include "../../team.h"

/*
 * create_automaton_timed_event_queue
 *
 * Allocate the memory required for a timed event queue.
 *
 * Returns: A pointer to the new memory.
 */
AUTOMATON_TIMED_EVENT_QUEUE *create_automaton_timed_event_queue()
{
  /*
   * Local Variables.
   */
  AUTOMATON_TIMED_EVENT_QUEUE *queue;

  /*
   * Allocate memory for the queue.
   */
  queue = (AUTOMATON_TIMED_EVENT_QUEUE *) DT_MALLOC(sizeof(AUTOMATON_TIMED_EVENT_QUEUE));

  /*
   * Set the top to NULL so that we can test that the queue is empty.
   */
  queue->top = NULL;

  return(queue);
}

/*
 * destroy_automaton_timed_event_queue_node
 *
 * Free the memory used for a timed event queue node. This does not free any
 * linked nodes or the event that this refers to. These are owned elsewhere.
 *
 * Parameters: node - The node will be freed.
 */
void destroy_automaton_timed_event_queue_node(AUTOMATON_TIMED_EVENT_QUEUE_NODE *node)
{
  DT_FREE(node);
}

/*
 * destroy_automaton_timed_event_queue
 *
 * Free the memory used in the event queue. This includes all sub nodes.
 *
 * Parameters: queue - Will be completely freed.
 */
void destroy_automaton_timed_event_queue(AUTOMATON_TIMED_EVENT_QUEUE *queue)
{
  AUTOMATON_TIMED_EVENT_QUEUE_NODE *node = queue->top;
  AUTOMATON_TIMED_EVENT_QUEUE_NODE *next_node;

  /*
   * Destroy all the nodes in the queue.
   */
  while (NULL != node)
  {
    next_node = node->next;
    destroy_automaton_timed_event_queue_node(node);
    node = next_node;
  }

  /*
   * Free the object itself.
   */
  DT_FREE(queue);
}

/*
 * create_automaton_timed_event_queue_node
 *
 * Allocate the memory required for a timed event queue node.
 *
 * Returns: A pointer to the newly created memory.
 */
AUTOMATON_TIMED_EVENT_QUEUE_NODE *create_automaton_timed_event_queue_node()
{
  /*
   * Local Variables.
   */
  AUTOMATON_TIMED_EVENT_QUEUE_NODE *node;

  node = (AUTOMATON_TIMED_EVENT_QUEUE_NODE *) DT_MALLOC(sizeof(AUTOMATON_TIMED_EVENT_QUEUE));

  /*
   * Set the next node that this points at to NULL so that we can test where
   * the end of the list is.
   */
  node->next = NULL;

  return(node);
}

/*
 * insert_timed_event_queue_node
 *
 * Adds a new node into the queue. It is inserted in order where the ordering
 * is on start + length i.e. the time at which the event should pop off.
 *
 * Parameters: queue - Can be empty.
 *             node
 */
void insert_timed_event_queue_node(AUTOMATON_TIMED_EVENT_QUEUE *queue,
                                   AUTOMATON_TIMED_EVENT_QUEUE_NODE *node)
{
  /*
   * Local Variables.
   */
  AUTOMATON_TIMED_EVENT_QUEUE_NODE *curr_node = queue->top;
  AUTOMATON_TIMED_EVENT_QUEUE_NODE *prev_node;
  Uint32 time_to_pop = node->length + node->start;

  /*
   * Special case: When the queue is empty the top will be null. In that case
   * we just put this node in at the top.
   */
  if (NULL == curr_node)
  {
    queue->top = node;
  }
  else
  {
    /*
     * Special case: If this element needs to be inserted at the top of the
     * queue then there is nothing to point at it.
     */
    if (curr_node->length + curr_node->start > time_to_pop)
    {
      queue->top = node;
      node->next = curr_node;
    }
    else
    {
      /*
       * Otherwise, we go through the list until we find where this node should
       * be inserted (keeping the list ordered on time to pop).
       */
      prev_node = curr_node;
      curr_node = curr_node->next;

      while (NULL != curr_node)
      {
        if (curr_node->length + curr_node->start > time_to_pop)
        {
          node->next = curr_node;
          prev_node->next = node;
        }
      }

      /*
       * Special case: If the current node is NULL then we reached the end of
       * the queue before being able to write this node in. Therefore we just
       * add it to the end.
       */
      if (NULL == curr_node)
      {
        prev_node->next = node;
        node->next = NULL;
      }
    }
  }
}

/*
 * process_timed_event_node
 *
 * Throws the event to whichever players were registered for it.
 *
 * Parameters: node - Contains the event information.
 *             match_state - Used to retrieve the player information.
 */
void process_timed_event_node(AUTOMATON_TIMED_EVENT_QUEUE_NODE *node,
                              MATCH_STATE *match_state)
{
  if (node->all_players)
  {
    throw_multi_player_ai_event(match_state->teams,
                                match_state->players_per_team,
                                node->event);
  }
  else
  {
    throw_single_player_ai_event(
                    match_state->teams[node->team_id]->players[node->player_id],
                    node->event);
  }
}

/*
 * pop_all_timed_events
 *
 * Goes through the timed queue and pops any events that have passed their
 * waiting time. These get thrown onto the appropriate player event queues for
 * ai processing.
 *
 * Parameters: queue - The queue to retrieve elements from. Can be empty.
 *             t - The current time in ms.
 *             match_state - Used to retrieve player information.
 */
void pop_all_timed_events(AUTOMATON_TIMED_EVENT_QUEUE *queue,
                          Uint32 t,
                          MATCH_STATE *match_state)
{
  /*
   * Local Variables.
   */
  AUTOMATON_TIMED_EVENT_QUEUE_NODE *curr_node = queue->top;

  /*
   * Loop through the queue until we either reach the end OR we find an element
   * that is not ready to pop yet.
   */
  while ((NULL != curr_node) && (curr_node->start + curr_node->length > t))
  {
    /*
     * Process the node to throw any events.
     */
    process_timed_event_node(curr_node, match_state);

    /*
     * All previous elements of the queue will also have popped so set the top
     * to the next element.
     */
    queue->top = curr_node->next;

    /*
     * Free up the resource used in the node that has popped.
     */
    destroy_automaton_timed_event_queue_node(curr_node);
  }
}
