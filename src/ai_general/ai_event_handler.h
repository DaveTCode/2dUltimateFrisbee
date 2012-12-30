/*
 * ai_event_handler.h
 *
 *  Created on: 29/06/2011
 *      Author: David Tyler
 */

#ifndef AI_EVENT_HANDLER_H_
#define AI_EVENT_HANDLER_H_

struct player;
struct automaton_event;
struct automaton;
struct team;

void throw_single_player_ai_event(struct player *, struct automaton_event *);
void throw_single_player_ai_event_by_name(struct player *,
                                          struct automaton *,
                                          char *);
void throw_multi_player_ai_event(struct team **,
                                 int,
                                 struct automaton_event *);
void throw_multi_player_ai_event_by_name(struct team **,
                                         int,
                                         struct automaton *,
                                         char *);

#endif /* AI_EVENT_HANDLER_H_ */
